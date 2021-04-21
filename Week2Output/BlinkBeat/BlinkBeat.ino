// this is the object file for the blinking object
// it represents a light from a specific output
// with a feature that indicates if it fades or blinks
// and determines the rest time (from off mode
// can also sync to other BlinkBeats

class BlinkBeat {

  private:
    int _pin; // the output pin
    bool _isFade; // does this fade or just blink
    int _onTime; // how long it is on fir
    int _coolDownTime; // the off time
    int _Voltage; // max voltage of this pin
    bool _currentState; // true for it is its on
    unsigned long _setTime; // the specific time right now for the object

  public:

  // Constructor
  BlinkBeat(int pin, bool fade , int onTime , int coolDownTime , int Voltage  , bool currentState ) {
    _pin = pin;
    _isFade = fade;
    _onTime = onTime;
    _coolDownTime = coolDownTime;
    _Voltage = Voltage;
    _currentState = currentState;
    _setTime = 0;
    pinMode(_pin, OUTPUT);
  }

  // Syncs this BlinkBeat with theo ther in terms of onTime, coolDownTime, currentState, and setTime
  void sync(BlinkBeat other) {
    _onTime = other._onTime;
    _coolDownTime = other._coolDownTime;
    _currentState = other._currentState;
    _setTime = 0;
    other._setTime = 0;
  }

  //checks status and updates fields, should be called only once per loop
  void checkStatus() {
    delay(500);
    unsigned long timeCheck = millis() - _setTime;
    if (_currentState && timeCheck >= _onTime) {
      _currentState = false;
      digitalWrite(_pin, LOW);
      _setTime = millis();
    } else if (!_currentState && !_isFade && timeCheck >= _coolDownTime) {
      _currentState = true;
      digitalWrite(_pin, HIGH);
      _setTime = millis();
    } else if (_currentState && _isFade) {
      int voltLevel = (int) ((1.0 * _Voltage / _onTime) * (_onTime - timeCheck));
      analogWrite(_pin, voltLevel);
    } else if (!_currentState && _isFade && timeCheck >= _coolDownTime) {
      analogWrite(_pin, _Voltage);
      _currentState = true;
      _setTime = millis();
    }
  }
};

BlinkBeat test1 = BlinkBeat(7, false, 2000, 2000, HIGH, true);
BlinkBeat test2 = BlinkBeat(4, false, 500, 500, HIGH, false);
BlinkBeat test3 = BlinkBeat(10, true, 5000, 5000, 255, true);
BlinkBeat component1 = BlinkBeat(5, true, 10000, 10000, 255, true);
BlinkBeat component2 = BlinkBeat(9, true, 50000, 50000, 100, true);
BlinkBeat component3 = BlinkBeat(11, true, 4000, 4000, 100, false);
void setup() {
  Serial.begin(9600);
}

void loop() {
  test1.checkStatus();
  test2.checkStatus();
  test3.checkStatus();
  component1.checkStatus();
  component2.checkStatus();
  component3.checkStatus();
  if (millis() > 30000 && millis < 31000) {
    test1.sync(test2);
    test3.sync(test1);
  }
  if (millis() > 50000 && millis < 51000) {
    component1.sync(component2);
    component3.sync(component1);
  }
  
}
