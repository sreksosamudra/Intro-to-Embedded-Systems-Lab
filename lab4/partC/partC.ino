/**
 * @file partC.ino
 * @author Arvin Nusalim and Samantha Reksosamudra
 * @date March 5, 2024
 * @brief This program demonstrates the functionality of Study Buddy. It incorporates a servo motor, LCD display, ultrasonic sensor, and joystick module.
 */

#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Servo.h>
#include <RTClib.h>
#include <LiquidCrystal.h>


#define BUZZER 6
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define SW_PIN   10  // Arduino pin connected to SW  pin

#define prescaler 8

#define A 440 //Hz
#define B 494 //Hz
#define C 523 //Hz
#define D 587 //Hz
#define E 659 //Hz
#define G 784 //Hz
#define g 392 //Hz
#define R 0   //Rest, play no sound

// servo
Servo myservo;  // create servo object to control a servo
int pos = 0;    // variable to read the value from the analog pin

// LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char* quote = "don't give up";
int length = sizeof(quote)/sizeof(quote[0]);

// Ultrasonic sensor
const int trigPin = 7;
const int echoPin = 8;
// defines variables
long duration;
int distance;

// Joystick
int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int switchState = 1; // Switch state is 1 when UNPRESSED, 0 when PRESSED
int time = 0;


// Declare a handle for the queue
QueueHandle_t distanceQueue, setTimeQueue, CDQueue, quoteQueue;

// Declare a handle for tasks
TaskHandle_t wh, md, it, disp, CD, qt, ps;

// Function prototypes for the tasks
void inputTime(void *pvParameters);
void measureDist(void *pvParameters);
void waveHand(void *pvParameters);
void displayLCD(void *pvParameters);
void displayCountdown(void *pvParameters);
void displayQuote(void *pvParameters);
void TaskPlaySong(void *pvParameters);  // Play song
void TaskPlayNote (int freq); // Play each note in the array. Will result in playing the whole song
void TaskPlayTone(int freq); // Play a tone given a frequency

void setup() {
  Serial.begin(921600);
  while (!Serial) {} // Wait for the serial port to connect. Needed for native USB ports only.

  // Create a queue to hold int values. Queue length is 1, and each item size is the size of int.
  distanceQueue = xQueueCreate(1, sizeof(int));
  setTimeQueue = xQueueCreate(1, sizeof(int));
  CDQueue = xQueueCreate(1, sizeof(int));
  quoteQueue = xQueueCreate(1, sizeof(int));

  // Servo setup
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  // LCD setup
  lcd.begin(16, 2);   // Set number of columns and rows
  lcd.print("Enter study time:");

  // Ultrasonic sensor setup
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  // Joystick setup
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);

  // Create tasks
  xTaskCreate(inputTime, "joystick", 128, NULL, 3, &it);
  xTaskCreate(waveHand, "servo", 128, NULL, 2, &wh);
  xTaskCreate(measureDist, "sensor", 128, NULL, 2, &md);
  xTaskCreate(displayLCD, "LCD", 128, NULL, 2, &disp);
  xTaskCreate(displayCountdown, "countdown", 128, NULL, 2, &CD);
  xTaskCreate(displayQuote, "quote", 128, NULL, 1, &qt);

  // Suspend tasks that will be used later on
  vTaskSuspend(qt);
  vTaskSuspend(md);

  vTaskStartScheduler();
}

void loop() {
  // Empty. All work is done in FreeRTOS tasks.
}

void inputTime(void *pvParameters) {
  
  for (;;) {

    if (!digitalRead(SW_PIN) && time != 0) {     // When switch is pressed,
      lcd.clear();                                      // clear the LCD display
      xQueueSend(CDQueue, &time, portMAX_DELAY);        // and send a signal to start the countdown
      
      // Suspend tasks already used and resume the next tasks
      vTaskSuspend(disp);  
      vTaskResume(qt);
      vTaskResume(md);
      vTaskSuspend(it); // suspend this task
    }

    // read analog Y analog values
    yValue = analogRead(VRY_PIN);
    
    // If joystick is at the rightmost position, increment time
    if (yValue > 700) {
      time += 1;
    } else if (yValue < 300) {   // else if joystick is at leftmost position, decrement time
      if (time >= 1) { 
        if (time == 10 || time == 100) {
          lcd.setCursor(0, 1);
          lcd.print("    "); 
        }
        time -= 1;      // avoid negative values
      }  
    }
    xQueueSend(setTimeQueue, &time, portMAX_DELAY);    // Send updated time to be displayed on LCD

    vTaskDelay(200 / portTICK_PERIOD_MS);

  }
}

