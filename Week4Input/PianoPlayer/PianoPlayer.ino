// Object like structure
// I know that objects take linking and loading with .h and .ccp
// but for the sake of practice with normal object conventions
// I am going to write my object structure and main method in this one file
// forgive me, I know I have sinned


// Create a Piano Object
// where user just has to specifiy what key number to play
// and the code will produce the sound
// can also change keys and their frequencies
// and the input
class PianoPlayer {
  private:
    const int _keyNumber; // how many keys there are
    int _keys[100]; // actual keys, shhhh don't tell them its a fixed size
    int _input; // what pin is producing the frequency
    const int MIN_FREQUENCY; // Lowest possible frequency

  public:

    // Constructor
    // assumes that the frequencies have at least as many elements
    // as denoted in keyNumbers
    // if a frequency is below min, sets to min
    // assumes the pin is a valid pin, if not sets to 6
    // if keyNumbers is negative, set it to zero
    PianoPlayer(int keyNumbers, int frequencies[], int input) :
      _keyNumber(max(keyNumbers, 0)) {
      for (int i = 0; i < _keyNumber; i++) {
        _keys[i] = max(MIN_FREQUENCY, frequencies[i]);
      }
      if ( input < 1 || input > 13) {
        _input = 6;
      } else {
        _input = input;
      }
      pinMode(_input, OUTPUT); // weird naming? should I change it?
    }

    // Getter, plays tone from key number played
    // assumes keynumber is valid, and tone is valid to play
    // returns the as an int the frequency played
    // if key is not available, returns -1
    // like all good computer scientists, we index at zero
    int play(int keyNumber) {
      return playDuration(keyNumber, 10);
    }

    // returns true if this note is in the Piano
    // false if not
    bool hasTone(int note) {
      for (int i = 0; i < _keyNumber; i++) {
        if (_keys[i] == note) {
          return true;
        }
      }
      return false;
    }

    // Mutator, changes the frequency at given keyNumber,
    // if index is not possible (as in keyNumber is not in amount of keys)
    // OR if the frequency is not possible (lower than min of 31 Hz)
    // returns -1
    // else returns old value
    int toneChange(int keyNumber, int note) {
      if (_keyNumber <= keyNumber || note < MIN_FREQUENCY) {
        return -1;
      } else {
        int oldNote = _keys[keyNumber];
        _keys[keyNumber] = note;
        return oldNote;
      }
    }

    // Getter, plays tone for a specific amount of time
    // once again keyNumber must be valid or just returns -1
    // returns what frequency is played
    // if duration is negative also just returns -1
    int playDuration(int keyNumber, int duration) {
      if (duration < 0 || keyNumber < 0 || keyNumber > _keyNumber) {
        return -1;
      } else {
        tone(_input, _keys[keyNumber], duration);
        return _keys[keyNumber];
      }
    }

    // setter, just changes input
    // input must be valid between 0-13 will return -1 if not valid
    // else returns old input
    int changeInputz(int input) {
      if (input < 0 || input > 13) {
        return -1;
      } else {
        int oldInput = _input;
        _input = input;
        return oldInput;
      }
    }
};



// ok here is just a set up of my object
int tones[] = {262, 294, 330, 350, 392}; // make the initial tone array
PianoPlayer Player = PianoPlayer(5, tones, 13);
const int Outputs[] = {3, 5, 7, 9, 11}; // output pins
const int Inputs[] = {4, 6, 8, 10, 12}; // input pins
unsigned long TimeDelay[] = {0, 0, 0, 0, 0}; // this is used to determine rebounce
int CurrentStates[] = {HIGH, HIGH, HIGH, HIGH, HIGH}; // current states of button

void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(Outputs[i], OUTPUT);
    digitalWrite(Outputs[i], HIGH);
    pinMode(Inputs[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  pressingKey();
  
}

// cycles through to see which button if any you are pressing,
// and plays noted key
void pressingKey() {
  for (int i = 0; i < 5; i++) {
    debouncedCheck(i);
    if (CurrentStates[i] == LOW) {
      Player.play(i);
      break;
    }
  }
}

// similar to pressingKey, just for extended period of time
void holdingKey(int duration) {
  for (int i = 0; i < 5; i++) {
    debouncedCheck(i);
    if (CurrentStates[i] == LOW) {
      Player.playDuration(i, duration);
      break;
    }
  }
}
// for debouncing
// checks to see if button is pressed again
// only changes state if alloted amount of time passes
void debouncedCheck(int button) {
  unsigned long timeDifference = millis() - TimeDelay[button];
  int readValue = digitalRead(Inputs[button]);
  if (timeDifference > 50 && readValue != CurrentStates[button]) {
    CurrentStates[button] = readValue;
    TimeDelay[button] = millis();
  }
}
  
