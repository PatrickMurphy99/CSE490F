// my code to create a cool LED mountain night light
const int LightInput = A0; // for brightness of night light
const int VolcanoRed = 3;
const int VolcanoBlue = 5;
const int VolcanoGreen = 6;
int ColorSwitch = 0; // used in switch statement for light color
unsigned long Time = 0; // used for cool down times for color switching
unsigned long LastEruption = 0; // cool down period for mountain
const int ErruptionInput = A1;
const int ErruptionOutput = 7;
const int ErruptionTone = 8;
const int LakeBlue = 9;
const int LakeGreen = 10;
const int LakeInput = A2;
const int LakeOutput = 12;

// Just sets up pins to be either input or output
// starts some of the pins to just keep running at 5V
void setup() {
  pinMode(LightInput, INPUT_PULLUP);
  pinMode(VolcanoRed, OUTPUT);
  pinMode(VolcanoBlue, OUTPUT);
  pinMode(VolcanoGreen, OUTPUT);
  pinMode(ErruptionInput, INPUT_PULLUP);
  pinMode(ErruptionOutput, OUTPUT);
  digitalWrite(ErruptionOutput, HIGH);
  pinMode(ErruptionTone, OUTPUT);
  pinMode(LakeBlue, OUTPUT);
  pinMode(LakeGreen, OUTPUT);
  pinMode(LakeInput, INPUT_PULLUP);
  pinMode(LakeOutput, OUTPUT);
  digitalWrite(LakeOutput, HIGH);
}

// this is where each of the checks for a component
// of the LED sculpture
void loop() {
  // put your main code here, to run repeatedly:
  int brightness = lightCheckIn();
  volcanoCheck(brightness);
  lakeCheck(brightness);
}

// checks the current brightness of surrondings
// returns int of max voltage to be used
// the darker the room is, the greater the voltage
int lightCheckIn() {
  return (int) map(analogRead(LightInput), 0, 1023, 0, 255);
}


// volcano check
// looks to see if the volcano should erupt
// or if it should change color
void volcanoCheck(int brightness) {
  bool isErrupting = erruptionCheck();
  if (!isErrupting && millis() - Time >= 200) {
    colorPick(brightness);
    Time = millis();
  }
  
}
// determines how to fade the RGB LED
// which color to cycle to
bool colorPick(int brightness) {
  switch (ColorSwitch) {
    case 1:
      turnOn(brightness, 0, 0);
      break;
    case 2:
      turnOn(brightness, brightness, 0);
      break;
    case 3:
      turnOn(0, brightness, 0);
      break;
    case 4:
      turnOn(0, brightness, brightness);
      break;
    case 5:
      turnOn(0, 0, brightness);
      break;
    case 6:
      turnOn(brightness, 0, brightness);
      break;
    default:
      turnOn(brightness, brightness, brightness);
      break;
   }
}

// Eruptioncheck to see if the mountain should erupt
// which is glowing bright red and emitting volcanic sounds
bool erruptionCheck() {
  if (digitalRead(ErruptionInput) == LOW && millis() - LastEruption >= 5000) {
    analogWrite(VolcanoRed, 255);
    analogWrite(VolcanoBlue, 0);
    analogWrite(VolcanoGreen, 0);
    tone(ErruptionTone, 35);
    LastEruption = millis();
    return true;
  } else if (LastEruption != 0 && millis() - LastEruption < 5000) {
    tone(ErruptionTone, map(millis() - LastEruption, 0, 5000, 35, 500));
    return true;
  } else {
    noTone(ErruptionTone);
    return false;
  }
}

// puts the mountain LED to the correct values
void turnOn(int red, int blue, int green) {
  analogWrite(VolcanoRed, red);
  analogWrite(VolcanoBlue, blue);
  analogWrite(VolcanoGreen, green);
  ColorSwitch = (ColorSwitch + 1) % 7;
}

// turns on the lake to the correct value
// reads from the physical input as well
void lakeCheck(int brightness) {
  int greenColor = map(analogRead(LakeInput), 0, 1023, 0, brightness);
  int blueColor = brightness - greenColor;
  analogWrite(LakeGreen, greenColor);
  analogWrite(LakeBlue, blueColor);
}