void measureDist(void *pvParameters) {

  for (;;) {
    
    digitalWrite(trigPin, LOW);   // Clears the trigPin
    delayMicroseconds(2);
    
    digitalWrite(trigPin, HIGH);    // Sets the trigPin on HIGH state for 10 micro seconds
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);    // Reads the echoPin, returns the sound wave travel time in microseconds
    
    distance = duration * 0.034 / 2;      // Calculating the distance

    if (distance < 5) {   // When an object is close to the sensor
      xQueueSend(distanceQueue, &distance, portMAX_DELAY);    // Send signal to move servo motor
      xQueueSend(quoteQueue, &distance, portMAX_DELAY);    // Send signal to display quote on LCD
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void waveHand(void *pvParameters) {

  for (;;) {
    if (xQueueReceive(distanceQueue, &distance, portMAX_DELAY) == pdPASS) { // Wait until receive signal to execute

      myservo.write(90);   // move to position at 120 degrees 
      vTaskDelay(500 / portTICK_PERIOD_MS);
      myservo.write(60);     // move to position at 0 degrees (original position)
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void displayLCD(void *pvParameters) {
  int minute = 0;
  for (;;) {
    if (xQueueReceive(setTimeQueue, &minute, portMAX_DELAY) == pdPASS) {     // Wait until receive time input from joystick 
      // set the cursor to column 0, line 1
      // (note: line 1 is the second row, since counting begins with 0):
      lcd.setCursor(0, 1);

      // print the time input 
      lcd.print(minute);

      lcd.setCursor(4, 1);
      lcd.print("mins");
    }
  }
}

void displayCountdown(void *pvParameters) {
  // Setup initial values
  int minute = 0;
  int second = -1;

  for (;;) {
    // Start the countdown
    if (minute > 0 || second > -1 || (xQueueReceive(CDQueue, &minute, portMAX_DELAY) == pdPASS)) {
      if (second == -1) {   // change of display from 1 minute to 59 seconds
        minute--;
        second = 59;
      }

      if (minute < 10) {      // handles values that change from a two-digit to one-digit (i.e. 10 to 9)
        lcd.setCursor(6, 0);
        lcd.print("0");
        lcd.setCursor(7, 0);
        lcd.print(minute);
      } else if (minute >= 10) {    // prints the 'minutes' value
        lcd.setCursor(6, 0);
        lcd.print(minute);
      }
      lcd.setCursor(8, 0);
      lcd.print(":");

      if (second < 10) {    // handles values that change from a two-digit to one-digit
        lcd.setCursor(9, 0);
        lcd.print("0");
        lcd.setCursor(10, 0);
        lcd.print(second);
      } else if (second >= 10) {  // prints the 'seconds' value
        lcd.setCursor(9, 0);
        lcd.print(second);
      }
      vTaskDelay(1000 / portTICK_PERIOD_MS);    // delay by 1 second

      if (minute == 0 && second == 0) {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("CONGRATZ");
        vTaskDelete(wh);
        vTaskDelete(md);
        vTaskDelete(qt);
        vTaskDelete(CD);
      }

      second--;   // decrement the 'seconds' value
    }
  }
}

void displayQuote(void *pvParameters) {
  char* printingQuote;

  for (;;) {
    if (xQueueReceive(quoteQueue, &distance, portMAX_DELAY) == pdPASS) {   // Wait for signal from ultrasonic sensor
      printingQuote = quote;    // Set the quote to be displayed
      lcd.setCursor(1, 1);      // Set cursor to the bottom line
      lcd.print(printingQuote);   // Print quote to LCD display
      vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay for a second to show the quote
      lcd.setCursor(1, 1);      
      lcd.print("                ");    // Clear the bottom line
    }
  }
}
