#define f1 400
#define f2 250
#define f3 800

#define prescaler 8
#define TIMER4_ON_BIT PRTIM4

#define c 3830 // 261 Hz

// Global variables
// int duration = 1000;
int counter = 0;
bool taskB_only = true;



/*******************  Prototypes for Part 3.2 functions  ********************/
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
  taskC(2000, 10000, 1000);      // 2 sec for TaskA, 4 sec for TaskB, 10 sec for simultaneous TaskA & TaskB, 1 sec doing nothing
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
  bit_set(PORTL, PORTL2);         // set pin 47 (or PORTL2) HIGH          
  bit_clr(PORTL, PORTL0);        // set pin 49 (or PORTL0) LOW 
  delay_task(333);
      
  bit_clr(PORTL, PORTL2);       // set pin 47 (or PORTL2) LOW 
  bit_set(PORTL, PORTL1);       // set pin 48 (or PORTL1) HIGH
  delay_task(333);

  bit_clr(PORTL, PORTL1);       // set pin 48 (or PORTL1) LOW 
  bit_set(PORTL, PORTL0);       // set pin 49 (or PORTL0) HIGH
  delay_task(334);

  counter += 1;     // increment after a sequence is finished

}

// Timer Tone Output
void taskB() {
  if (taskB_only) {
    play_tone(f1); delay_task(1000);
    play_tone(f2); delay_task(1000);
    play_tone(f3); delay_task(1000);
    play_tone(0); delay_task(1000);
  } else if (counter % 4 == 0) {
    play_tone(f1);
  } else if (counter % 4 == 1) {
    play_tone(f2);
  } else if (counter % 4 == 2) {
    play_tone(f3);
  } else if (counter % 4 == 3) {
    play_tone(0);
  } else {}
}

// Control operation for Task A and Task B
void taskC(int taskA_dur, int both_dur, int delay_dur) {

  unsigned long start = millis();

  // Do Task A for 2 sec
  while(millis() - start < taskA_dur) {
    taskA();
  }

  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);
  
  // Set to TRUE once we played task B one time
  taskB_only = true;
  // Do Task B once
  taskB();

  counter = 0;
  taskB_only = false;
  // Do both tasks simultaneously
  while(millis() - start < both_dur) {
    taskB();
    taskA();
  }

  // Clear all LEDs
  bit_clr(PORTL, PORTL2); bit_clr(PORTL, PORTL1); bit_clr(PORTL, PORTL0);

  // Do nothing
  delay_task(delay_dur);
}
