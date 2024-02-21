/*
This code demonstrates a simple RR Scheduler 
for Task 1 (flashing external LED) and Task 2 (playing Mario theme song)
*/

#define prescaler 8

#define E 659 //Hz
#define C 523 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

/*******************  Prototypes for Demo 1 functions  ********************/
void task2();                                         // plays song, rest, then repeat
void task2_2(int freq);                               // plays a song (Mario theme song)                    
void task2_1(int freq);                               // plays a tone 
void task1();                                         // flash an external LED

// Variables to control Task 2
int counter = 0;
unsigned long start = 0;
unsigned long temp = 0;

void setup() {
  // Initialize external LED
  pinMode(12, OUTPUT);

  // Initialize passive buzzer
  pinMode(6, OUTPUT);
 
  // Setup timer
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

}

void loop() {
  // RR scheduler
  while (1) {
    task1();
    task2();
    
    // short delay
    delayMicroseconds(10);
  }
}


// Play song for 2.1 seconds, rest for 6.1 seconds, repeat
void task2() {
  // Play the song
  if (millis() - start < 2100) {
    task2_2(melody[counter]);
  }
  // After song is finished, then rest 
  else if (millis() - start < 6100) {
    OCR4A = 0;
  } 
  // Else, reset counter and timestamp
  else {
    counter = -1;
    start = millis();
  }
}

// Play each note in the array. Will result in playing the whole song
void task2_2(int freq) {
  if (millis() - temp > 100) {
    counter++;
    temp = millis();
  } else {
    task2_1(melody[counter]);
  }
}

// Play a tone given a frequency
void task2_1(int freq) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = F_CPU / (2 * prescaler * freq);
  }
}

void task1() {
  if (millis() % 1000 < 250) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
}
