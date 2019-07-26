#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int ROTATE_BUTTON_PIN_NUMBER = 10;
const int LEFT_BUTTON_PIN_NUMBER = 12;
const int RIGHT_BUTTON_PIN_NUMBER = 11;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

//a global variable that represents the game board
bool board[16][8];


const int ROWS = 16;
const int COL = 8;

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Board {
  public:
    Board() {
      //sets the game board
      for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COL; c++) {
          board[r][c] = false;
        }
      }
    }
    
  //translates the panel from horizontal to vertical
    void lightUp(int x, int y, Color z) {
      x = x * 2;
      y = y * 2;
      matrix.drawPixel(y, x, z.to_333());
      matrix.drawPixel(y + 1, x, z.to_333());
      matrix.drawPixel(y, x + 1, z.to_333());
      matrix.drawPixel(y + 1, x + 1, z.to_333());
    }

};

Board pixelBoard;

class Shape {
  public:
    int xPoints[4];
    int yPoints[4];

    Color color;
    int orientation;
    unsigned long milliClock;

    void draw_with_rgb() {
      draw(color, true);
    }

    void draw(Color z, bool colored) {
      for (int i = 0; i < 4; i++) {
        board[yPoints[i]][7 - xPoints[i]] = colored;
        pixelBoard.lightUp(xPoints[i], yPoints[i], z);
      }
    }

    void erase() {
      draw(BLACK, false);
    }

    bool testMove() {
      erase();
      for (int i = 0; i < 4; i++) {
        if (yPoints[i] + 1 < 16) {
          if (board[yPoints[i] + 1][7 - xPoints[i]]) {
            return false;
          }
        } else {
          return false;
        }
      }
      return true;
    }

    void move() {
      if (millis() > milliClock + 1000) {
        milliClock = millis();
        for (int i = 0; i < 4; i++) {
          yPoints[i]++;
        }
      }
    }

    virtual void rotate() {
      return;
    }


    bool testSlideRight() {
      erase();
      for (int i = 0; i < 4; i++) {
        if (xPoints[i] != 0) {
          if (board[yPoints[i]][7 - (xPoints[i] - 1)]) {
            return false;
          }
        } else {
          return false;
        }
      }
      return true;


    }

    void slideRight() {

      if (testSlideRight()) {
        for (int i = 0; i < 4; i++) {
          xPoints[i]--;
        }
      }


    }

    bool testSlideLeft() {
      erase();
      for (int i = 0; i < 4; i++) {
        if (xPoints[i] != 7) {
          if (board[yPoints[i]][7 - (xPoints[i] + 1)]) {
            return false;
          }
        } else {
          return false;
        }
      }
      return true;
    }

    void slideLeft() {

      if (testSlideLeft()) {
        for (int i = 0; i < 4; i++) {
          xPoints[i]++;
        }
      }
    }

};

class Shape0 : public Shape {
  public:
    Shape0() {
      xPoints[0] = 5;
      yPoints[0] = 1;

      xPoints[1] = 4;
      yPoints[1] = 1;

      xPoints[2] = 3;
      yPoints[2] = 1;

      xPoints[3] = 2;
      yPoints[3] = 1;

      color = AQUA;
      orientation = 0;
      milliClock = millis();
    }

    virtual void rotate() {
      if (orientation == 0) {
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] + 1;
        xPoints[3] = xPoints[1];
        yPoints[3] = yPoints[1] + 2;
      }

      if (orientation == 1) {
        xPoints[3] = xPoints[2] - 1;
        yPoints[3] = yPoints[2];
        xPoints[1] = xPoints[2] + 1;
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[2] + 2;
        yPoints[0] = yPoints[2];
      }

      if (orientation == 2) {
        xPoints[0] = xPoints[1];
        xPoints[2] = xPoints[1];
        xPoints[3] = xPoints[1];

        yPoints[0] = yPoints[1] - 2;
        yPoints[2] = yPoints[1] - 1;
        yPoints[3] = yPoints[1] + 1;
      }

      if (orientation == 3) {
        xPoints[0] = xPoints[1] - 2;
        xPoints[2] = xPoints[1] - 1;
        xPoints[3] = xPoints[1] + 1;

        yPoints[0] = yPoints[1] - 1;
        yPoints[2] = yPoints[1] - 1;
        yPoints[3] = yPoints[1] - 1;
        yPoints[1] -= 1;
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }
    }
};

