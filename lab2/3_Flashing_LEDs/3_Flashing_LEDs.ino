#define f1 400
#define f2 250
#define f3 800

#define prescaler 8
#define TIMER4_ON_BIT PRTIM4

int duration = 1000;

/*******************  Prototypes for Part 2 functions  ********************/
void bit_set(volatile uint8_t &reg, uint8_t position);             //set required register bits
void bit_clr(volatile uint8_t &reg, uint8_t position);             //clear all required register bits

// the setup function runs once when you press reset or power the board
void setup() {
  bit_set(DDRH, PH3);       // activate digital pin 6
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX
}

// the loop function runs over and over again forever
void loop() {
  play_tone(f1, duration);
  play_tone(f2, duration);
  play_tone(f3, duration);
  play_tone(0, duration);
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

