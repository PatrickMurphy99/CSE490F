// April 17th
// this is will focus on Serial debugging, fading an LED< and Blinking Two LEDS, input


unsigned long PAST_TIME = 0; // helps determine time difference
int WAIT_TIME = 0; // how long the user requested until fade time
bool OUTPUTS[13]; // represents what outputs are being used

// sets up the Serial port for printig
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (timeCheck(millis())) {
    turnOffInputs();
    WAIT_TIME = updateSettings();
  }
}

// simply measures if the time difference between PAST_TIME and current_time is 
// equal to or larger than the WAIT_TIME
bool timeCheck(unsigned long current_time) {
  return current_time - PAST_TIME >= WAIT_TIME;
}


// simply sets all boolean ports to false
// and turns off all circuits
void turnOffInputs() {
  for (int i = 0; i < 13; i++) {
    if (OUTPUTS[i]) {
      if (analogCheck(i)) {
        analogWrite(i, 0);
      } else {
        digitalWrite(i, LOW);
      }
      OUTPUTS[i] = false;
    }
  }
}

// asks which outlet(s) is being used for the circuit
// if it is an analog output, asks what voltage you would like
// asks for how long until you would be asked again
unsigned long updateSettings() {
  Serial.println("which output would you like on? enter -1 to stop");
  while (true) {
    int input = Serial.parseInt();
    if (input == -1) {
      break;
    } else if (input < 0 || input > 13) {
      Serial.println("Please enter a valid number, or -1 to stop request");
    } else {
      OUTPUTS[input] = true;
    }
  }
  return timeAndVoltageAdjustments();
}

// checks to see if the current output is analog or not
bool analogCheck(int output) {
  if (output == 3 || output == 5 || output == 6 || output == 9 || output == 10 || output == 11) {
    return true;
  } else {
    return false;
  }
}

// adjusts the voltages of the output
// also adjusts the waiting period time
unsigned long timeAndVoltageAdjustments() {
  Serial.println("how many seconds should we wait, give a non negative integer");
  int waitTime = -1;
  while (waitTime <= 0) {
    waitTime = Serial.parseInt() * 1000;
  }
  for (int i = 0; i < 13; i++) {
    if (OUTPUTS[i]) {
      if (analogCheck(i)) {
        Serial.println("what voltage (from 1 to 255) should we have for this output?");
        int answer = 0;
        while (answer == 0) {
          answer = Serial.parseInt();
        }
        analogWrite(i, answer);
      } else {
        digitalWrite(i, HIGH);
      }
    }
  }
  PAST_TIME = millis();
  return waitTime;
}
