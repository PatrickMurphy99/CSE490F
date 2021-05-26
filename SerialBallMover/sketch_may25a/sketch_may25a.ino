// sets up the baudrate and sends data corresponding to the joystick and the potentiometer
const int LROUTPUT = 4;
const int LRINPUT = A3;
const int UDOUTPUT = 8;
const int UDINPUT = A2;
void setup() { // sets up all the pins and mode
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LROUTPUT, OUTPUT);
  pinMode(LRINPUT, INPUT);
  pinMode(UDOUTPUT, OUTPUT);
  pinMode(UDINPUT, INPUT);
  digitalWrite(LROUTPUT, HIGH);
  digitalWrite(UDOUTPUT, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  int xData = map(analogRead(LRINPUT), 0, 1000, -10, 10);
  int yData = map(analogRead(UDINPUT), 0, 1000, -10, 10);
  String Data = xData + " " + yData;
  Serial.print(xData);
  Serial.print(" ");
  Serial.println(yData);
  delay(200);
}
