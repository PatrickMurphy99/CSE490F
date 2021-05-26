// first checks what light to turn on
// then checks how bright
// then checks for how long to keep on

const int REDLIGHT = 3;

const int GREENLIGHT = 9;

const int YELLOWLIGHT = 5;

const int BLUELIGHT = 6;

unsigned long RedTime;

unsigned long BlueTime;

unsigned long GreenTime;

unsigned long YellowTime;


// justs sets up the pins and Serial
void setup() {
  // put your setup code here, to run once:
  pinMode(REDLIGHT, OUTPUT);
  pinMode(GREENLIGHT, OUTPUT);
  pinMode(YELLOWLIGHT, OUTPUT);
  pinMode(BLUELIGHT, OUTPUT);
  Serial.begin(9600);
  RedTime = 0;
  BlueTime = 0;
  GreenTime = 0;
  YellowTime = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
      String readValue = Serial.readStringUntil('\n');
      if (readValue.indexOf(' ') == -1) {
        Serial.println("we need a brightness value");
      } else {
        String readLight = readValue.substring(0, readValue.indexOf(' '));
        String sub = readValue.substring(readValue.indexOf(' ') + 1, readValue.length());
        readValue = sub;
        if (readValue.length() <= 0 || readValue.indexOf(' ') == -1) {
          Serial.println("we need a time value");
        } else {
          String readBrightness = readValue.substring(0, readValue.indexOf(' '));
          String readTime = readValue.substring(readValue.indexOf(' ') + 1);
          int ledValue = readLight.toInt();
          if (ledValue != REDLIGHT && ledValue != BLUELIGHT && ledValue != YELLOWLIGHT && ledValue != GREENLIGHT) {
            Serial.println("you need a valid color value"); 
          } else {
            int brightnessAmount = readBrightness.toInt();
            brightnessAmount = constrain(brightnessAmount, 0, 255);
            int timeAmount = readTime.toInt();
            timeAmount = constrain(timeAmount, 0, 10000);
            Serial.println(ledValue);
            Serial.println(brightnessAmount);
            analogWrite(ledValue, brightnessAmount);
            if (ledValue == REDLIGHT) {
              RedTime = millis() + timeAmount;
            } else if (ledValue == BLUELIGHT) {
              BlueTime = millis() + timeAmount;
            } else if (ledValue == YELLOWLIGHT) {
              YellowTime = millis() + timeAmount;
              Serial.println(timeAmount);
            } else {
              GreenTime = millis() + timeAmount;
            }
          }
        }
      }
    } 
  if (millis() > RedTime) {
    analogWrite(REDLIGHT, 0);
  }
  if (millis() > BlueTime) {
     analogWrite(BLUELIGHT, 0);
  }
  if (millis() > GreenTime) {
     analogWrite(GREENLIGHT, 0);
  }
  if (millis() > YellowTime) {
     analogWrite(YELLOWLIGHT, 0);
  }
}
