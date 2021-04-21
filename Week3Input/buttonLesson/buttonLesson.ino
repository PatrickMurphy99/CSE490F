void setup() {
  // put your setup code here, to run once:
  // this is very simple code to test inputs and outputs
  pinMode(3, OUTPUT);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, OUTPUT);
  digitalWrite(3, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(9, HIGH);
  Serial.begin(9600);

}

void loop() {
  // just to test different button configurations
  if (digitalRead(4) == HIGH) { // simple pull down
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
  }
  if (digitalRead(7) == HIGH) { // simple pull up
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
  }
  if (digitalRead(10) == LOW) { // inverted more complicated pull up
    digitalWrite(11, HIGH);
  } else {
    digitalWrite(11, LOW);
  }
}
