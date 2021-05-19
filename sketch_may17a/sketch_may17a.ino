int writer = 5;
int reader = A2;
void setup() {
  // put your setup code here, to run once:
  pinMode(writer, OUTPUT);
  pinMode(reader, INPUT_PULLUP);
  digitalWrite(writer, HIGH);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A2));
  delay(500);
}
