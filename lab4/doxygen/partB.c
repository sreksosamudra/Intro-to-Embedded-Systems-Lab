/**
 * @file partB.c
 * @brief This file includes the implementation of executing multiple tasks at the same time, while running a background task at real-time.
 */

#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <arduinoFFT.h>

#define LED_PIN_T1 10
#define BUZZER 6

#define prescaler 8

#define A 440 //Hz
#define B 494 //Hz
#define C 523 //Hz
#define D 587 //Hz
#define E 659 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

int melody[] = {R, A, R, B, R, C, R, E, R, R, C, R, R, C, R, D, R, R, B, R, R, B, R, C, R, R, A, R, R, A, R, B, R};
// R, R, R, R, R, R, E, R, R, C, R, R, C, R, D, R, R, B, R, R, B, R, C, R, R, A, R, R, A, R, B, R, R, R, R, R, R, R, E, R, R, C, R, R, C, R, D, R, R, B, R, R, B, R, C, R, R, A, R, R, A, R, B, R, R, R, E, R, D, R, C, R, R, R, E, R, D, R, C, R, R, R, B, R, R, R, B, R, A, R};   // Bling Bling Bang theme song

// Variables for playing song (Task2)
int counter = 0;
unsigned long temp = 0;
int task2_counter = 0;

// Variables for FFT (Task3)
int N = 64;   // size of array
double array[64];

// Function prototypes for the tasks
void TaskBlink(void *pvParameters);
void TaskPlaySong(void *pvParameters);  // Play song
void TaskPlayNote (int freq); // Play each note in the array. Will result in playing the whole song
void TaskPlayTone(int freq); // Play a tone given a frequency
void TaskRT3p0(void *pvParameters); // Generates an array of N pseudo-random doubles
void TaskRT3p1(void *pvParameters);
void TaskRT4(void *pvParameters);


// Declare a handle for the queue
QueueHandle_t queue1, queue2;

// Declare a handle for tasks
TaskHandle_t RT3p0, RT3p1, RT4;

/**
 * @brief Initializes the hardware and tasks for executing multiple tasks simultaneously.
 *
 * @param None
 * @return n/a
 * @details This function sets up the required hardware components and initializes tasks.
 */
void setup() {
  Serial.begin(921600);
  while (!Serial) {} // Wait for the serial port to connect. Needed for native USB ports only.

  // Setup Timer 4
  TCCR4B = 0;
  TCCR4A = 0;
  TCCR4B |= (1 << WGM42) | (1 << CS41);    // set to CTC mode and prescaler 8
  TCCR4A |= (1 << COM4A0);                // toggle (buzzer) pin when TIMER4_ON_BIT reaches MAX

  // Create a queue to hold int values. Queue length is 1, and each item size is the size of int.
  queue1 = xQueueCreate(1, sizeof(int));
  queue2 = xQueueCreate(1, sizeof(int));

  // Check if the queue was created successfully
  if (queue1 == NULL || queue2 == NULL) {
    Serial.println("Queue creation failed");
    return; // Queue was not created and no memory was available
  }

  // Create tasks
  xTaskCreate(TaskBlink, "Blink", 128, NULL, 4, NULL); // Task for blinking the LED
  xTaskCreate(TaskPlaySong, "Song", 128, NULL, 4, NULL);
  xTaskCreate(TaskRT3p0, "GenerateArray", 128, NULL, 2, &RT3p0);
  xTaskCreate(TaskRT3p1, "PrintTime", 128, NULL, 4, &RT3p1);
  xTaskCreate(TaskRT4, "FFT", 2048, NULL, 4, &RT4);

  vTaskStartScheduler();
}

/**
 * @brief loop function is empty as all work is done in FreeRTOS tasks.
 *
 * @param None
 * @return n/a
 * @details n/a
 */
void loop() {
  // Empty. All work is done in FreeRTOS tasks.
}

/**
 * @brief turn external LED on for 100 ms and off for 200 ms
 *
 * @param pvParameters Pointer to task parameters.
 * @return n/a
 * @details n/a
 */