class Shape1 : public Shape {
  public:
    Shape1() {
      xPoints[0] = 5;
      yPoints[0] = 0;

      xPoints[1] = 5;
      yPoints[1] = 1;

      xPoints[2] = 4;
      yPoints[2] = 1;

      xPoints[3] = 3;
      yPoints[3] = 1;

      color = BLUE;
      orientation = 0;
      milliClock = millis();
    }

    virtual void rotate() {
      if (orientation == 0) {
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] + 1;
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2] - 1;
        xPoints[0] = xPoints[1] - 1;
        yPoints[0] = yPoints[1];
      }
      if (orientation == 1) {
        xPoints[3] = xPoints[2] + 1;
        yPoints[3] = yPoints[2];
        xPoints[1] = xPoints[2] - 1;
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] + 1;
      }
      if (orientation == 2) {
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] - 1;
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2] + 1;
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
      }
      if (orientation == 3) {
        xPoints[3] = xPoints[2] - 1;
        yPoints[3] = yPoints[2];
        xPoints[1] = xPoints[2] + 1;
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }

    }
};

class Shape2 : public Shape {
  public:
    Shape2() {
      xPoints[0] = 5;
      yPoints[0] = 1;

      xPoints[1] = 4;
      yPoints[1] = 1;

      xPoints[2] = 3;
      yPoints[2] = 1;

      xPoints[3] = 3;
      yPoints[3] = 0;

      color = ORANGE;
      orientation = 0;
      milliClock = millis();
    }

    virtual void rotate() {
      if (orientation == 0) {
        yPoints[3] += 2;
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] + 1;
      }
      if (orientation == 1) {
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1];
        yPoints[3]--;
        xPoints[1]++;
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1]++;
      }
      if (orientation == 2) {
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2] - 1;
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] + 1;
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];

      }
      if (orientation == 3) {
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2];
        yPoints[0]++;
        xPoints[2]--;
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] - 1;
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }
    }

};

class Shape3 : public Shape {
  public:

    Shape3() {
      xPoints[0] = 4;
      yPoints[0] = 1;

      xPoints[1] = 3;
      yPoints[1] = 1;

      xPoints[2] = 4;
      yPoints[2] = 2;

      xPoints[3] = 3;
      yPoints[3] = 2;

      color = YELLOW;
      milliClock = millis();
    }

    virtual void rotate() {

    }
};

class Shape4 : public Shape {
  public:
    Shape4() {
      xPoints[0] = 5;
      yPoints[0] = 2;

      xPoints[1] = 4;
      yPoints[1] = 2;

      xPoints[2] = 4;
      yPoints[2] = 1;

      xPoints[3] = 3;
      yPoints[3] = 1;

      color = GREEN;
      orientation = 0;
      milliClock = millis();
    }

    virtual void rotate() {
      if (orientation == 0) {
        xPoints[0] = xPoints[2];
        yPoints[0] = yPoints[2];
        xPoints[2] = xPoints[1] - 1;
        yPoints[2] = yPoints[1];
        yPoints[3] += 2;
      }

      if (orientation == 1) {
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2];
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1];
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2] + 1;
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
      }

      if (orientation == 2) {
        xPoints[3] = xPoints[1];
        yPoints[3] = yPoints[1];
        xPoints[1] = xPoints[2] + 1;
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
      }

      if (orientation == 3) {
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1];
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2];
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] - 1;
        xPoints[3] = xPoints[2] - 1;
        yPoints[3] = yPoints[2];
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }
    }
};

class Shape5 : public Shape {
  public:
    Shape5() {
      xPoints[0] = 5;
      yPoints[0] = 1;

      xPoints[1] = 4;
      yPoints[1] = 1;

      xPoints[2] = 3;
      yPoints[2] = 1;

      xPoints[3] = 4;
      yPoints[3] = 0;

      color = PURPLE;
      milliClock = millis();
      orientation = 0;
    }

