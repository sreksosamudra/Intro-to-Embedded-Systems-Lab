/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// Set LED_EXTERNAL to pin 10
#define LED_EXTERNAL 10
// Set buzzer to pin 2
#define BUZZER 2

//Source code: https://forum.arduino.cc/t/how-to-program-piezo-buzzer-without-the-tone-library/327434/9 by Jurs
// void myTone(byte pin, uint16_t frequency, uint16_t duration)
// { // input parameters: Arduino pin number, frequency in Hz, duration in milliseconds
//   unsigned long startTime=millis();
//   unsigned long halfPeriod= 1000000L/frequency/2;
//   pinMode(pin,OUTPUT);
//   while (millis()-startTime< duration)
//   {
//     digitalWrite(pin,HIGH);
//     delayMicroseconds(halfPeriod);
//     digitalWrite(pin,LOW);
//     delayMicroseconds(halfPeriod);
//   }
//   pinMode(pin,INPUT);
// }

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN and BUZZER as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  myTone(BUZZER, 250, 3000);
}

// the loop function runs over and over again forever
void loop() {

  //currentMillis -> saves the time stamp
  //if (currentMillis - ) 

  digitalWrite(LED_BUILTIN, HIGH);  // turn the built-in LED on (HIGH is the voltage level)
  digitalWrite(LED_EXTERNAL, LOW);   // turn the external LED off by making the voltage LOW
  // digitalWrite(BUZZER, HIGH);  // make a click sound
  delay(200);                      // wait for 1 second

  digitalWrite(LED_BUILTIN, LOW);   // turn the built-in LED off 
  digitalWrite(LED_EXTERNAL, HIGH);  // turn the external LED on 
  // digitalWrite(BUZZER, LOW);  // turn off click sound
  delay(200);                      // wait for 1 second
}
