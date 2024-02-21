#include "SevSeg.h" //(Copyright 2020 Dean Reading)
SevSeg sevseg; //Instantiate a seven segment controller object

#define prescaler 8

#define E 659 //Hz
#define C 523 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

int counter = -1;
bool delays = false;
unsigned long start = 0;
unsigned long temp = 0;
int deciSeconds = 40;       //40 ds = 4 sec

/*******************  Prototypes for Part 4 functions  ********************/
void task4();                                         // Counts down in 0.1s for 4 seconds before playing a song
void task3();                                         // Counts-up for each decisecond on a 4-digit 7-segment display
void task2();                                         // plays song, rest, then repeat
void task2_2(int freq);                               // plays a song (Mario theme song)                    
void task2_1(int freq);                                // plays a tone 


void setup() {
  // Initialize passive buzzer
  pinMode(6, OUTPUT);
 
  // Setup timer
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

  // Setup start time timestamp
  // start = millis();
  temp = millis();
  
  // Setup 7-segment
  byte numDigits = 4;
  byte digitPins[] = {22, 23, 24, 25};
  byte segmentPins[] = {26, 27, 28, 29, 30, 31, 32, 33};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  sevseg.setBrightness(90);
}

void loop() {
  task4();
}

void task4() {
  task3();
  if (deciSeconds == 1) {
    start = millis();
  } 
  else if (deciSeconds == 0) {
    task2();
  }
}


void task3() {
  static unsigned long timer = millis();

  // Count for each decisecond (=100ms)
  if (millis() - timer >= 100) {
    timer += 100;
    // If not finished counting down to 0, display countdown
    if (deciSeconds > 0) {
      deciSeconds--; // 100 milliSeconds is equal to 1 deciSecond
      sevseg.setNumber(deciSeconds, 1);
    }
    // else, display the tone frequency
    else {
      sevseg.setNumber(melody[counter], 0);
    }
  }

  sevseg.refreshDisplay(); // Must run repeatedly

}

// Play song for 2.1 seconds, rest for 6.1 seconds, repeat
void task2() {
  // Play the song
  if (millis() - start < 2100) {
    task2_2(melody[counter]);
  }
  // Else, reset counter and timestamp
  else {
    counter = -1;
    start = millis();
    deciSeconds = 40;
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



