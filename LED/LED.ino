// April 17th
// this covers the last three lessons
// covering RGB LEDS, crossfaiding, and rate blinking
int RedPin = 4;
int GreenPin = 7;
int BluePin = 8;
void setup() {
  pinMode(RedPin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //digitalWrite(RedPin, HIGH);
  //digitalWrite(GreenPin, HIGH);
  digitalWrite(BluePin, HIGH);
  Serial.println("ON");
  delay(5000);
  //digitalWrite(RedPin, LOW);
  //digitalWrite(GreenPin, LOW);
  digitalWrite(BluePin, LOW);
  Serial.println("OFF");
  delay(5000);
  

}
