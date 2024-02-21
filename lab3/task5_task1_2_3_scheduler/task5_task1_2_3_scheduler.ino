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
unsigned long smiley_timer = 0;
int task2_counter = 0;
uint8_t smile[4] = {0x64, 0x09, 0x09, 0x52};  // Segments to turn on for all 4-digits to display a smiley

/*******************  Prototypes for Part 5 functions  ********************/
void task3();                                         // Countdown for each decisecond on a 4-digit 7-segment display
void task2();                                         // plays song, rest, then repeat
void task2_2(int freq);                               // plays a song (Mario theme song)                    
void task2_1(int freq);                               // plays a tone 
void task1();                                         // flash an external LED


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
  task5(); 
}

void task5() {
  // Always run task 1 
  task1();
  // Run task 2 twice
  if (task2_counter < 2) {
    task2();
  }
  // Run task 3 after running task 2 twice
  else if (task2_counter == 2) {
    task3();
    start = millis();
  }
  // Run task 2 for the final time
  else if (task2_counter == 3) {
    task2();
    smiley_timer = millis();
  } 
  // Display a smiley face for 2 sec
  else if (millis() - smiley_timer < 2000) {
    task5_1();
  }
}

// Display a smiley face
void task5_1() {
  sevseg.setSegments(smile); 
  sevseg.refreshDisplay(); // Must run repeatedly
}

void task3() {
  static unsigned long timer = millis();
  static int deciSeconds = 30;

  // Count for each decisecond (=100ms)
  if (millis() - timer >= 100) {
    timer += 100;
    // If not finished counting down to 0, display countdown
    if (deciSeconds > 0) {
      deciSeconds--; // 100 milliSeconds is equal to 1 deciSecond
      sevseg.setNumber(deciSeconds, 1);
    } 
    // if finished, then increment counter (to update Task 5 scheduler)
    else {
      task2_counter++;
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
    task2_counter++;
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



