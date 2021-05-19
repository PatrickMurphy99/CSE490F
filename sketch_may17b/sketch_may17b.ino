uint8_t playerOneButtonOutput;
int otherPin; 
void setup() {
  // put your setup code here, to run once:
  playerOneButtonOutput = 6;
  ledcSetup(playerOneButtonOutput, 490, 8);
  pinMode(4, OUTPUT);
  ledcAttachPin(4, playerOneButtonOutput);
  ledcWrite(playerOneButtonOutput, HIGH);
  otherPin = A0;
  pinMode(otherPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(otherPin));
  delay(500);
}
