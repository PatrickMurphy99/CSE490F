#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Tone32.hpp" // get around for directory problem
// this include comes from the makability lab, thank you again
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
  int vibrationChannel; // will the vibrations will come from
  // updates both the lights for the snake
  // winningLight increases in brightness as time goes by
  // losingLight increases in brightness the more size has decreased
  void updateLights() {
    int winningBrightness = map(millis(), startTime, winTime, 0, 4000);
    ledcWrite(winningLight, winningBrightness);
    int losingBrightness = 4000 - map(snakeLength, 0, startLength, 0, 4000);
    ledcWrite(losingLight, losingBrightness);
  }

  // updates teh x and y locations for each ball
  // each ball is one move behind the one in front (will take place of)
  // if hitts a wall will lose a head and bounce in opposite direction
  void updateBody() {
    int changeInX = 0;
    int changeInY = 0;
    if (analogRead(xPositionFinder) > 2000) {
      changeInX = radius;
    } else if (analogRead(xPositionFinder) < 1200) {
      changeInX = -1 * radius;
    }
    if (analogRead(yPositionFinder) > 2000) {
      changeInY = radius;
    } else  if (analogRead(yPositionFinder) < 1200) {
      changeInY = -1 * radius;
    }
    xLocations[0] += 2 * changeInX;
    yLocations[0] += 2 * changeInY;
    boolean dontUpdate = false;
    if (snakeLength > 1) {
      if (xLocations[0] == xLocations[1] && yLocations[0] == yLocations[1]
      || (changeInX == 0 && changeInY == 0)) {
        xLocations[0] -= 2 * changeInX;
        yLocations[0] -= 2 * changeInY;
        dontUpdate = true;
      }
    }
    if (!dontUpdate) {
      for (int i = snakeLength - 1; i >= 0; i--) {
        xLocations[i + 1] = xLocations[i];
        yLocations[i + 1] = yLocations[i];
      }
    }
    // this long if statement just checks if we hit any of the walls
    // if so size goes to zero and snake player loses
    if (xLocations[0] >= (SCREEN_WIDTH - radius) || xLocations[0] <= radius ||
    yLocations[0] >= (SCREEN_HEIGHT - radius) || yLocations[0] <= radius) {
      snakeLength = 0;
      digitalWrite(vibrationChannel, HIGH);
    }
    // checks if the front node hits any of the intermediate nodes
    // if so gets rid of that node and any below it
    for (int i = 1; i < snakeLength; i++) {
      //if (abs(xLocations[0] - xLocations[i]) <= radius && abs(yLocations[0] - yLocations[i]) <= radius) {
        //snakeLength = i;
        //ledcWrite(vibrationChannel, HIGH);
       // break;
      //}
    }
    // writes to the display the locations of all the nodes
    for (int i = 0; i < snakeLength; i++) {
      Display.drawCircle(xLocations[i], yLocations[i], radius, SSD1306_WHITE); 
    }
  }

  public:

  // Constructor
  SnakePlayer(uint8_t WinningLight, uint8_t LosingLight, int StartLength, int VibrationChannel,
  int XPositionFinder, int YPositionFinder, unsigned long WinTime, int Radius) {
    winningLight = WinningLight;
    losingLight = LosingLight;
    xLocations = new uint16_t[StartLength];
    yLocations = new uint16_t[StartLength];
    startLength = StartLength;
    snakeLength = StartLength;
    radius  = Radius;
    vibrationChannel = VibrationChannel;
    winTime = millis() + WinTime;
    startTime = millis();
    xPositionFinder = XPositionFinder;
    yPositionFinder = YPositionFinder;
  }

  // public update call to adust snake
  // the light and the location
  void updateSnake() {
    digitalWrite(vibrationChannel, LOW);
    updateLights();
    updateBody();
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

  // returns 1 if snakePlayer has won
  // returns -1 if sniperPlayer has won
  // returns 0 if neither player has won yet
  int gameOver() {
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
    Display.display();
  }

  // resets size and time
  void resetSize() {
    snakeLength = startLength;
    winTime = (winTime - startTime) + millis();
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
  int playerTwoButton;

  // updates both the lights for the snake
  // winningLight increases in brightness as snakeLength decreases
  // losingLight increases in brightness as time goes by
  void updateLights(SnakePlayer currentSnake) {
    int winningBrightness = map(currentSnake.snakeSize(), 0, startLength, 0, 4000);
    ledcWrite(winningLight, winningBrightness);
    int losingBrightness = map(millis(), startTime, loseTime, 0, 4000);
    ledcWrite(losingLight, losingBrightness);
  }

  // updates the current position of the gun
  void updatePosition() {
    int gunPositionIndex = map(analogRead(gunChannel), 0, 4096, 0, 2 * gunLength);
    Display.drawLine(xBase, yBase, xHeadLocations[gunPositionIndex], yHeadLocations[gunPositionIndex], SSD1306_WHITE);
  }

  // updates the bullet location if fired, or checks if we fire a bullet
  void updateBullet(SnakePlayer currentSnake) {
    int gunPositionIndex = (map(analogRead(gunChannel), 0, 4096, 0, 2 * gunLength));
    if (shotBullet) {
      bulletXLocation += bulletXVelocity;
      bulletYLocation += bulletYVelocity;
      uint16_t xSpot = (bulletXLocation / gunLength) + xBase;
      uint16_t ySpot = (bulletYLocation / gunLength) + yBase;
      // checks if the bullet is in a viable location
      if (xSpot < 0 || xSpot > SCREEN_WIDTH
      || ySpot < 0 || ySpot > SCREEN_HEIGHT) {
        shotBullet = false; // bullet is off screen, you can shoot now
      } else {
        Display.drawPixel(xSpot, ySpot, SSD1306_WHITE);
      }
    } 
    if (digitalRead(playerTwoButton) == LOW) {
      shotBullet = true;
      bulletXVelocity = (xHeadLocations[gunPositionIndex] - xBase);
      bulletYVelocity = yHeadLocations[gunPositionIndex];
      bulletXLocation = 0;
      bulletYLocation = 0;
      Toner.playNote(NOTE_A, 4); 
    }
  }

  public:

  // Constructor
  SniperPlayer(uint8_t WinningLight, uint8_t LosingLight, int GunLength, int GunChannel,
  unsigned long LoseTime, SnakePlayer snake, int soundPin, uint8_t soundChannel, int PlayerTwoButton)
  : Toner(soundPin, soundChannel) {
    winningLight = WinningLight;
    losingLight = LosingLight;
    gunLength = GunLength;
    gunChannel = GunChannel;
    xBase = SCREEN_WIDTH / 2;
    yBase = 0;
    xHeadLocations = new uint16_t[2 * gunLength + 1];
    yHeadLocations = new uint16_t[2 * gunLength + 1];
    for (uint16_t i = 0; i < gunLength; i++) {
      xHeadLocations[i] = gunLength - i + xBase;
      xHeadLocations[2 * gunLength - i] = xBase - (gunLength - i);
      yHeadLocations[i] = i + yBase;
      yHeadLocations[2 * gunLength - i] = i + yBase;
    }
    bulletXLocation = xBase;
    bulletYLocation = yBase;
    bulletXVelocity = 0;
    bulletYVelocity = 0;
    shotBullet = false;
    startLength = snake.snakeSize();
    startTime = millis();
    loseTime = startTime + LoseTime;
    playerTwoButton = PlayerTwoButton;
  }

  // updates the sniper
  void updateSniper(SnakePlayer currentSnake) {
    Toner.update();
    updateLights(currentSnake);
    updatePosition();
    updateBullet(currentSnake);
  }

  // resets sniper
  void resetSniper(SnakePlayer currentSnake) {
    startLength = currentSnake.snakeSize();
    loseTime = (loseTime - startTime) + millis();
    startTime = millis();
  }

  // gives returns an array of the xLocation, yLocation of the bullet
  uint16_t bulletX() {
   return (bulletXLocation / gunLength) + xBase;
  }

  // y location
  uint16_t bulletY() {
    return (bulletYLocation / gunLength);
  }
    
};


int potentiometerInput;
int vibratingChannel;
uint8_t playerOneRedLight;
uint8_t playerOneGreenLight;
uint8_t playerTwoRedLight;
uint8_t playerTwoGreenLight;
int playerTwoButtonInput;
int playerOneButtonInput;
int joyStickLRInput;
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
  delay(3000);
  Serial.println("test");
  delay(4000);
  if (!Display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Display.clearDisplay();
  
  //potentiometerOutput = 18
   pinMode(18, OUTPUT);
   digitalWrite(18, HIGH);
   
   potentiometerInput = 34;
   pinMode(34, INPUT);
  
  noiseMaker = 10;
  ledcSetup(noiseMaker, 490, 8);
  noisePin = 19; // check if 16 or 19
  pinMode(noisePin, OUTPUT);
  ledcAttachPin(noisePin, noiseMaker); // is this supposed to be 16 or 19
  Tone32 tonez(noisePin, noiseMaker);
  unsigned long t = 100000;
  //tonez.playNote(NOTE_C, 4, t); it works
 
  //vibratingChanel = 17;
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);

  vibratingChannel = 16;
  pinMode(vibratingChannel, OUTPUT);
  //digitalWrite(16, HIGH);
  
  
  playerOneRedLight = 1;
  ledcSetup(playerOneRedLight, 490, 8);
  pinMode(13, OUTPUT);
  ledcAttachPin(13, playerOneRedLight);
  //ledcWrite(1, 4000);
  
  playerOneGreenLight = 2;
  ledcSetup(playerOneGreenLight, 490, 8);
  pinMode(26, OUTPUT);
  ledcAttachPin(26, playerOneGreenLight);
  //ledcWrite(2, 4000);
  
  playerTwoRedLight = 3;
  ledcSetup(playerTwoRedLight, 490, 8);
  pinMode(25, OUTPUT);
  ledcAttachPin(25, playerTwoRedLight);
  //ledcWrite(3, 4000);
  
  playerTwoGreenLight = 4;
  ledcSetup(playerTwoGreenLight, 490, 8);
  pinMode(5, OUTPUT);
  ledcAttachPin(5, playerTwoGreenLight);
  //ledcWrite(4, 4000);
  
  //playerTwoButtonOutput = 5;
  //ledcSetup(playerTwoButtonOutput, 490, 8);
  pinMode(15, OUTPUT);
  //ledcAttachPin(19, playerTwoButtonOutput);
  //ledcWrite(playerTwoButtonOutput, HIGH);
  digitalWrite(15, OUTPUT);
  
  playerTwoButtonInput = 33;
  pinMode(33, INPUT_PULLUP);
  
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);

  playerOneButtonInput = 32;
  pinMode(playerOneButtonInput, INPUT_PULLUP);
  
  //joyStickLROutput = 27;
  pinMode(27, OUTPUT);
  digitalWrite(27, HIGH);
  
  joyStickLRInput = 12;
  pinMode(joyStickLRInput, INPUT);

  //joyStickUDOutput
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  joyStickUDInput = 36;
  pinMode(joyStickUDInput, INPUT);
  
  startMenu();
}

