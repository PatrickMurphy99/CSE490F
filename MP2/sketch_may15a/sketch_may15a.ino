#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Tone32.hpp" // get around for directory problem
uint16_t SCREEN_WIDTH = 128; // OLED display width, in pixels
uint16_t SCREEN_HEIGHT = 64; // OLED display height, in pixels
int TIME_DELAY = 20;
Adafruit_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// represents the first player who has the 5 circle snake object
// uses the controler with a joystick, and the vibro motor
class SnakePlayer {

  private: 
  uint8_t winningLight; // slowly turns brighter as time goes out
  uint8_t losingLight; // gets brigher everytime you lose a component
  int startLength; // inital length of the snake
  int snakeLength; // current length of the snake, loses when it goes to zero
  uint16_t* xLocations; // locations of every x component of the snake
  uint16_t* yLocations; // locations of every y component of the snake
  int xPositionFinder; // input for x position
  int yPositionFinder; // input for y position
  unsigned long startTime; // when the game started
  unsigned long winTime; // when the snake player wins the game if he survives too
  int radius; // radius of each node in the snake
  uint8_t vibrationChannel; // will the vibrations will come from

  // updates both the lights for the snake
  // winningLight increases in brightness as time goes by
  // losingLight increases in brightness the more size has decreased
  void updateLights() {
    int winningBrightness = map(millis(), startTime, winTime, 0, HIGH);
    ledcWrite(winningLight, winningBrightness);
    int losingBrightness = HIGH - map(snakeLength, 0, startLength, 0, HIGH);
    ledcWrite(losingLight, losingBrightness);
  }

  // updates teh x and y locations for each ball
  // each ball is one move behind the one in front (will take place of)
  // if hitts a wall will lose a head and bounce in opposite direction
  void updateBody() {
    for (int i = snakeLength - 1; i > 0; i--) {
      xLocations[i + 1] = xLocations[i];
      yLocations[i + 1] = yLocations[i];
    }
    int changeInX = map(analogRead(xPositionFinder), 0, 4096, -1 * radius,radius);
    int changeInY = map(analogRead(yPositionFinder), 0, 4096,-1 * radius, radius);
    xLocations[0] += changeInX;
    yLocations[0] += changeInY;
    // trying to fully backtrack
    if (snakeLength > 1 && xLocations[0] == xLocations[0] && yLocations[0] == yLocations[0]) {
      xLocations[0] -= (2 * changeInX); // inertia for x
      yLocations[0] -= (2 * changeInY); // inertia for y
      // if snake is going in the opposite direction, going to switch tail to back of snake 
      for (int i = 0; i < (snakeLength / 2); i++) {
        int tempX = xLocations[i];
        int tempY = yLocations[i];
        xLocations[i] = xLocations[snakeLength - i];
        yLocations[i] = yLocations[snakeLength - i]; 
      }
    }
    // this long if statement just checks if we hit any of the walls
    // if so, just makes the back the front again, but removes a node (ie gits rid of front)
    if (xLocations[0] >= SCREEN_WIDTH || xLocations[0] <= 0 ||
    yLocations[0] >= SCREEN_HEIGHT || yLocations[0] <= 0) {
      for (int i = 0; i < (snakeLength / 2); i++) {
        int tempX = xLocations[i];
        int tempY = yLocations[i];
        xLocations[i] = xLocations[snakeLength - i];
        yLocations[i] = yLocations[snakeLength - i]; 
      }
      snakeLength--;
      ledcWrite(vibrationChannel, HIGH);
    }
    // checks if the front node hits any of the intermediate nodes
    // if so gets rid of that node and any below it
    for (int i = 1; i < snakeLength; i++) {
      if (abs(xLocations[0] - xLocations[i]) <= radius && abs(yLocations[0] - yLocations[i]) <= radius) {
        snakeLength = i;
        ledcWrite(vibrationChannel, HIGH);
        break;
      }
    }
    // writes to the display the locations of all the nodes
    for (int i = 0; i < snakeLength; i++) {
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE); 
    }
  }

  public:

  // Constructor
  SnakePlayer(uint8_t WinningLight, uint8_t LosingLight, int StartLength, uint8_t VibrationChannel,
  int XPositionFinder, int YPositionFinder, unsigned long WinTime, int Radius) {
    winningLight = WinningLight;
    losingLight = 0; // figure out the light stuff
    xLocations = new uint16_t[StartLength];
    yLocations = new uint16_t[StartLength];
    startLength = StartLength;
    snakeLength = StartLength;
    radius  = Radius;
    vibrationChannel = VibrationChannel;
    winTime = millis() + WinTime;
    startTime = millis();
  }

  // public update call to adust snake
  // the light and the location
  void updateSnake() {
    ledcWrite(vibrationChannel, LOW);
    updateLights();
    updateBody();
  }

  // returns the length of the snake
  int snakeSize() {
    return snakeLength;
  }

  // checks if given points hits the snake, and if so removes node
  void checksBody(int xSpot, int ySpot) {
    for (int i = 0; i < snakeLength; i++) {
      if (abs(xLocations[i] - xSpot) <= radius && abs(yLocations[i] - ySpot) <= radius) {
        snakeLength--;
        ledcWrite(vibrationChannel, HIGH);
      }
    }
  }

  // returns 1 if snakePlayer has won
  // returns -1 if sniperPlayer has won
  // returns 0 if neither player has won yet
  bool gameOver() {
    if (snakeLength <= 0) {
      return -1;
    } else if (millis() >= winTime) {
      return 1;
    } else {
      return 0;
    }
  }

  // sets initial position for the snake player at given x and y
  // each component is two radius away from the next in terms of x
  void setInitial(uint16_t initialX, uint16_t initialY) {
    for (int i = 0; i < snakeLength; i++) {
      xLocations[i] = initialX - (2 * radius * i);
      yLocations[i] = initialY;
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE);
    } 
  }
};

