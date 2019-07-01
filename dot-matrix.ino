#define matrix_size 8
#define num_of_directions 2
typedef byte dot_matrix[matrix_size][matrix_size];
dot_matrix matrix = {
  {0, 0, 1, 1, 1, 1, 0, 0},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {1, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 1, 0, 0, 0, 0, 1, 1},
  {1, 0, 1, 1, 1, 1, 0, 1},
  {0, 1, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 1, 1, 1, 0, 0}
};

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
byte offset = 0;

void setup() {
  initPins();
  resetPins();
}

void loop() {
  checkButton();
  drawMatrix(); // refreshSpeed * 8 ms delay = 16 ms = 62.5 fps
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
        digitalWrite(row[y], matrix[y][x]);
        rowIsActive = matrix[y][x];
      }
      digitalWrite(col[mod(x+offset,8)], 1-matrix[y][x]);
    }
    delayMicroseconds((refreshSpeed*1000)/matrix_size);
  }
}

void checkButton() {
  for (byte i=0; i<num_of_directions; i++) {
    buttonState[i] = digitalRead(dirPin[i]);
    if (buttonState[i] != lastButtonState[i]) {
      if (buttonState[i]) {
        buttonPress(i);
      }
    }
    lastButtonState[i] = buttonState[i];
  }
}

void buttonPress(byte i) {
  offset += mod(2*i-1, 8);
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

int mod(int k, int n) {
    return ((k %= n) < 0) ? k+n : k;
}