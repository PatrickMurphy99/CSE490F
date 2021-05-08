#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
uint16_t SCREEN_WIDTH = 128; // OLED display width, in pixels
uint16_t SCREEN_HEIGHT = 64; // OLED display height, in pixels
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int Yread = A3;
const int Xread = A2;
const int Xout = 5;
const int Yout = 6;

uint16_t xPosition;
uint16_t yPosition;
uint16_t radius;
unsigned long currentTime;
const int AmpPin = 8;
const int VibratingPin = 9;

// simple moving ball
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Display.clearDisplay();
  pinMode(Xread, INPUT);
  pinMode(Yread, INPUT);
  pinMode(Xout, OUTPUT);
  pinMode(Yout, OUTPUT);
  digitalWrite(Xout, HIGH);
  digitalWrite(Yout, HIGH);

  xPosition = 30;
  yPosition = 30;
  radius = 5; // I like this size
  currentTime = millis(); // for smoother animations
  pinMode(AmpPin, OUTPUT);
  digitalWrite(AmpPin, HIGH);
  pinMode(VibratingPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Display.clearDisplay();
  if (millis() - currentTime > 50) { // when to change positions
    xPosition += map(analogRead(Xread), 150, 850, -3, 3);
    yPosition -= map(analogRead(Yread), 150, 850, -3, 3);
    digitalWrite(VibratingPin, LOW);
    currentTime = millis();
  }
  if (xPosition <= 2) { // wrapping distortion
    // could change to bounce
    xPosition = 125.;
    digitalWrite(VibratingPin, HIGH);
  } else if (xPosition >= 126) {
    xPosition = 3;
    digitalWrite(VibratingPin, HIGH);
  }
  if (yPosition <= 2) { /// wrapping distortion
    // could change to bounce
    yPosition = 61;
    digitalWrite(VibratingPin, HIGH);
  } else if (yPosition >= 62) {
    yPosition = 3;
    digitalWrite(VibratingPin, HIGH);
  }
  Display.drawCircle(xPosition, yPosition, radius, SSD1306_WHITE);
  Display.display();
}