class SniperPlayer {

  private:
  
  uint8_t winningLight; // grows brighter after each successful shot
  uint8_t losingLight; // sloly gros brigher as time goes by
  uint16_t xBase; // where the origin x part of the sniper
  uint16_t yBase; // where the origin y part of the sniper
  uint16_t* xHeadLocations; // where the end of the x part of the sniper is
  uint16_t* yHeadLocations; // where the end of the yp part of the sniper is
  uint16_t bulletXLocation; // x location of the bullet
  uint16_t bulletYLocation; // y location of the bullet
  uint16_t bulletXVelocity; // x change in location of bullet
  uint16_t bulletYVelocity; // y change in location of bullet
  bool shotBullet; // determines if the bullet has been shot
  int gunLength; // length of gun
  // button stuff as well
  int gunChannel; // reads the channel
  int startLength; // initial snake size
  unsigned long startTime; // when the game started
  unsigned long loseTime; // when the snake player loses the game if he survives too
  Tone32 Toner; // where the sound source will come from
  SnakePlayer currentSnake; // current snake in game

  // updates both the lights for the snake
  // winningLight increases in brightness as snakeLength decreases
  // losingLight increases in brightness as time goes by
  void updateLights() {
    int winningBrightness = map(currentSnake.snakeSize(), 0, startLength, 0, HIGH);
    ledcWrite(winningLight, winningBrightness);
    int losingBrightness = map(millis(), startTime, loseTime, 0, HIGH);
    ledcWrite(losingLight, losingBrightness);
  }

  // updates the current position of the gun
  void updatePostion() {
    int gunPositionIndex = map(analogRead(gunChannel), 0, 4096, 0, 2 * gunLength);
    Display.drawLine(xBase, yBase, xHeadLocations[gunPositionIndex], yHeadLocations[gunPositionIndex], SSD1306_WHITE);
  }

