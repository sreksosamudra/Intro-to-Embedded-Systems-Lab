#define OP_DECODEMODE  8
#define OP_SCANLIMIT   10
#define OP_SHUTDOWN    11
#define OP_DISPLAYTEST 14
#define OP_INTENSITY   10

#define f1 400
#define f2 250
#define f3 800

#define prescaler 8
#define TIMER4_ON_BIT PRTIM4

#define c 261 // OCR4A = 3830
#define d 294 // OCR4A = 3400
#define e 329 // OCR4A = 3038
#define f 349 // OCR4A = 2864
#define g 392 // OCR4A = 2550
#define a 440 // OCR4A = 2272
#define b 493 // OCR4A = 2028
#define C 523 // OCR4A = 1912
#define R 0
 
int melody[] = { e, R, d, R, c, R, d, R, e, R,e, R,e, R,d, R,d, R,d, R,e, R,g,
R,g, R,e, R,d, R,c, R,d, R,e, R,e, R,e, R,e, R,d, R,d, R,e, R,d, R,c, R,c ,R};


/*******************  Prototypes for Part 4 functions  ********************/
void bit_set(volatile uint8_t &reg, uint8_t position);             //set required register bits
void bit_clr(volatile uint8_t &reg, uint8_t position);             //clear all required register bits
void play_tone(int freq, int duration);                            //plays a tone 
void delay_task(int duration);                                     //runs a delay for a set of duration
void taskA();                                                      //turns ON/OFF an LED sequence
void taskB();                                                      //outputs a tone on timer 
void taskC(int taskA_dur, int delay_dur);                          //controls both task A and task B
void spiTransfer(volatile byte row, volatile byte data);      // SPI command to LED matrix for given row
                                                              //data - bit representation of LEDs in a given row; 1 indicates ON, 0 indicates OFF

// Global variables
// int duration = 1000;
int counter = 0;
bool taskB_only = true;

// change these pins as necessary
int DIN = 51;
int CS =  52;
int CLK = 53;

int mov_col = 0;
int mov_row = 0;

byte spidata[2]; //spi shift register uses 16 bits, 8 for ctrl and 8 for data


void setup() {

  // setup serial port to read A0 and A1 at 9600 baud
  Serial.begin(9600);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  // SPI for LED matrix setup
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  digitalWrite(CS, HIGH);
  spiTransfer(OP_DISPLAYTEST,0);
  spiTransfer(OP_SCANLIMIT,7);
  spiTransfer(OP_DECODEMODE,0);
  spiTransfer(OP_SHUTDOWN,1);

  // Task A setup
  bit_set(DDRL, PL2);     // activate digital pin 47
  bit_set(DDRL, PL1);     // activate digital pin 48
  bit_set(DDRL, PL0);     // activate digital pin 49

  // Task B setup
  bit_set(DDRH, PH3);     // activate digital pin 6
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

}

void loop() {
  taskC(2000, 1000);      // 2 sec for TaskA, 1 sec doing nothing

  // taskB();
  // ledMatrix_task();

}



/**************************************************************************
  simplified I/O and other functions for this assignment
***************************************************************************/
void bit_set(volatile uint8_t &reg, uint8_t position) {
  reg |= 1 << position;
}

void bit_clr(volatile uint8_t &reg, uint8_t position) {
  reg &= ~(1 << position);
}

void play_tone(int freq) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = F_CPU / (2 * prescaler * freq);
  }
}

void delay_task(int duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {}
}

// LED Sequence
void taskA() {
  if (counter % 3 == 0) {
    bit_set(PORTL, PORTL2);         // set pin 47 (or PORTL2) HIGH          
    bit_clr(PORTL, PORTL0);        // set pin 49 (or PORTL0) LOW 
    // delay_task(333);
  } else if (counter % 3 == 1) {
    bit_clr(PORTL, PORTL2);       // set pin 47 (or PORTL2) LOW 
    bit_set(PORTL, PORTL1);       // set pin 48 (or PORTL1) HIGH
    // delay_task(333);
  } else {
    bit_clr(PORTL, PORTL1);       // set pin 48 (or PORTL1) LOW 
    bit_set(PORTL, PORTL0);       // set pin 49 (or PORTL0) HIGH
    // delay_task(334);
  }
  // counter += 1;     // increment after a sequence is finished

}

// Timer Tone Output
void taskB() {
  if (taskB_only) {
    for (int i = 0; i < sizeof(melody)/sizeof(melody[0]); i++) {
      play_tone(melody[i]);
      delay_task(333);
    }
  } else {
    play_tone(melody[counter]);
  }
}

// Control operation for Task A and Task B
void taskC(int taskA_dur, int delay_dur) {

  unsigned long start = millis();

  // Reset variables
  counter = 0;
  taskB_only = false;
  start = millis();


  // Do both tasks simultaneously
  while(millis() - start < sizeof(melody)/sizeof(melody[0]) * 333) {
    unsigned long temp = millis();
    taskB();
    taskA();
    while (millis() - temp < 334) {
      // taskB();
      // taskA();
      ledMatrix_task();
    }
    counter++;
  }

  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);
  // Set buzzer to silent
  play_tone(R);

}

void ledMatrix_task() {
    // Get the column and row index
  convert(analogRead(A0), analogRead(A1));  // A0 for X-direction, A1 for Y-direction
  // Shift the row index value
  mov_row = 1 << mov_row;
  // Turn on the corresponding row and column LED
  spiTransfer(mov_col, mov_row);
  // Turn off the whole row
  spiTransfer(mov_col, 0b00000000);
}

void spiTransfer(volatile byte opcode, volatile byte data){
  int offset = 0; //only 1 device
  int maxbytes = 2; //16 bits per SPI command
  
  for(int i = 0; i < maxbytes; i++) { //zero out spi data
    spidata[i] = (byte)0;
  }
  //load in spi data
  spidata[offset+1] = opcode+1;
  spidata[offset] = data;
  digitalWrite(CS, LOW); //
  for(int i=maxbytes;i>0;i--)
    shiftOut(DIN,CLK,MSBFIRST,spidata[i-1]); //shift out 1 byte of data starting with leftmost bit
  digitalWrite(CS,HIGH);
}

void convert(int analogValue1, int analogValue2) {
  mov_col = analogValue1 * 8/1024;     // X-direction
  mov_row = analogValue2 * 8/1024;     // Y-direction
}
