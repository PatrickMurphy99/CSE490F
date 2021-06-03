#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Tone32.hpp" // fix this!
// this include comes from the makability lab, thank you again
uint16_t SCREEN_WIDTH = 128; // OLED display width, in pixels
uint16_t SCREEN_HEIGHT = 64; // OLED display height, in pixels
int TIME_DELAY = 20;
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

class SnakePlayer {

  private: 
  int startLength; // inital length of the snake
  int snakeLength; // current length of the snake, loses when it goes to zero
  uint16_t* xLocations; // locations of every x component of the snake
  uint16_t* yLocations; // locations of every y component of the snake
  unsigned long startTime; // when the game started
  int radius; // radius of each node in the snake
  int vibrationChannel; // will the vibrations will come from
  // updates both the lights for the snake
  // winningLight increases in brightness as time goes by
  // losingLight increases in brightness the more size has decreased
  

  // updates the x and y locations for each ball
  // each ball is one move behind the one in front (will take place of)
  // if hitts a wall will lose a head and bounce in opposite direction
  void updateBody(int changeInX, int changeInY) {
    uint8_t newXHead = xLocations[0] + 2 * radius * changeInX;
    uint8_t newYHead = yLocations[0] + 2 * radius * changeInY;
    if (snakeLength > 1 && (changeInX != 0 || changeInY != 0)) {
      if (newXHead == xLocations[1] && newYHead == yLocations[1]) {
        for (int i = 0; i < snakeLength - 1; i++) {
          xLocations[i] = xLocations[i + 1];
          yLocations[i] = yLocations[ i + 1];
        }
        xLocations[snakeLength - 1] += 2 * radius * changeInX;
        yLocations[snakeLength - 1] += 2 * radius * changeInY;
        for (int i = 0; i < snakeLength / 2; i++) {
          uint8_t tempX = xLocations[i];
          uint8_t tempY = yLocations[i];
          xLocations[i] = xLocations[snakeLength - 1 - i];
          yLocations[i] = yLocations[snakeLength - 1 - i];
          xLocations[snakeLength - 1 - i] = tempX;
          yLocations[snakeLength - 1 - i] = tempY;
        }
      } else {
        for (int i = snakeLength - 1; i > 0; i--) {
          xLocations[i] = xLocations[i - 1];
          yLocations[i] = yLocations[i - 1];
        }
        xLocations[0] = newXHead;
        yLocations[0] = newYHead;
      }
    }

    // this long if statement just checks if we hit any of the walls
    // if so size goes to zero and snake player loses
    if (xLocations[0] >= (SCREEN_WIDTH - radius) || xLocations[0] <= radius ||
    yLocations[0] >= (SCREEN_HEIGHT - radius) || yLocations[0] <= radius) {
      snakeLength = 0;
      digitalWrite(vibrationChannel, HIGH);
    }
    // writes to the display the locations of all the nodes
    if (snakeLength > 0) {
      Display.fillCircle(xLocations[0], yLocations[0], radius, SSD1306_WHITE);
    }
    for (int i = 1; i < snakeLength; i++) {
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE); 
    }
  }

  public:

  // Constructor
  SnakePlayer(int StartLength, int VibrationChannel,
  unsigned long WinTime, int Radius) {
    xLocations = new uint16_t[StartLength];
    yLocations = new uint16_t[StartLength];
    startLength = StartLength;
    snakeLength = StartLength;
    radius  = Radius;
    vibrationChannel = VibrationChannel;
    startTime = millis();
  }

  // public update call to adust snake
  // the light and the location
  void updateSnake(int changeInX, int changeInY) {
    digitalWrite(vibrationChannel, LOW);
    updateBody(changeInX, changeInY);
  }

  // returns the length of the snake
  int snakeSize() {
    return snakeLength;
  }

  // checks if given points hits the snake, and if so removes node
  void checksBody(uint8_t xSpot, uint8_t ySpot) {
    for (int i = 0; i < snakeLength; i++) {
      if (abs(xLocations[i] - xSpot) <= radius && abs(yLocations[i] - ySpot) <= radius) {
        snakeLength--;
        digitalWrite(vibrationChannel, HIGH);
      }
    }
  }

  // returns true if this snake has died
  // false if not
  bool gameOver() {
    return snakeLength <= 0;
  }

  // sets initial position for the snake player at given x and y
  // each component is two radius away from the next in terms of x
  void setInitial(uint16_t initialX, uint16_t initialY) {
    xLocations[0] = initialX;
    yLocations[0] = initialY;
    Display.fillCircle(xLocations[0], yLocations[0], radius, SSD1306_WHITE);
    for (int i = 1; i < snakeLength; i++) {
      xLocations[i] = initialX - (2 * radius * i);
      yLocations[i] = initialY;
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE);
    }
    Display.display();
  }

  // resets size and time
  void resetSize() {
    snakeLength = startLength;
  }

  // prints out x and y pairs for the central points
  // seperates x and y by commas, then a space between different points
  void printLocations() {
    for (int i = 0; i < snakeLength; i++) {
      Serial.print(xLocations[i]);
      Serial.print(",");
      Serial.print(yLocations[i]);
      Serial.print(" ");
    }
  }

  // stops the vibrations
  void stopVibrations() {
    digitalWrite(vibrationChannel, LOW);
  }
};

