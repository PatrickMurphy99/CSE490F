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
  bool isPlayerOne;
  unsigned long lastButtonTime;
  Tone32 Toner;

  // updates the x and y locations for each ball
  // each ball is one move behind the one in front (will take place of)
  // if hitts a wall will lose a head and bounce in opposite direction
  void updateBody(int changeInX, int changeInY, boolean boosted) {
    int boost = 2;
    if (boosted || millis() - lastButtonTime < 150) {
      boost = 4;
      if (isPlayerOne) {
      digitalWrite(vibrationChannel, HIGH);
      Toner.playNote(NOTE_A, 4);
      }
    }
    uint8_t newXHead = xLocations[0] + boost * radius * changeInX;
    uint8_t newYHead = yLocations[0] + boost * radius * changeInY;
    if (snakeLength > 1 && (changeInX != 0 || changeInY != 0)) {
      if (newXHead == xLocations[1] && newYHead == yLocations[1]) {
        for (int i = 0; i < snakeLength - 1; i++) {
          xLocations[i] = xLocations[i + 1];
          yLocations[i] = yLocations[ i + 1];
        }
        xLocations[snakeLength - 1] += boost * radius * changeInX;
        yLocations[snakeLength - 1] += boost * radius * changeInY;
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
    } else {
      xLocations[0] = newXHead;
      yLocations[0] = newYHead;
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

  // checks if we hit the other snake
  void otherSnakeCheck(SnakePlayer *otherSnake) {
    if (millis() - (*otherSnake).lastButtonTime < 150 && millis() - lastButtonTime >= 150
     && (*otherSnake).snakeLength > 0 && abs((*otherSnake).xLocations[0] - xLocations[0]) < 2 * radius 
     && abs((*otherSnake).yLocations[0] - yLocations[0]) < 2 * radius) {
      snakeLength = 0;
      Toner.playNote(NOTE_A, 4);
      digitalWrite(vibrationChannel, HIGH);      
    } else {
      for (int i = 1; i < (*otherSnake).snakeLength; i++) {
        if (abs((*otherSnake).xLocations[i] - xLocations[0]) < 2 * radius &&
        abs((*otherSnake).yLocations[i] - yLocations[0]) < 2 * radius) {
          snakeLength = 0;
          Toner.playNote(NOTE_A, 4);
          digitalWrite(vibrationChannel, HIGH);
        }
      }
    }
  }

  public:

  int playerNumber;
  // Constructor
  SnakePlayer(int StartLength, int VibrationChannel,
  unsigned long WinTime, int Radius, int PlayerNumber, bool IsPlayerOne, int soundPin, int soundChannel)
    : Toner(soundPin, soundChannel) {
    xLocations = new uint16_t[StartLength];
    yLocations = new uint16_t[StartLength];
    startLength = StartLength;
    snakeLength = StartLength;
    radius  = Radius;
    vibrationChannel = VibrationChannel;
    startTime = millis();
    playerNumber = PlayerNumber;
    isPlayerOne = IsPlayerOne;
    lastButtonTime = 0;
  }

  // public update call to adust snake
  // the light and the location
  void updateSnake(int changeInX, int changeInY, bool speedUp, SnakePlayer *otherSnake) {
    if (isPlayerOne && millis() - lastButtonTime > 90) {
      Toner.update();
      digitalWrite(vibrationChannel, LOW);
    }
    if (speedUp && millis() - lastButtonTime > 150) {
      lastButtonTime = millis();
      snakeLength--;
    }
    updateBody(changeInX, changeInY, speedUp);
    otherSnakeCheck(otherSnake);
  }

  // returns the length of the snake
  int snakeSize() {
    return snakeLength;
  }

  // gives the radius of the snake
  int getRadius() {
    return radius;
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
  void setInitial(uint16_t initialX, uint16_t initialY, int directionFactor) {
    xLocations[0] = initialX;
    yLocations[0] = initialY;
    Display.fillCircle(xLocations[0], yLocations[0], radius, SSD1306_WHITE);
    for (int i = 1; i < snakeLength; i++) {
      xLocations[i] = initialX - (2 * radius * i) * directionFactor;
      yLocations[i] = initialY;
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE);
    }
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
  void stopNoise() {
    digitalWrite(vibrationChannel, LOW);
    Toner.update();
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

const int NOISECHANNEL = 1;
const int SOUNDPIN = 12;

SnakePlayer* Snake1;

SnakePlayer* Snake2;

unsigned long timer = millis();

int Player1Count;

int Player2Count;

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
  Serial.begin(115200);
  Player1Count = 0;
  Player2Count = 0;
  introMenu();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((*Snake1).gameOver() || (*Snake2).gameOver()) {
    gameOverScreen();
  } else if (digitalRead(BUTTON1INPUT) == LOW) {
    pauseMenu();
  } else if (millis() - timer > 30) {
    Display.clearDisplay();
    if (Serial.available() > 0) {
    trimUp();
    String snake2X = Serial.readStringUntil(',');
    String snake2Y = Serial.readStringUntil(' ');
    String snake2B = Serial.readStringUntil('\n');
    int x2 = snake2X.toInt();
    int y2 = snake2Y.toInt();
    int b2 = snake2B.toInt();
    if (x2 == -100 && y2 == -100 && b2 == -100) {
      pauseMenu();
      x2 = 0;
      y2 = 0;
      b2 = 0;
    }
    boolean b = (b2 == 1);
    (*Snake2).updateSnake(x2, y2, b, Snake1);
  } else {
    (*Snake2).updateSnake(0, 0, false, Snake1);
  }
    bool buttonPressed = (digitalRead(BUTTON2INPUT) == LOW);
    (*Snake1).updateSnake(moveBody(LRINPUT), moveBody(UDINPUT), buttonPressed, Snake2);
    Display.display();
    timer = millis();
    Serial.print("PLAYING ");
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

// introduction menue
void introMenu() {
  delay(200);
  cleanOut();
  int sizeSnake = 3;
  while (digitalRead(BUTTON2INPUT) == HIGH) {
    setUpDisplay();
    int change = inputFromWeb();
    delay(30);
    if (change == -100) {
      break;
    } else if (change != 0) {
      sizeSnake += change;
      if (sizeSnake == 0) {
        sizeSnake = 6;
      } else if (sizeSnake == 7) {
        sizeSnake = 1;
      }
    }
    Display.print("size of the snake: ");
    Display.println(sizeSnake);
    Display.display();
    Serial.print("INTRO 1 ");
    Serial.println(sizeSnake);
    delay(30);
  }
  Display.clearDisplay();
  delay(200);
  cleanOut();
  int radius = 2;
  while (digitalRead(BUTTON2INPUT) == HIGH) {
    setUpDisplay();
    int change = inputFromWeb();
    delay(30);
    if (change == -100) {
      break;
    } else if (change != 0) {
      radius += change;
      if (radius == 0) {
        radius = 4;
      } else if (radius == 5) {
        radius = 1;
      }
    }
    Display.print("radius of the snake: ");
    Display.println(radius);
    Display.display();
    Serial.print("INTRO 2 ");
    Serial.println(radius);
    delay(30);
  }
  delay(200);
  cleanOut();
  Snake1 = new SnakePlayer(sizeSnake, VIB, 10000, radius, 1, true, SOUNDPIN, NOISECHANNEL);
  Snake2 = new SnakePlayer(sizeSnake, VIB, 10000, radius, 1, false, SOUNDPIN, NOISECHANNEL);
  int spaceTaken = 2 * sizeSnake * radius;
  initialMode(spaceTaken);
}

// prints out the locations in the ready start menu
// spaceTaken where the sanke snarts
void initialMode(int spaceTaken) {
  cleanOut();
  digitalWrite(REDLIGHT, LOW);
  digitalWrite(GREENLIGHT, LOW);
  for (int i = 3; i > 0; i--) {
    setUpDisplay();
    Display.setCursor(60, 0);
    Display.println(i);
    (*Snake1).setInitial(2 * spaceTaken, 40, 1);
    (*Snake2).setInitial(spaceTaken, 30, -1); 
    Display.display();   
    Serial.print("LOADING ");
    Serial.print(i);
    Serial.print(" ");
    (*Snake1).printLocations();
    Serial.print("| ");
    (*Snake2).printLocations();
    Serial.println();
    delay(1000);
  }
  
}

// changes the options based on joystick
int changeOptions() {
  if (analogRead(UDINPUT) < 1000) {
    return -1;
  } else if (analogRead(UDINPUT) > 2000) {
    return 1;
  } else {
    return 0;
  }
}

// sets up the text options for the display
void setUpDisplay() {
   Display.clearDisplay();
    Display.setTextSize(1);
    Display.setTextColor(WHITE);
    Display.setCursor(0, 0);
}

// takes the input from the web to change the settings
int inputFromWeb() {
  if (Serial.available() > 0) {
      String response = Serial.readStringUntil('\n');
      if (response.equals("done")) {
        return -100;
      } else if (response.equals("1")) {
        return 1;
      } else if (response.equals("-1")) {
        return  -1;
      }
    } else {
    return changeOptions();
  }
}

// the game over screen
void gameOverScreen() {
  delay(300);
  cleanOut();
  int gameStatus = 0;
  (*Snake1).stopNoise();
  (*Snake2).stopNoise();
  if ((*Snake1).gameOver()) {
    digitalWrite(REDLIGHT, HIGH);
    Player2Count++;
    gameStatus = 2;
  } else {
    digitalWrite(GREENLIGHT, HIGH);
    Player1Count++;
    gameStatus = 1;
  }
  int button = 1;
  cleanOut();
  while (digitalRead(BUTTON2INPUT) == HIGH) {
    setUpDisplay();
    if ((*Snake1).gameOver()) {
      Display.println("Player 2 won");
    } else {
      Display.println("Player 1 won");
    } 
    Display.print("Player 1 total ");
    Display.println(Player1Count);
    Display.print("Player 2 total ");
    Display.println(Player2Count);
    int change = inputFromWeb();
    button += change;
    delay(30);
    Display.print("Play again or change the rules? ");
    if (change == -100) {
      break;
    } else if (button == 1 || button == -1) {
      button = 1;
      Display.println("Play again");
    } else {
      button = 0;
      Display.println("New rules");
    }
    Display.display();
    Serial.print("GAMEOVER ");
    Serial.print(gameStatus);
    Serial.print(" ");
    Serial.print(Player1Count);
    Serial.print(" ");
    Serial.print(Player2Count);
    Serial.print(" ");
    Serial.println(button);
    delay(30);
  }
  delay(200);
  if (button < -10) {
    button += 100;
  }
  if ( button == 1) {
    (*Snake1).resetSize();
    (*Snake2).resetSize();
    delay(200);
    cleanOut();
    initialMode((*Snake1).snakeSize() * 2 * (*Snake1).getRadius());
  } else {
    introMenu();
  }
}

// when the menu is paused
void pauseMenu() {
  delay(200);
  cleanOut();
  int unpause = 1;
  while (digitalRead(BUTTON2INPUT) == HIGH) {
    setUpDisplay();
    Display.println("The game is currently paused");
    int change = inputFromWeb();
    delay(30);
    Display.print("Should we reset the game? ");
    unpause += change;
    if (change == -100) {
      break;
    } else if (unpause == 1 || unpause == -1) {
      unpause = 1;
      Display.println("no");
    } else {
      unpause = 0;
      Display.println("yes");
    }
    Display.display();
    Serial.print("PAUSED ");
    Serial.println(unpause);
    delay(100);
  }
  delay(200);
  if (unpause < -10) {
    unpause += 100;
  }
  if (unpause == 0) {
    introMenu();
  }
}

// makes sure no extra noise inputs
void cleanOut() {
  while (Serial.available() > 0) {
    Serial.readStringUntil('\n');
  }
}

// makes sure there is only a single input
void trimUp() {
  while (Serial.available() > 1) {
    Serial.readStringUntil('\n');
  }
}
