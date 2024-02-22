/*
This code demonstrates an DDS Scheduler 
for Task 2 (playing Mario theme song), and Task 3 (count up every 100ms)
It demonstrates the following behavior:
  - While no music is being output, the 7-segment display indicates a countdown in 10ths of a second until the next time the theme is played 
  - During the music, the frequency (in decimal Hz.) of the current tone is displayed on the 7-Segment display
*/

#include "SevSeg.h" //(Copyright 2020 Dean Reading)
SevSeg sevseg; //Instantiate a seven segment controller object

#define LED_PIN_TASK1 12
#define BUZZER 6

#define prescaler 8

#define E 659 //Hz
#define C 523 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {R, E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

// Task states definitions, representing the lifecycle of a task.
#define STATE_RUNNING 0 // Task is currently running. 
#define STATE_READY 1 // Task is ready to run. 
#define STATE_SLEEP 2 // Task is waiting for its sleep time to expire before it can run again.
#define STATE_DEAD 3 // Task has completed its function or has been terminated.

#define N_MAX_TASKS 5 // Maximum number of tasks that can be managed by the system.

int task_delays[] = {1, 1}; // Array defining delays for tasks in milliseconds. Adjust these values to change the blinking rate.

// Variables to control Task 2
int counter = 0;
unsigned long temp = 0;
unsigned long start = 0;
int deciSeconds = 40;
// static unsigned long timer = 0;

/*******************  Prototypes for Demo 5 functions  ********************/
void initTask(int id, const char* name, void (*func)(), unsigned short int initialState, unsigned int initialSleep);
void task_self_quit();
void task_start(int taskID);
void task3();                                         // Countdown for each decisecond on a 4-digit 7-segment display
void task2();                                         // plays song, rest, then repeat
void task2_2(int freq);                               // plays a song (Mario theme song)                    
void task2_1(int freq);                               // plays a tone 
void task1_on();                                      // turn on external LED
void task1_off();                                     // turn off external LED

// Structure defining a Task Control Block (TCB), which holds information about each task.
typedef struct TCBstruct {
  int taskID; // Unique identifier for the task within the task list.
  char taskName[20]; // Descriptive name for the task. Useful for debugging and readability.
  int numCalls; // Counts how many times the task has been called. Useful for monitoring task activity.
  unsigned short int state; // Current state of the task (e.g., READY, SLEEP, DEAD).
  void (*functionptr)(); // Pointer to the function that the task needs to execute.
  unsigned int sleeptime; // Time in milliseconds the task needs to wait before next execution.
} task;

task tasklist[N_MAX_TASKS]; // Array to hold tasks. Acts as the Task Control Block (TCB) registry.
int deadlist[N_MAX_TASKS]; // Array to keep track of tasks that have been terminated.
int time_count; // Counter used to simulate time passing.
int quit_flag; // A flag indicating whether a task has requested to terminate itself.

void setup() {
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
  
  Serial.begin(9600);
  // Initializes digital pins connected to LEDs as outputs. Sets the initial state of LEDs to OFF for safety.
  pinMode(LED_PIN_TASK1, OUTPUT);
  // Initialize passive buzzer
  pinMode(BUZZER, OUTPUT);
 
  // Setup timer
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX
  
  time_count = 0; // Reset time counter.
  quit_flag = 0; // Ensure quit flag is reset at start.

  // Initialize the task list with tasks for toggling LEDs. Each task is assigned a function, initial state, and sleep time.
  // The pattern is to have one task for turning an LED on and another for turning it off, creating a blinking effect.
  // initTask(0, "LED1_on", &task1_on, STATE_READY, 0);
  // initTask(1, "LED1_off", &task1_off, STATE_SLEEP, 250);
  // initTask(1, "LED1_off", &task1_off, STATE_SLEEP, 250); // LED2 blinks with a period of 500 ms (250 ms on, 250 ms off).
  initTask(0, "Play_T3", &task3, STATE_READY, 0);
  initTask(1, "Play_T2", &task2, STATE_SLEEP, 4000);
}

