#ifndef display_h
#define display_h

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <limits.h>

#define sclk 13
#define mosi 11
#define cs   4
#define rst  3
#define dc   2

const int TOP_LEFT_CORNER[] = {10, 8};
const int boardWidth = 49;
const int boardHeight = 13;
const int BOTTOM_RIGHT_CORNER[] = {TOP_LEFT_CORNER[0] + boardWidth, TOP_LEFT_CORNER[1] + boardHeight};
char image[boardWidth][boardHeight]; // [x][y]: 0 = black, 1 = amber, 2 = change to black, 3 = change to amber (using char since it's shorter than an int)
const int BLACK = 0xFFFF;
const int AMBER = 0xF800;

class Display {
  public:
    begin();
    drawPixel(int x, int y, int color);
    shiftImage(int x);
    drawLine(int startX, int startY, int endX, int endY, int color);
    setCursor(int x, int y);
    setTextColor(int color);
    setTextSize(int size);
    printText(char* str);
    fillRect(int startX, int startY, int endX, int endY, int color);
    drawRect(int startX, int startY, int endX, int endY, int color);
    clearDestinationBoard();
    drawImage();
  private:
    static Adafruit_ST7735 display;
    static bool running;
};

Adafruit_ST7735 Display::display = Adafruit_ST7735(&SPI, cs, dc, rst);
bool Display::running = false;

Display::begin() {
  if (!running) {
    for (int x = 0; x < boardWidth; x++) {
      for (int y = 0; y < boardHeight; y++) {
        if (image[x][y] == 1) {
          image[x][y] = 0;
        }
      }
    }
    display.initR(INITR_MINI160x80);
    display.setRotation(3);
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    display.fillScreen(BLACK);
    drawRect(TOP_LEFT_CORNER[0] - 1, TOP_LEFT_CORNER[1] - 1, BOTTOM_RIGHT_CORNER[0] + 1, BOTTOM_RIGHT_CORNER[1] + 1, 0xF00F);
    running = true;
  }
};

bool withinBounds(int x, int y) {
  return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight;
}

Display::drawPixel(int x, int y, int color) {
  if (withinBounds(x, y)) {
    if (color == BLACK && image[x][y] != 0 && image[x][y] != 2) {
      image[x][y] = 2;
    } else if (color == AMBER) {
      image[x][y] = 3;
    }
  }
}

Display::shiftImage(int x) {
  for (int xx = boardWidth - 1 - x; xx >= 1; xx--) {
    for (int yy = 1; yy < boardHeight; yy++) {
      drawPixel(xx + x, yy, image[xx][yy] == 3 ? AMBER : BLACK);
      drawPixel(xx, yy, BLACK);
    }
  }
}

Display::drawLine(int startX, int startY, int endX, int endY, int color) {
  if (startX > endX) {
    int temp = startX;
    startX = endX;
    endX = temp;
  }
  if (startY > endY) {
    int temp = startY;
    startY = endY;
    endY = temp;
  }
  bool isYEqual = startY == endY;
  bool isXEqual = startX == endX;
  if (!isXEqual && !isYEqual) {
    setCursor(0, 16);
    setTextColor(0xFF000);
    setTextSize(1);
    printText("ERROR: UNEXPECTED DRAW");
  } else {
    int *ptr = isYEqual ? &startX : &startY;  // stores a pointer to the variable whether it needs to draw vertically (chooses x) or horizontally (chooses y)
    int end = isYEqual ? endX : endY;
    for (; *ptr <= end; (*ptr)++) {
      drawPixel(startX, startY, color);
    }
  }
};

Display::setCursor(int x, int y) {
  display.setCursor(x, y);
};

Display::setTextColor(int color) {
  display.setTextColor(color);
};

Display::setTextSize(int size) {
  display.setTextSize(size);
};

Display::printText(char* str) {
  display.print(str);
};

Display::drawRect(int x1, int y1, int x2, int y2, int color) {
  display.drawLine(x1, y1, x2, y1, color);
  display.drawLine(x2, y1, x2, y2, color);
  display.drawLine(x2, y2, x1, y2, color);
  display.drawLine(x1, y2, x1, y1, color);
}

Display::drawImage() {
  for (int x = 0; x < boardWidth; x++) {
    for (int y = 0; y < boardHeight; y++) {
      if (image[x][y] == 2 || image[x][y] == 1) {
        display.drawPixel(x + TOP_LEFT_CORNER[0], y + TOP_LEFT_CORNER[1] - 1, BLACK);
        image[x][y] = 0;
      } else if (image[x][y] == 3) {
        display.drawPixel(x + TOP_LEFT_CORNER[0], y + TOP_LEFT_CORNER[1] - 1, AMBER);
        image[x][y] = 1;
      }
    }
  }
};

#endif