  // updates the bullet location if fired, or checks if we fire a bullet
  void updateBullet() {
    int gunPositionIndex = map(analogRead(gunChannel), 0, 4096, 0, 2 * gunLength);
    currentSnake.checksBody(xBase, yBase);
    currentSnake.checksBody(xHeadLocations[gunPositionIndex], yHeadLocations[gunPositionIndex]);
    int xInBetween = (xHeadLocations[gunPositionIndex] - xBase) + xBase;
    int yInBetween = (yHeadLocations[gunPositionIndex] - yBase) + yBase;
    currentSnake.checksBody(xInBetween, yInBetween);
    if (shotBullet) {
      bulletXLocation += bulletXVelocity;
      bulletYLocation += bulletYVelocity;
      uint16_t xSpot = (bulletXLocation / gunLength) + xBase;
      uint16_t ySpot = (bulletYLocation / gunLength) + yBase;
      // checks if the bullet is in a viable location
      if (bulletXLocation <= 0 || bulletXLocation >= SCREEN_WIDTH
      || bulletYLocation <= 0 || bulletYLocation >= SCREEN_HEIGHT) {
        shotBullet = false; // bullet is off screen, you can shoot now
      } else {
        Display.drawLine(xSpot, ySpot, xSpot, ySpot, SSD1306_WHITE);
        currentSnake.checksBody(xSpot, ySpot);
      }
    } else {
      if (digitalRead(gunChannel) == LOW) {
        shotBullet = true;
        bulletXVelocity = xHeadLocations[gunPositionIndex];
        bulletYVelocity = yHeadLocations[gunPositionIndex];
        bulletXLocation = 0;
        bulletYLocation = 0;
        Toner.playNote(NOTE_A, 4); 
      }
    }
  }

  public:

  // Constructor
  SniperPlayer(uint8_t WinningLight, uint8_t LosingLight, int GunLength, int GunChannel,
  unsigned long LoseTime, SnakePlayer snake, int soundPin, uint8_t soundChannel)
  : currentSnake(snake), Toner(soundPin, soundChannel) {
    winningLight = WinningLight;
    losingLight = LosingLight;
    gunLength = GunLength;
    gunChannel = GunChannel;
    xBase = SCREEN_WIDTH / 2;
    yBase = 0;
    xHeadLocations = new uint16_t[2 * gunLength + 1];
    yHeadLocations = new uint16_t[2 * gunLength + 1];
    bulletXLocation = xBase;
    bulletYLocation = yBase;
    bulletXVelocity = 0;
    bulletYVelocity = 0;
    shotBullet = false;
    startLength = snake.snakeSize();
    startTime = millis();
    loseTime = startTime + LoseTime;
  }

  // updates the sniper
  void updateSniper() {
    Toner.update();
    int oldSize = currentSnake.snakeSize();
    updateLights();
    updateBullet();
    if (currentSnake.snakeSize() < oldSize) {
      Toner.playNote(NOTE_C, 4);
    }
  }
};

uint8_t potentiometerOutput;
int potentiometerInput;
uint8_t vibratingChannel;
uint8_t playerOneRedLight;
uint8_t playerOneGreenLight;
uint8_t playerTwoRedLight;
uint8_t playerTwoGreenLight;
uint8_t playerTwoButtonOutput;
int playerTwoButtonInput;
uint8_t playerOneButtonOutput;
int playerOneButtonInput;
uint8_t joyStickLROutput;
int joyStickLRInput;
uint8_t joyStickUDOutput;
int joyStickUDInput;

int noisePin;
uint8_t noiseMaker;

unsigned long currentTime;
SnakePlayer* snake;
SniperPlayer* sniper;

int playerOneTotal;

