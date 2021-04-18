#include "BlinkBeat.ino"
void setup() {
  BlinkBeat test = new BlinkBeat(4, false, 1000, 1000, HIGH, true);
}

void loop() {
  // put your main code here, to run repeatedly:
  test.checkStatus();
}
