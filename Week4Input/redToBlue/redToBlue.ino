// simple code to make an RGB LED red or blue (or purple)


const int Output = 3; // output voltage
const int RedOutput = 9; // analog write for red
const int BlueOutput = 6; // analog write for blue
const int Input = A0; // the analog reading source

// sets up all the pins, and sets the OutPut to write
void setup() {
  // put your setup code here, to run once:
  pinMode(Output, OUTPUT);
  digitalWrite(Output, HIGH);
  pinMode(RedOutput, OUTPUT);
  pinMode(BlueOutput, OUTPUT);
  pinMode(Input, INPUT);
}

void loop() {
  int readValue = analogRead(Input);
  int redValue = readValue / 4;
  analogWrite(RedOutput, redValue);
  analogWrite(BlueOutput, 255 - redValue);
}
