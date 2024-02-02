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
R,g, R,e, R,d, R,c, R,d, R,e, R,e, R,e, R,e, R,d, R,d, R,e, R,d, R,c, R,c };



// Global variables
// int duration = 1000;
int counter = 0;
bool taskB_only = true;



/*******************  Prototypes for Part 3.3 functions  ********************/
void bit_set(volatile uint8_t &reg, uint8_t position);             //set required register bits
void bit_clr(volatile uint8_t &reg, uint8_t position);             //clear all required register bits
void play_tone(int freq, int duration);                            //plays a tone 
void delay_task(int duration);                                     //runs a delay for a set of duration
void taskA();                                                      //turns ON/OFF an LED sequence
void taskB();                                                      //outputs a tone on timer 
void taskC(int taskA_dur, int delay_dur);                          //controls both task A and task B

void setup() {
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
}


// LED Sequence
void taskA() {
  if (counter % 3 == 0) {
    bit_set(PORTL, PORTL2);         // set pin 47 (or PORTL2) HIGH          
    bit_clr(PORTL, PORTL0);        // set pin 49 (or PORTL0) LOW 
    delay_task(333);
  } else if (counter % 3 == 1) {
    bit_clr(PORTL, PORTL2);       // set pin 47 (or PORTL2) LOW 
    bit_set(PORTL, PORTL1);       // set pin 48 (or PORTL1) HIGH
    delay_task(333);
  } else {
    bit_clr(PORTL, PORTL1);       // set pin 48 (or PORTL1) LOW 
    bit_set(PORTL, PORTL0);       // set pin 49 (or PORTL0) HIGH
    delay_task(333);
  }
  counter += 1;     // increment after a sequence is finished

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
  
  // if (!taskB_done) {
  //   play_tone(f1); delay_task(1000);
  //   play_tone(f2); delay_task(1000);
  //   play_tone(f3); delay_task(1000);
  //   play_tone(0); delay_task(1000);

  //   // Set to TRUE once we played task B one time
  //   taskB_done = true;

  // } else if (counter % 4 == 0) {
  //   play_tone(f1);
  // } else if (counter % 4 == 1) {
  //   play_tone(f2);
  // } else if (counter % 4 == 2) {
  //   play_tone(f3);
  // } else if (counter % 4 == 3) {
  //   play_tone(0);
  // } else {}

}

// Control operation for Task A and Task B
void taskC(int taskA_dur, int delay_dur) {

  unsigned long start = millis();

  // Do Task A for 2 sec
  while(millis() - start < taskA_dur) {
    taskA();
  }

  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);
  
  // Set boolean to play task B only
  taskB_only = true;
  // Do Task B once
  taskB();

  // Reset variables
  counter = 0;
  taskB_only = false;
  start = millis();


  // Do both tasks simultaneously
  while(millis() - start < sizeof(melody)/sizeof(melody[0]) * 333) {
    taskB();
    taskA();
  }

  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);
  // Set buzzer to silent
  play_tone(R);


  // Do nothing
  delay_task(delay_dur);


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
