#define prescaler 8

#define E 659 //Hz
#define C 523 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

int counter = 0;
// bool delays = false;
unsigned long start = 0;
unsigned long temp = 0;

/*******************  Prototypes for Part 2 functions  ********************/
void task2();                                         // plays song, rest, then repeat
void play_song(int freq);                             // plays a song (Mario theme song)                    
void play_tone(int freq);                              // plays a tone 

void setup() {
  // Initialize passive buzzer
  pinMode(6, OUTPUT);
 
  // Setup timer
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

  // Setup start time timestamp
  start = millis();
  temp = millis();
}

void loop() {
  task2();
}

// Play song for 2.1 seconds, rest for 6.1 seconds, repeat
void task2() {
  // Play the song
  if (millis() - start < 2100) {
    play_song(melody[counter]);
  }
  // After song is finished, then rest 
  else if (millis() - start < 6100) {
    OCR4A = 0;
  } 
  // Else, reset counter and timestamp
  else {
    counter = 0;
    start = millis();
  }
}

// Play each note in the array. Will result in playing the whole song
void play_song(int freq) {
  if (millis() - temp > 100) {
    counter++;
    temp = millis();
  } else {
    play_tone(melody[counter]);
  }
}

// Play a tone given a frequency
void play_tone(int freq) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = F_CPU / (2 * prescaler * freq);
  }
}
