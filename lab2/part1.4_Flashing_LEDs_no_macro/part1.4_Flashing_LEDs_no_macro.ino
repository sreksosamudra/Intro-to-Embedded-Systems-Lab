#define LED1 47
#define LED2 48
#define LED3 49

/*******************  Prototypes for Part 1.4 functions  ********************/
void delay_task(int duration);      // runs a delay for a set of duration

// the setup function runs once when you press reset or power the board
void setup() {
  // PART 1.2 - using pinMode and digitalWrite 
  // initialize digital pins as an output.
  // pinMode(LED1, OUTPUT);
  // pinMode(LED2, OUTPUT);
  // pinMode(LED3, OUTPUT);

  //PART 1.4 - using DDR and PORT
  DDRL |= 1 << 2;   //pin 47
  DDRL |= 1 << 1;   //pin 48
  DDRL |= 1 << 0;   //pin 49
}

// the loop function runs over and over again forever
void loop() {
  // PART 1.2 - using pinMode and digitalWrite
  // digitalWrite(LED1, HIGH); 
  // // digitalWrite(LED2, LOW); 
  // digitalWrite(LED3, LOW);
  // delay(333);      

  // digitalWrite(LED1, LOW); 
  // digitalWrite(LED2, HIGH); 
  // // digitalWrite(LED3, LOW);
  // delay(333); 

  // // digitalWrite(LED1, LOW); 
  // digitalWrite(LED2, LOW); 
  // digitalWrite(LED3, HIGH);
  // delay(333); 

  // PART 1.4 - using DDR and PORT
  PORTL |= 1 << PORTL2;         // set pin 47 (or PORTL2) HIGH     
  PORTL &= ~(1 << PORTL0);      // set pin 49 (or PORTL0) LOW 
  delay_task(333);
       
  PORTL &= ~(1 << PORTL2);      // set pin 47 (or PORTL2) LOW 
  PORTL |= 1 << PORTL1;         // set pin 48 (or PORTL1) HIGH
  delay_task(333);

  PORTL &= ~(1 << PORTL1);      // set pin 48 (or PORTL1) LOW 
  PORTL |= 1 << PORTL0;         // set pin 49 (or PORTL0) HIGH
  delay_task(333);
}

/**************************************************************************
  simplified I/O and other functions for this assignment
***************************************************************************/

void delay_task(int duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {}
}

