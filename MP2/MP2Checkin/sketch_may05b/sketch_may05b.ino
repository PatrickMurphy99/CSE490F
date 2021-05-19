#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

uint16_t SCREEN_WIDTH = 128; // OLED display width, in pixels
uint16_t SCREEN_HEIGHT = 64; // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
void setup() { // simple program to manipulate text
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
}

// simply asks users for where they want coordinates for my test message
// was trying to allow users to customize the message, but Strings in Arduino are hard
// they are like this in C/C++, and I would rather focus on the display
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("enter a value for x and then for y");
  delay(1000);
  uint16_t x = 30;
  uint16_t y = 30;
  String message = "Testing for Pat";
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(message);
  display.display();
  delay(1000);
  

}
