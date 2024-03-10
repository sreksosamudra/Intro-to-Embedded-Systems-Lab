/**
 * @file partA.c
 * @brief This code demonstrates a simple FreeRTOS application 
for flashing external LED and reading an analog value
 */

#include <Arduino_FreeRTOS.h>

#define LED_PIN_T1 6
int sensorValue;

// define two tasks for Blink & AnalogRead
void TaskBlink1( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

/**
 * @brief Setups the serial port and created tasks that will be performed 
 * 
 * @param None
 * @return n/a
 * @details This function initializes the baud rate and serial port. It also creates the tasks that will be performed in the preemptive scheduler.
 */

// the setup function runs once when you press reset or power the board
void setup() {
  
  // initialize serial communication at 9600 bits per second:
  Serial.begin(19200);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  } 

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink1
    ,  "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  xTaskCreate(
    TaskAnalogRead
    ,  "AnalogRead"
    ,  128  // Stack size
    ,  NULL
    ,  0  // Priority
    ,  NULL );

  vTaskStartScheduler();


}

/**
 * @brief This function is empty because things are done in Tasks.
 * 
 * @param None
 * @return n/a
 */

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

/**
 * @brief Flash an external LED for a period of 100 ms
 * 
 * @param pvParameters pointer to task parameters
 * @return n/a
 */
void TaskBlink1(void *pvParameters)  // This is a task.
{

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_PIN_T1, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    analogWrite(LED_PIN_T1, sensorValue/4);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 100 / portTICK_PERIOD_MS ); // wait for one second
    analogWrite(LED_PIN_T1, 0);    // turn the LED off by making the voltage LOW
    vTaskDelay( 100 / portTICK_PERIOD_MS ); // wait for one second
  }
}

/**
 * @brief FReads an analog input on pin 0, prints the result to the serial monitor.
 * 
 * @param pvParameters pointer to task parameters
 * @return n/a
 */

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
 // (void) pvParameters;

  for (;;)
  {
    // read the input on analog pin 0:
    sensorValue = analogRead(A7);  /// modify for your input pin!
    // print out the value you read:
    Serial.println(sensorValue);
    vTaskDelay(500/portTICK_PERIOD_MS);  // 0.5 sec in between reads for stability
  }
}


