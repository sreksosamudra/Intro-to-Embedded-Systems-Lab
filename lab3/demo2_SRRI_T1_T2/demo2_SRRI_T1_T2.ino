/*
This code demonstrates an SRRI Scheduler 
for Task 1 (flashing external LED) and Task 2 (playing Mario theme song)
*/

#define CLK_FREQ 16000000 // Define the clock frequency for timer calculations, 16 MHz for Arduino Mega.
#define CLK_SCALE 1024    // Prescaler value for Timer 1, used to scale down the clock frequency.

#define LED_PIN_TASK1 12
#define BUZZER 6

#define prescaler 8

#define E 659 //Hz
#define C 523 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {R, E, R, E, R, R, E, R, R, C, R, E, R, R, G, R, R, R, R, R, g, R};

// Define task and system states.
#define PENDING 0  // Status flag indicating that an ISR (Interrupt Service Routine) cycle is pending.
#define DONE 1     // Status flag indicating that an ISR cycle is complete.
#define RUNNING 0  // Task state indicating it is currently being executed.
#define READY 1    // Task state indicating it is ready to be executed.
#define SLEEPING 2    // Task state indicating it is currently sleeping (not executing).
#define N_MAX_TASKS 10 // Maximum number of tasks that can be scheduled in the taskArray.

/*******************  Prototypes for Demo 2 functions  ********************/
void (*taskScheduler[N_MAX_TASKS])();                 // Array of function pointers to tasks.
void schedule_sync();                                 // Synchronizes the task scheduler
void sleep_474(int t);                                // Set the current task's state to SLEEP and assign a sleeptime.
void task2();                                         // plays song, rest, then repeat
void task2_2(int freq);                               // plays a song (Mario theme song)                    
void task2_1(int freq);                               // plays a tone 
void task1();                                      // flashing external LED

// initializing arrays for ISR use
int taskSleep[N_MAX_TASKS];              // initializing arrays for sleeping tasks
int taskState[N_MAX_TASKS];              // initializing arrays for task state

volatile int sFlag;                      // Volatile flag used by ISR to signal the main loop about ISR cycle completion.
int task_index;                          // Index of the current task being processed in the loop.

// Variables to control Task 2
int counter = 0;
unsigned long start = 0;
unsigned long temp = 0;

void setup() {
  // Initialize external LED
  pinMode(LED_PIN_TASK1, OUTPUT);

  // Initialize passive buzzer
  pinMode(BUZZER, OUTPUT);
 
  // Setup timer
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

  // Timer 1 configuration for system timing/interrupts.
  OCR1A = CLK_FREQ / (500 * 2 * CLK_SCALE) - 1; // Calculate compare match value for a 2ms period.
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set Timer 1 to CTC mode with a prescaler of 1024.
  TIMSK1 |= (1<<OCIE1A); // Enable Timer 1 Compare Match A Interrupt.

  sei(); // Enable global interrupts for ISR handling.

  // Initialize task management variables and setup initial task states and sleep times.
  task_index = 0; // Start processing tasks from the beginning of the taskArray.
  sFlag = PENDING; // Initial ISR cycle status set to pending.

  // Setup taskArray with specific tasks, their initial sleep times, and states.
  int j = 0; // Task array index for initialization.
  // task1 and task2 is initially set to READY to start execution without delay.
  taskScheduler[j] = &task1; taskSleep[j] = 0; taskState[j] = READY; j++;
  taskScheduler[j] = &task2; taskSleep[j] = 0; taskState[j] = READY; j++;
  // taskScheduler[j] = &task2_off; taskSleep[j] = 250; taskState[j] = SLEEP; j++;
  // // schedule_sync is always READY to adjust task states and sleep times based on the ISR signal.
  taskScheduler[j] = &schedule_sync; taskSleep[j] = 0; taskState[j] = READY; j++;
  taskScheduler[j] = NULL; // Mark the end of task initialization with a NULL pointer.

}

// ISR for Timer 0 Compare Match A, triggered every 2ms based on timer setup.
ISR(TIMER1_COMPA_vect) {
  sFlag = DONE; // Signal that the ISR cycle is complete, allowing schedule_sync to update tasks.
}

void loop() {
  // Main loop cycles through tasks, checking their state and executing READY tasks.
  if (taskState[task_index] == READY) {
    taskState[task_index] = RUNNING;
    (*taskScheduler[task_index])(); // Execute the current READY task.
  }
  task_index++; // Move to the next task in the array.
  // Reset task_index to cycle through tasks continuously.
  if (task_index >= N_MAX_TASKS || taskScheduler[task_index] == NULL) task_index = 0;
}

void schedule_sync() {
  // Wait for ISR to signal that the current cycle is done.
  while (sFlag != DONE) {}
  for (int i = 0; taskScheduler[i] != NULL; i++) {
    // Decrement sleeptime for SLEEPING tasks and update their state to READY if sleeptime has elapsed.
    taskSleep[i] -= 2; // Each cycle represents a 2ms decrement.
    if (taskSleep[i] <= 0) {
      taskSleep[i] = 0;
      taskState[i] = READY; // Task is now READY for execution.
    }
  }
  task_index = -1; // Reset task_index for the next round, ensuring the first task is processed next.
  sFlag = PENDING; // Reset sFlag for the next ISR cycle.
}

void sleep_474(int t) {
  // Set the current task's state to SLEEPING and assign a sleeptime.
  taskState[task_index] = SLEEPING;
  taskSleep[task_index] = t;
}

// Play song for 2.1 seconds, rest for 4 seconds, repeat
void task2() {
  // Play the song
  if (counter < (sizeof(melody) / sizeof(melody[0]))) {
    task2_2(melody[counter]);
  }
  // Else, SLEEP for 4 sec, reset counter and timestamp
  else {
    sleep_474(480); // sleep for 4 sec
    counter = 0;
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
    digitalWrite(LED_PIN_TASK1, HIGH);
  } else {
    digitalWrite(LED_PIN_TASK1, LOW);
  }
}
  
