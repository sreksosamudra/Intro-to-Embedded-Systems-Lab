#define f1 400
#define f2 250
#define f3 800

#define prescaler 8
#define TIMER4_ON_BIT PRTIM4

int duration = 1000;

/*******************  Prototypes for Part 3.1 functions  ********************/
void bit_set(volatile uint8_t &reg, uint8_t position);             //set required register bits
void bit_clr(volatile uint8_t &reg, uint8_t position);             //clear all required register bits
void play_tone(int freq, int duration);                            //plays a tone for a set of duration
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
  taskC(2000, 4000, 1000);      // 2 sec for TaskA, 4 sec for TaskB, 1 sec doing nothing
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

void play_tone(int freq, int duration) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = F_CPU / (2 * prescaler * freq);
  }
  unsigned long start = millis();
  while (millis() - start < duration) {}
}

void delay_task(int duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {}
}

// LED Sequence
void taskA(int taskA_dur) {
  unsigned long start = millis();

  while(millis() - start < taskA_dur) {
    bit_set(PORTL, PORTL2);         // set pin 47 (or PORTL2) HIGH          
    bit_clr(PORTL, PORTL0);        // set pin 49 (or PORTL0) LOW 
    delay_task(333);
        
    bit_clr(PORTL, PORTL2);       // set pin 47 (or PORTL2) LOW 
    bit_set(PORTL, PORTL1);       // set pin 48 (or PORTL1) HIGH
    delay_task(333);

    bit_clr(PORTL, PORTL1);       // set pin 48 (or PORTL1) LOW 
    bit_set(PORTL, PORTL0);       // set pin 49 (or PORTL0) HIGH
    delay_task(334);
  }
  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);
}

// Timer Tone Output
void taskB(int taskB_dur) {
  unsigned long start = millis();

  while(millis() - start < taskB_dur) {
    play_tone(f1, duration);
    play_tone(f2, duration);
    play_tone(f3, duration);
    play_tone(0, duration);
  }
}

// Control operation for Task A and Task B
void taskC(int taskA_dur, int taskB_dur, int delay_dur) {
  taskA(taskA_dur);
  taskB(taskB_dur);
  delay_task(delay_dur);
}