void loop() {
  if (millis() - currentTime >= 80) {
    Display.clearDisplay();
    (*snake).updateSnake();
    (*sniper).updateSniper(*snake);
    uint8_t bulletLX = (*sniper).bulletX();
    uint8_t bulletLY = (*sniper).bulletY();
    (*snake).checksBody(bulletLX, bulletLY);
    if ((*snake).gameOver() != 0) {
      winScreen((*snake).gameOver());
    }
    currentTime = millis();
    Display.display();
  }
}

// start menue for the main game
// asks some basic settings for the game
void startMenu() {
   Display.clearDisplay();
   Display.setTextSize(1);
   Display.setTextColor(WHITE);
   Display.setCursor(0, 30);
   Display.println("length of 3 p1 press");
   Display.println("length of 4 p2 press");
   Display.display();
   int snakeSize = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      snakeSize = 3;
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      snakeSize = 4;
      break;
    }
   }
   delay(1000);
   Display.clearDisplay();
   Display.setTextSize(1);
   Display.setTextColor(WHITE);
   Display.setCursor(0, 30);
   Display.println("30 secs p1 press");
   Display.println("one min p2 press");
   Display.display();
   unsigned long gameTime  = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      gameTime = 30000;
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      gameTime = 60000;
      break;
    }
   }
   delay(1000);
   Display.clearDisplay();
   Display.setTextSize(1);
   Display.setTextColor(WHITE);
   Display.setCursor(0, 30);
   Display.println("radius 3 p1 press");
   Display.println("radius 4 p2 press");
   Display.display();
   int radius = 0;
   while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      radius = 3;
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      radius = 4;
      break;
    }
   }
   delay(1000);
   snake = new SnakePlayer(playerOneGreenLight, playerOneRedLight, snakeSize, vibratingChannel,
   joyStickLRInput, joyStickUDInput, gameTime, radius);
   sniper = new SniperPlayer(playerTwoGreenLight, playerTwoRedLight, 5, potentiometerInput,
   gameTime, *snake, noisePin, noiseMaker, playerTwoButtonInput);
   delayStart();
}