const int UDOUTPUT = 14; 
const int UDINPUT = 32;

const int LROUTPUT = 15;
const int LRINPUT = 33; // reverse this one, it is backwards

const int AMP = 27;
const int VIB = 21;

const int BUTTON1OUTPUT = 17;
const int BUTTON1INPUT = 16;

const int BUTTON2OUTPUT = 19;
const int BUTTON2INPUT = 18;

const int REDLIGHT = 5;
const int GREENLIGHT = 4;

Tone32* Tone;

SnakePlayer* Snake1;

SnakePlayer* Snake2;

unsigned long timer = millis();

void setup() {
  // put your setup code here, to run once:
  if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Display.clearDisplay();
  
  pinMode(UDOUTPUT, OUTPUT);
  pinMode(UDINPUT, INPUT);
  digitalWrite(UDOUTPUT, HIGH);

  pinMode(LROUTPUT, OUTPUT);
  pinMode(LRINPUT, INPUT);
  digitalWrite(LROUTPUT, HIGH);

  pinMode(AMP, OUTPUT);
  pinMode(VIB, OUTPUT);
  digitalWrite(AMP, HIGH);
  //digitalWrite(VIB, HIGH); this causes vibrations

  pinMode(BUTTON1OUTPUT, OUTPUT);
  pinMode(BUTTON1INPUT, INPUT);
  digitalWrite(BUTTON1OUTPUT, HIGH);

  pinMode(BUTTON2OUTPUT, OUTPUT);
  pinMode(BUTTON2INPUT, INPUT);
  digitalWrite(BUTTON2OUTPUT, HIGH);

  pinMode(REDLIGHT, OUTPUT);
  pinMode(GREENLIGHT, OUTPUT);

  pinMode(12, OUTPUT); // sound
  ledcSetup(1, 450, 8);
  ledcAttachPin(12, 1);
  Tone = new Tone32(12, 1);
  //(*Tone).playNote(NOTE_C, 4, 10000); it works, remember to update
  Serial.begin(115200);
  Snake1 = new SnakePlayer(3, VIB, 10000, 3);
  Snake2 = new SnakePlayer(3, VIB, 10000, 3);
  (*Snake1).setInitial(40, 40);
  (*Snake2).setInitial(30, 30);
  Display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((*Snake1).gameOver()) {
    while (true) {
    if (millis() - timer > 30) {
      (*Snake1).stopVibrations();
    }
      Serial.println("Lost");
    }
  }
  if (millis() - timer > 50) {
    Display.clearDisplay();
    if (Serial.available() > 0) {
    String snake2X = Serial.readStringUntil(',');
    String snake2Y = Serial.readStringUntil('\n');
    int x2 = snake2X.toInt();
    int y2 = snake2Y.toInt();
    (*Snake2).updateSnake(x2, y2);
  } else {
    (*Snake2).updateSnake(0, 0);
  }
    (*Snake1).updateSnake(moveBody(LRINPUT), moveBody(UDINPUT));
    Display.display();
    timer = millis();
    (*Snake1).printLocations();
    Serial.print("| ");
    (*Snake2).printLocations();
    Serial.println();
  }
}

// reads if we should move left or right
// returns 1, 0, or -1
int moveBody(int readingChannel) {
  if (analogRead(readingChannel) < 1000) {
    return 1;
  } else if (analogRead(readingChannel) > 2000) {
    return -1;
  } else {
    return 0;
  }
}
