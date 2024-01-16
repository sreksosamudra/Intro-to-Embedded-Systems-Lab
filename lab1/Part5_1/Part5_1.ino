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
// Set frequency
#define FREQ 250
// Set interval time variable (for part 5.1)
unsigned long interval = 200;
// Set duration time variable
unsigned long duration = 3000; // 3 seconds
unsigned long prevTime = 0;
unsigned long startTime = 0;



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN, LED_EXTERNAL and BUZZER as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_EXTERNAL, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  prevTime = millis() - interval;
  startTime = millis();
  //Initial LED state 
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_EXTERNAL, LOW);
}

// the loop function runs over and over again forever
void loop() {
  unsigned long currMillis = millis();
  if(currMillis - prevTime > interval) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));  // turn the onboard LED to the opposite state it's currently in
    digitalWrite(LED_EXTERNAL, !digitalRead(LED_EXTERNAL));   // turn the external LED to the opposite state it's currently in

    prevTime = currMillis;
  }
  if (millis() - startTime < duration) {
    //switch speakers
    digitalWrite(BUZZER,HIGH);
    delay(2);
    digitalWrite(BUZZER,LOW);
    delay(2);  //(1/FREQ/2) * 1000
  }

}

// Other option for: Continuous 250 Hz tone while flashing LEDs
// void loop() {
//   digitalWrite(LED_BUILTIN, HIGH);  
//   digitalWrite(LED_EXTERNAL, LOW);   

//   for (int i = 0; i < 50; i++) {
//     digitalWrite(BUZZER,HIGH);
//     delay(2);
//     digitalWrite(BUZZER,LOW);
//     delay(2);
//   }

//   digitalWrite(LED_BUILTIN, LOW);  
//   digitalWrite(LED_EXTERNAL, HIGH); 

//   for (int i = 0; i < 50; i++) {
//     digitalWrite(BUZZER,HIGH);
//     delay(2);
//     digitalWrite(BUZZER,LOW);
//     delay(2);
//   }

}