void loop() {
  // Main loop iterates over the tasklist, executing tasks based on their state and sleep time.
  for (int i = 0; tasklist[i].functionptr != NULL; i++) {
    // Check if task is ready and sleep time has elapsed.
    if (tasklist[i].state == STATE_READY && tasklist[i].sleeptime == 0) {
      tasklist[i].functionptr(); // Execute the task's function.
      tasklist[i].numCalls++; // Increment the call count for monitoring.
      tasklist[i].state = STATE_SLEEP; // Set task to sleep state.
      tasklist[i].sleeptime = task_delays[i % 2]; // Assign sleep time from the predefined delays array.
    } else if (tasklist[i].state == STATE_SLEEP) {
      // If task is sleeping, decrement its sleep time.
      tasklist[i].sleeptime--;
      if (tasklist[i].sleeptime <= 0) {
        // Once sleep time is up, set task state to ready for next execution.
        tasklist[i].state = STATE_READY;
      }
    }
   
    // If song has finished playing
    if (counter == (sizeof(melody) / sizeof(melody[0]))) {
      tasklist[1].sleeptime = 4000;
      counter = 0;          // reset counter (note)
      deciSeconds = 40;     // reset count-down value
    }

    // Handle task self-termination logic.
    if (quit_flag != 0) {
      tasklist[i].state = STATE_DEAD;
      deadlist[i] = tasklist[i].taskID;
      quit_flag = 0; // Reset quit flag after handling.
    }
  }
  increment(); // Increment the simulated time counter.
}

// Increment function simulates time passing.
void increment() {
  delay(1); // Simulates a 1 ms time step.
  if (time_count < 30000) { // Simulate for a specific duration (30 seconds in this example).
    time_count++;
  } else {
    time_count = 0; // Reset time counter to loop the simulation.
  }
}

void task3() {
  static unsigned long timer = time_count;
  if (time_count - timer >= 100) {
    // timer += 100;
    timer = time_count;
    if (deciSeconds > 0) {
      Serial.println(deciSeconds);
      deciSeconds--;
      sevseg.setNumber(deciSeconds, 1);
    } else {
      sevseg.setNumber(melody[counter], 0);
    }
  }
  sevseg.refreshDisplay(); // Must run repeatedly

}

// Play song for 2.1 seconds, rest for 6.1 seconds, repeat
void task2() {
  // Play the song
  if (counter < (sizeof(melody) / sizeof(melody[0]))) {
    task2_2(melody[counter]);
  }
  // Else, SLEEP for 4 sec, reset counter and timestamp
  else {
    counter = 0;
    deciSeconds = 40;
  }
  
}

// Play each note in the array. Will result in playing the whole song
void task2_2(int freq) {
  if (time_count - temp > 100) {
    counter++;
    temp = time_count;
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

// Utility function to initialize tasks. 
void initTask(int id, const char* name, void (*func)(), unsigned short int initialState, unsigned int initialSleep) {
  tasklist[id].taskID = id;
  strncpy(tasklist[id].taskName, name, sizeof(tasklist[id].taskName) - 1); // Ensure name is null-terminated.
  tasklist[id].functionptr = func;
  tasklist[id].state = initialState;
  tasklist[id].sleeptime = initialSleep;
  tasklist[id].numCalls = 0;
}

// Function to terminate a task from within itself.
void task_self_quit() {
  quit_flag = 1; // Set quit flag to indicate a task wishes to terminate.
}

// Function to restart a terminated task.
void task_start(int taskID) {
  if (deadlist[taskID] != -1) { // Check if task is in the deadlist.
    deadlist[taskID] = -1; // Remove task from deadlist.
    initTask(taskID, tasklist[taskID].taskName, tasklist[taskID].functionptr, STATE_READY, 0); // Reinitialize the task.
  }
}