// simple code to make an RGB light goe from Red to Green (sith to jedi)
// has matching sound effect range
// also can plot data on the serial moniter



const int Output = 3; // output voltage
const int RedOutput = 9; // analog write for red
const int GreenOutput = 6; // analog write for green
const int Input = A0; // the analog reading source
const int SoundOutput = 10; // for creating sound
const int LowFrequency = 32;
const int HighFrequency = 350;

// sets up all the pins, and sets the OutPut to write
void setup() {
  // put your setup code here, to run once:
  pinMode(Output, OUTPUT);
  digitalWrite(Output, HIGH);
  pinMode(RedOutput, OUTPUT);
  pinMode(GreenOutput, OUTPUT);
  pinMode(Input, INPUT);
  pinMode(SoundOutput, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int readValue = analogRead(Input);
  int redValue = map(readValue, 0, 1055, 0, 255); // actually using mapping
  analogWrite(RedOutput, redValue);
  analogWrite(GreenOutput, 255 - redValue);
  int frequency = map(readValue, 0, 1055, LowFrequency, HighFrequency);
  tone(SoundOutput, frequency);
  Serial.print(readValue);
  Serial.print(",");
  Serial.println(frequency);
}