void TaskBlink(void *pvParameters) {
  pinMode(LED_PIN_T1, OUTPUT);

  for (;;) {
    digitalWrite(LED_PIN_T1, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(LED_PIN_T1, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief goes through the melody array and play the note while the song didn't end for 3 times and pauses 1.5 second between the playbacks.
 *
 * @param pvParameters Pointer to task parameters.
 * @return n/a
 * @details n/a
 */
void TaskPlaySong(void *pvParameters) {
  pinMode(BUZZER, OUTPUT);

  for (;;) {
    if (task2_counter == 3) {
      vTaskSuspend(NULL);
    }
    // Play the song
    if (counter < (sizeof(melody) / sizeof(melody[0]))) {
      TaskPlayNote(melody[counter]);
    }
    // Else, reset counter and timestamp
    else {
      counter = 0;
      task2_counter++;
      vTaskDelay(1500 / portTICK_PERIOD_MS);
    }
  }
}

/**
 * @brief Generates an array of N pseudo-random doubles and suspend itself.
 *
 * @param pvParameters Pointer to task parameters.
 * @return n/a
 * @details n/a
 */
// Generates an array of N pseudo-random doubles
void TaskRT3p0(void *pvParameters) {
   static int min = 1;
   static int max = 10;
  
    // Serial.println("running TaskRT3p0");
    for (int i = 0; i < N; i++) {
      array[i] = random(min, max);
    }
    
    vTaskSuspend(NULL);
    
    // vTaskPrioritySet(RT3p1, 4);
    // vTaskPrioritySet(RT4, 3);
}

/**
 * @brief Prints the total wall clock time for 5 FFTs and suspends itself.
 *
 * @param pvParameters Pointer to task parameters.
 * @return n/a
 * @details This task sends data to a queue for TaskRT4() and waits for the FFT task
 * to signal completion. It calculates the total wall clock time for 5 FFTs and prints 
 * the result to the serial monitor. After that, it suspends itself.
 */
void TaskRT3p1(void *pvParameters) {
  TickType_t time = 0;
  TickType_t total = 0;
  for (;;) {
    for (int i = 0; i < 5; i++) {
      xQueueSend(queue1, &array, portMAX_DELAY);

      // Wait until this task is resumed
      while (xQueueReceive(queue2, &time, portMAX_DELAY) != pdPASS) {}
      total += time;
    }

    Serial.print("Total wall clock time for 5 FFTs: ");
    Serial.println(total - time);

    vTaskDelay(100 / portTICK_PERIOD_MS); // Prevent this task from hogging the CPU

    vTaskSuspend(NULL);
  }
}

/**
 * @brief Performs Fast Fourier Transform (FFT) on received data and sends wall clock time after completion.
 *
 * @param pvParameters Pointer to task parameters.
 * @return n/a
 * @details This task receives data from a queue, performs FFT on the data, calculates the wall clock time 
 * taken for FFT computation, and sends the time to another queue to signal completion.
 */
void TaskRT4(void *pvParameters) {
  int start = 0;
  uint16_t samples = N;
  double signalFrequency  = 1000;
  double samplingFrequency = 5000;   //Nyquist rate
  uint16_t N_SAMPLES = N;

  // double amplitude = 5;
  double cycles = (((samples) * signalFrequency) / samplingFrequency);
  double vReal[N_SAMPLES];
  double vImag[N_SAMPLES];
  for (;;) {
    // Receive the pointer data from Task RT3p1
    if (xQueueReceive(queue1, &array, portMAX_DELAY) == pdPASS) {

      // Start FFT on the data
      for (uint16_t i = 0; i < samples; i++) {
        vReal[i] = int8_t((array[i] * (sin((i * (twoPi * cycles)) / samples))) / 2.0);   /* Build data with positive and negative values*/
        vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
      }
      arduinoFFT fft = arduinoFFT(vReal, vImag, samples, samplingFrequency);

      fft.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      TickType_t start = xTaskGetTickCount();
      fft.Compute(vReal, vImag, samples, FFT_FORWARD);
      TickType_t end = xTaskGetTickCount();
      TickType_t wallClockTime = (end-start) * (1000/configTICK_RATE_HZ);

      // Signal that FFT task has finished
      xQueueSend(queue2, &wallClockTime, portMAX_DELAY);
    }
  }
}

/**
 * @brief Play each note in the array. Will result in playing the whole song
 *
 * @param freq frequency of the note
 * @return n/a
 */
void TaskPlayNote(int freq) {
  if (millis() - temp > 100) {
    counter++;
    temp = millis();
  } else {
    TaskPlayTone(melody[counter]);
  }
}

/**
 * @brief Play a tone given a frequency
 *
 * @param freq frequency of the note
 * @return n/a
 */
// Play a tone given a frequency
void TaskPlayTone(int freq) {
  if (freq == 0) {
    OCR4A = 0;
  } else {
    OCR4A = F_CPU / (2 * prescaler * freq);
  }
}

