#define LED1 47
#define LED2 48
#define LED3 49

// the setup function runs once when you press reset or power the board
void setup() {
  // PART 1.2 - using pinMode and digitalWrite 
  // initialize digital pins as an output.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  // PART 1.2 - using pinMode and digitalWrite
  digitalWrite(LED1, HIGH); 
  // digitalWrite(LED2, LOW); 
  digitalWrite(LED3, LOW);
  delay(333);      

  digitalWrite(LED1, LOW); 
  digitalWrite(LED2, HIGH); 
  // digitalWrite(LED3, LOW);
  delay(333); 

  // digitalWrite(LED1, LOW); 
  digitalWrite(LED2, LOW); 
  digitalWrite(LED3, HIGH);
  delay(333); 
}

