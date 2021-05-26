// first checks what light to turn on
// then checks how bright
// then checks for how long

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
    String readLight = Serial.readStringUntil(' ');
    if (Serial.available() > 0) {
      String readBrightness = Serial.readStringUntil(' ');
      if (Serial.available() > 0) {
        String readTime = Serial.readStringUntil('\n');
        int ledValue = readLight.toInt();
        if (ledValue != REDLIGHT && ledValue != BLUELIGHT && ledValue != YELLOWLIGHT && ledValue != GREENLIGHT) {
          Serial.println("you need a valid color value"); 
        } else {
          int brightnessAmount = readBrightness.toInt();
          brightnessAmount = constrain(brightnessAmount, 0, 255);
          int timeAmount = readTime.toInt();
          timeAmount = constrain(timeAmount, 0, 10000);
          analogWrite(ledValue, brightnessAmount);
          if (brightnessAmount == REDLIGHT) {
            RedTime = millis() + timeAmount;
          } else if (brightnessAmount == BLUELIGHT) {
            BlueTime = millis() + timeAmount;
          } else if (brightnessAmount == YELLOWLIGHT) {
            YellowTime = millis() + timeAmount;
          } else {
            GreenTime = millis() + timeAmount;
          }
        }
      } else {
        Serial.println("you need a time integer");
      }
    } else {
      Serial.println("you need a brightness integer");
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
