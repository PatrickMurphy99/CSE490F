void setup() {
  // put your setup code here, to run once:
  pinMode(6, OUTPUT);
  pinMode(A2, INPUT_PULLUP);
  analogWrite(6, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A2));
  delay(500);
}