int playerTwoTotal;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Display.clearDisplay();
  potentiometerOutput = 9;
  ledcSetup(potentiometerOutput, 490, 8);
  pinMode(14, OUTPUT);
  ledcAttachPin(14, potentiometerOutput);
  potentiometerInput = 32;
  pinMode(32, INPUT);
  noiseMaker = 10;
  ledcSetup(noiseMaker, 490, 8);
  noisePin = 16; // check if 16 or 19
  pinMode(noisePin, OUTPUT);
  ledcAttachPin(noisePin, noiseMaker); // is this supposed to be 16 or 19
  vibratingChannel = 0;
  ledcSetup(vibratingChannel, 490, 8);
  pinMode(27, OUTPUT);
  ledcAttachPin(27, vibratingChannel);
  playerOneRedLight = 1;
  ledcSetup(playerOneRedLight, 490, 8);
  pinMode(12, OUTPUT);
  ledcAttachPin(12, playerOneRedLight);
  playerOneGreenLight = 2;
  ledcSetup(playerOneGreenLight, 490, 8);
  pinMode(13, OUTPUT);
  ledcAttachPin(13, playerOneGreenLight);
  playerTwoRedLight = 3;
  ledcSetup(playerTwoRedLight, 490, 8);
  pinMode(21, OUTPUT);
  ledcAttachPin(21, playerTwoRedLight);
  playerTwoGreenLight = 4;
  ledcSetup(playerTwoGreenLight, 490, 8);
  pinMode(17, OUTPUT);
  ledcAttachPin(17, playerTwoGreenLight);
  playerTwoButtonOutput = 5;
  ledcSetup(playerTwoButtonOutput, 490, 8);
  pinMode(19, OUTPUT);
  ledcAttachPin(19, playerTwoButtonOutput);
  playerTwoButtonInput = 18;
  pinMode(playerTwoButtonInput, INPUT_PULLUP);
  playerOneButtonOutput = 6;
  ledcSetup(playerOneButtonOutput, 490, 8);
  pinMode(4, OUTPUT);
  ledcAttachPin(4, playerOneButtonOutput);
  playerOneButtonInput = 36;
  pinMode(playerOneButtonInput, INPUT_PULLUP);
  joyStickLROutput = 7;
  ledcSetup(joyStickLROutput, 490, 8);
  pinMode(39, OUTPUT);
  ledcAttachPin(39, joyStickLROutput);
  joyStickLRInput = 34;
  pinMode(joyStickLRInput, INPUT);
  joyStickUDOutput = 8;
  ledcSetup(joyStickUDOutput, 490, 8);
  pinMode(25, OUTPUT);
  ledcAttachPin(25, joyStickUDOutput);
  joyStickUDInput = 26;
  pinMode(joyStickUDInput, INPUT);
  startMenu();
}

void loop() {
  Display.clearDisplay();
  if (millis() - currentTime >= 20) {
    (*snake).updateSnake();
    (*sniper).updateSniper();
    if ((*snake).gameOver() == 1) {
      winScreen(1);
    } else if ((*snake).gameOver() == -1) {
      winScreen(-1);
    }
    currentTime = millis();
  }
  Display.display();
}

// start menue for the main game
// asks some basic settings for the game
void startMenu() {
   Display.clearDisplay();
   Display.setCursor(30, 30);
   Display.println("if you want snake length of 3 player 1 press button");
   Display.println("if you want the length to be 5 player 2 press button");
   int snakeSize = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      snakeSize = 3;
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      snakeSize = 5;
      break;
    }
   }
   Display.clearDisplay();
   Display.println("if you want the game to be a minute long have player one press");
   Display.println("if you want the game to be two minutes, have player two press");
   unsigned long gameTime  = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      gameTime = 60000;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      gameTime = 120000;
    }
   }
   Display.clearDisplay();
   Display.println("if you want the radius to be 1 unit player one press");
   Display.println("if you want the radius to be 2 units player two press");
   int radius = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      radius = 1;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      radius = 2;
    }
   }
   snake = new SnakePlayer(playerOneGreenLight, playerOneRedLight, snakeSize, vibratingChannel,
   joyStickLRInput, joyStickUDInput, gameTime, radius);
   sniper = new SniperPlayer(playerTwoGreenLight, playerTwoRedLight, snakeSize, playerTwoButtonInput,
   gameTime, *snake, noisePin, noiseMaker);
   delayStart();
}

// win screen, displays who won and what is the total score
// asks users if they want to play again or change settings
void winScreen(int game) {
  Display.clearDisplay();
  Display.setCursor(30, 30);
  if (game == 1) {
    playerOneTotal++;
    Display.println("Player one has won the game");
  } else {
    playerTwoTotal++;
    Display.println("Player two has won the game");
  }
  Display.println("player one current score " + playerOneTotal);
  Display.println("player two current score " + playerTwoTotal);
  Display.println("Play Again, player one press");
  Display.println("change settings, player two press");
  while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      delayStart();
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      startMenu();
      break;
    }
  }
}

// starts game in three seconds
void delayStart() {
  Display.clearDisplay();
  Display.setCursor(30, 30);
  Display.println(3);
  delay(1000);
  Display.clearDisplay();
  Display.println(2);
  delay(1000);
  Display.print(1);
  delay(1000);
  Display.clearDisplay();
  (*snake).setInitial(30, 30);
}