    virtual void rotate() {
      if (orientation == 0) {
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] + 1;
        xPoints[3] = xPoints[1] - 1;
        yPoints[3] = yPoints[1];
      }
      if (orientation == 1) {
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
        xPoints[2] = xPoints[1] - 1;
        yPoints[2] = yPoints[1];
        xPoints[3] = xPoints[1];
        yPoints[3] = yPoints[1] + 1;
      }
      if (orientation == 2) {
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] + 1;
        xPoints[3] = xPoints[1] + 1;
        yPoints[3] = yPoints[1];

      }
      if (orientation == 3) {
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
        xPoints[2] = xPoints[1] - 1;
        yPoints[2] = yPoints[1];
        xPoints[3] = xPoints[1];
        yPoints[3] = yPoints[1] - 1;
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }
    }
};


class Shape6 : public Shape {
  public:
    Shape6() {
      xPoints[0] = 4;
      yPoints[0] = 0;

      xPoints[1] = 3;
      yPoints[1] = 0;

      xPoints[2] = 3;
      yPoints[2] = 1;

      xPoints[3] = 2;
      yPoints[3] = 1;

      color = RED;
      milliClock = millis();
      orientation = 0;
    }

    virtual void rotate() {
      if (orientation == 0) {
        xPoints[1] = xPoints[3];
        yPoints[1] = yPoints[3];
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] + 1;
      }
      if (orientation == 1) {
        xPoints[1] = xPoints[2];
        yPoints[1] = yPoints[2];
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1] + 1;
        xPoints[3] = xPoints[2] - 1;
        yPoints[3] = yPoints[2];
      }
      if (orientation == 2) {
        xPoints[2] = xPoints[0];
        yPoints[2] = yPoints[0];
        xPoints[3] = xPoints[2];
        yPoints[3] = yPoints[2] + 1;
        xPoints[0] = xPoints[1];
        yPoints[0] = yPoints[1] - 1;

      }
      if (orientation == 3) {
        xPoints[2] = xPoints[1];
        yPoints[2] = yPoints[1];
        xPoints[1] = xPoints[0];
        yPoints[1] = yPoints[0];
        xPoints[0] = xPoints[1] + 1;
        yPoints[0] = yPoints[1];
        xPoints[3] = xPoints[2] - 1;
        yPoints[3] = yPoints[2];
      }

      orientation++;
      if (orientation > 3) {
        orientation = 0;
      }
    }
};

Shape* tile;

class Game {
  public:
    int score;
    bool shapeFalling;

    Game() {
      score = 0;
    }

    void setUp() {
      shapeFalling = false;

    }

    Shape* rndShape() {
      int type;
      randomSeed(analogRead(4));
      type = (int)random(0, 6);
      if (type == 0) {
        return new Shape0();
      }
      if (type == 1) {
        return new Shape1();
      }
      if (type == 2) {
        return new Shape2();
      }
      if (type == 3) {
        return new Shape3();
      }
      if (type == 4) {
        return new Shape4();
      }
      if (type == 5) {
        return new Shape5();
      }
      if (type == 6) {
        return new Shape6();
      }

      return new Shape6();
    }


    void gameUpdate(int potentiometer_value, bool rotate_button_pressed, bool left_button_pressed, bool right_button_pressed) {
      if (!shapeFalling) {
        tile = rndShape();
        tile->draw_with_rgb();
      }
      shapeFalling = true;

      if (tile->testMove()) {
        tile->move();
      } else {
        shapeFalling = false;
      }

      if (left_button_pressed) {
        delay(100);
        tile->slideLeft();
      }
      tile->draw_with_rgb();
      tile->erase();

      if (right_button_pressed) {
        delay(100);
        tile->slideRight();
      }
      tile->draw_with_rgb();
      tile->erase();

      if (rotate_button_pressed) {
        //if(tile->canRotate()) {
        tile->rotate();
        delay(100);
        //}
      }
      tile->draw_with_rgb();

    }
};

Game game;


void setup() {
  Serial.begin(9600);
  pinMode(ROTATE_BUTTON_PIN_NUMBER, INPUT);
  pinMode(LEFT_BUTTON_PIN_NUMBER, INPUT);
  pinMode(RIGHT_BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
}

void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool rotate_button_pressed = (digitalRead(ROTATE_BUTTON_PIN_NUMBER) == HIGH);
  bool left_button_pressed = (digitalRead(LEFT_BUTTON_PIN_NUMBER) == HIGH);
  bool right_button_pressed = (digitalRead(RIGHT_BUTTON_PIN_NUMBER) == HIGH);
  game.gameUpdate(potentiometer_value, rotate_button_pressed, left_button_pressed, right_button_pressed);
}
