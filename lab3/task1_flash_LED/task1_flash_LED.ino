/*******************  Prototypes for Part 1 functions  ********************/
void task1();                                         // flash an external LED

void setup() {
  // Initialize external LED
  pinMode(12, OUTPUT);
}

void loop() {
  task1();
}

void task1() {
  if (millis() % 1000 < 250) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
}
