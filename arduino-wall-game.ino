#define matrix_size 8
#define num_of_directions 2
typedef byte dot_matrix[matrix_size];
dot_matrix matrix = {B00000000};

const byte numbers[10][matrix_size] = {
{
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000
},{
  B00100000,
  B01100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B01110000
},{
  B01110000,
  B10001000,
  B00001000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B11111000
},{
  B01110000,
  B10001000,
  B00001000,
  B00110000,
  B00001000,
  B00001000,
  B10001000,
  B01110000
},{
  B00001000,
  B00011000,
  B00101000,
  B01001000,
  B10001000,
  B11111000,
  B00001000,
  B00001000
},{
  B11111000,
  B10000000,
  B10000000,
  B11110000,
  B00001000,
  B00001000,
  B10001000,
  B01110000
},{
  B01110000,
  B10001000,
  B10000000,
  B11110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000
},{
  B11111000,
  B00001000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B01000000,
  B01000000
},{
  B01110000,
  B10001000,
  B10001000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000
},{
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B10001000,
  B01110000
}};

const byte refreshSpeed = 16; // 62.5 fps
const byte row[8] = {
  2, 3, 4, 5, 6, 7, 8, 9
};
const byte col[8] = {
  A0, A1, A2, A3, A4, A5, 11, 12
};
const byte dirPin[num_of_directions] = {10, 13};
byte buttonState[num_of_directions] = {0};
byte lastButtonState[num_of_directions] = {0};

unsigned int gameOverCounter;
byte gameOver;
unsigned int gameOverDuration;
byte playerLocation;
unsigned int playerCounter;
unsigned int playerStepDuration;
unsigned short int score;
short int scoreTextOffset;
const unsigned short int fullSpeedScore = 200; // must be larger than wallStartStepDuration-1


unsigned int wallSpeedDuration;
unsigned int wallSpeedCounter;
unsigned int wallCounter;
const byte wallStartStepDuration = 20;
byte wallStepDuration;
byte wallLocation;
byte wallHole;

void setup() {
  initPins();
  resetPins();
  gameSetup();
}

void gameSetup() {
  randomSeed(score);
  score = 0;
  playerLocation = random(8);
  playerCounter = 0;
  playerStepDuration = 5;
  gameOverCounter = 0;
  gameOver = 0;
  resetWall();
}

void resetWall() {
  wallHole = random(8);
  wallLocation = 0;
  wallStepDuration = wallStartStepDuration;
  wallCounter = 0;
  wallSpeedCounter = 0;
  wallSpeedDuration = 8; // seconds interval for wall speed update
}

void loop() {
  updateGame();
  drawMatrix(); // refreshSpeed * 8 ms delay = 16 ms = 62.5 fps
}

void updateGame() {
  matrixClear();
  if (!gameOver) {

    checkButton();
    if (wallLocation == matrix_size-1 && wallHole != playerLocation) {
      byte scoreLength = lengthDecimal(score);
      gameOver = 1;
      scoreTextOffset = -scoreLength*6-8;
      gameOverDuration = scoreLength+1; // seconds
    } else {
      updateWall();
      matrixWall();
      matrixPlayer();
    }
  } else {  
    if (gameOverCounter >= (gameOverDuration*1000)/refreshSpeed) {
      gameSetup();
    } else {
      gameOverCounter++;
      if (gameOverCounter%((16*5)/refreshSpeed)==0) scoreTextOffset++;
      matrixScore();
    }
  }
}

void updateWall() {
  if (wallCounter >= (16*wallStepDuration)/refreshSpeed) {
    wallCounter = 0;
    if (wallLocation == matrix_size-1) {
      wallHole = random(8);
      wallLocation = 0;
      score++;
    } else {
      wallLocation++;
    }
  } else {
    wallCounter++;
  }

  if (score/(fullSpeedScore/(wallStartStepDuration-1)) < wallStartStepDuration) {
    wallStepDuration = wallStartStepDuration-score/(fullSpeedScore/(wallStartStepDuration-1));
  } else {
    wallStepDuration == 1;
  }
}

void drawMatrix() {
  byte rowIsActive;
  for (byte y = 0; y<matrix_size; y++) {
    for (byte i = 0; i<matrix_size; i++) {
      digitalWrite(row[i], LOW);
    }
    rowIsActive = 0;
    for (byte x = 0; x<matrix_size; x++) {
      if (!rowIsActive) {
        digitalWrite(row[y], readMatrix(y,x));
        rowIsActive = readMatrix(y,x);
      }
      digitalWrite(col[x], 1-readMatrix(y,x));
    }
    delayMicroseconds((refreshSpeed*1000)/matrix_size);
  }
}

void checkButton() {
  for (byte i=0; i<num_of_directions; i++) {
    buttonState[i] = digitalRead(dirPin[i]);
    if (buttonState[i]) {
      if (playerCounter >= playerStepDuration) {
        playerCounter = 0;
      } else if (playerCounter == 0) {
        playerLocation = mod(playerLocation+(2*i-1), 8);
        playerCounter++;
      } else {
        playerCounter++;
      }
    }
  }
}

void matrixClear() {
  for (byte y = 0;y<matrix_size; y++) {
    for (byte x = 0; x<matrix_size; x++) {
      matrix[y] &= ~(1UL << matrix_size-1-x);
    }
  }
}

void matrixPlayer() {
  matrix[matrix_size-1] |= 1UL << matrix_size-1-playerLocation;
}

void matrixScore() {
  unsigned long scoreBinaryRow;
  for (byte y=0; y<matrix_size;y++) {
    scoreBinaryRow = scoreToBinary(score, y);
    for (byte x=0; x<matrix_size;x++) {
      bitWrite(matrix[y], matrix_size-1-x, bitRead(scoreBinaryRow, matrix_size-1-(x+scoreTextOffset)));
    }
  }
}

unsigned long scoreToBinary(unsigned short int scoreLoc, byte row){
  unsigned long scoreText = 0UL;
  byte numberPosition = 0;
  if (scoreLoc == 0) {
    scoreText += (unsigned long)numbers[0][row] >> 2;
  }
  while (scoreLoc > 0) {
    if (numberPosition == 0) {
      scoreText += (unsigned long)numbers[scoreLoc%10][row] >> 2;
    } else {
      scoreText += (unsigned long)numbers[scoreLoc%10][row] << (6*numberPosition-2);
    }
    scoreLoc /= 10;
    numberPosition++;
  }
  return scoreText;
}

void matrixWall() {
  for (byte x=0; x<matrix_size;x++) {
    if (x!=wallHole) {
      matrix[wallLocation] |= 1UL << matrix_size-1-x;
    }
  }
}

byte readMatrix(byte y, byte x) {
  return (matrix[y] >> (matrix_size-1-x)) & 1;
}

void initPins() {
  for (byte i = 0; i<matrix_size; i++) {
    pinMode(row[i], OUTPUT);
    pinMode(col[i], OUTPUT);
  }
  for (byte i = 0; i<num_of_directions; i++) {
    pinMode(dirPin[i], INPUT);
  }
}
void resetPins() {
  for (byte i = 0; i<matrix_size; i++) {
    digitalWrite(row[i], LOW);
    digitalWrite(col[i], HIGH);
  }
}

byte lengthDecimal(unsigned short int i) {
  if(i > 9999)
    return 5;
  else if(i > 999)
    return 4;
  else if(i > 99)
    return 3;
  else if(i > 9)
    return 2;
  else
    return 1;
}

int mod(int k, int n) {
    return ((k %= n) < 0) ? k+n : k;
}