// win screen, displays who won and what is the total score
// asks users if they want to play again or change settings
void winScreen(int game) {
  Display.clearDisplay();
  Display.setTextSize(1);
  Display.setTextColor(WHITE);
  Display.setCursor(0, 0);
  if (game == 1) {
    playerOneTotal++;
    Display.println("p1 won");
    ledcWrite(playerOneGreenLight, 4000);
    ledcWrite(playerOneRedLight, 0);
    ledcWrite(playerTwoGreenLight, 0);
    ledcWrite(playerTwoRedLight, 4000);
  } else {
    playerTwoTotal++;
    Display.println("p2 won");
    ledcWrite(playerOneGreenLight, 0);
    ledcWrite(playerOneRedLight, 4000);
    ledcWrite(playerTwoGreenLight, 4000);
    ledcWrite(playerTwoRedLight, 0);
  }
  Display.println("p1 score:");
  Display.println(playerOneTotal);
  Display.println("p2 score:");
  Display.println(playerTwoTotal);
  Display.println("same mode p1 press");
  Display.println("change mode p2 press");
  Display.display();
  while (true) {
    if (digitalRead(playerOneButtonInput) == LOW) {
      delay(1000);
      (*snake).resetSize();
      delayStart();
      break;
    } else if (digitalRead(playerTwoButtonInput) == LOW) {
      delay(1000);
      startMenu();
      break;
    }
  }
}

// starts game in three seconds
void delayStart() {
  for (int i = 3; i > 0; i--) {
    Display.clearDisplay();
    (*snake).setInitial(30, 30);
    Display.setTextSize(1);
    Display.setTextColor(WHITE);
    Display.setCursor(60, 30);
    Display.println(i);
    Display.display();
    delay(1000);
  }
  currentTime = millis();
}
