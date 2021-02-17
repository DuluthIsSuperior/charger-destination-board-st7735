#ifndef display_h
#define display_h

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <limits.h>

#define sclk 13
#define mosi 11
#define cs   4
#define rst  3
#define dc   2

const int TOP_LEFT_CORNER[] = {1, 1};
const int BOTTOM_RIGHT_CORNER[] = {49, 14};
const int boardWidth = BOTTOM_RIGHT_CORNER[0] - TOP_LEFT_CORNER[0] + 1;
const int boardHeight = BOTTOM_RIGHT_CORNER[1] - TOP_LEFT_CORNER[1] + 1;
char image[boardWidth][boardHeight]; // [x][y]: 0 = black, 1 = amber, 2 = change to black, 3 = change to amber (using char since it's shorter than an int)
const int BLACK = 0x000000;
const int AMBER = 0xFFB400;

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
    fillScreen(int color);
    clearDestinationBoard();
    drawImage();
  private:
    static Adafruit_SSD1331 display;
    static bool running;
};

Adafruit_SSD1331 Display::display = Adafruit_SSD1331(&SPI, cs, dc, rst);
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
    display.begin();
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    fillScreen(0x0);
//    drawRect(TOP_LEFT_CORNER[0] - 1, TOP_LEFT_CORNER[1] - 1, BOTTOM_RIGHT_CORNER[0] + 2, BOTTOM_RIGHT_CORNER[1] + 2, AMBER);
    running = true;
  }
};

bool withinBounds(int x, int y) {
  return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight;
}

Display::drawPixel(int x, int y, int color) {
  if (x < boardWidth && x >= 0 && y < boardHeight && y >= 0) {
    if (color == BLACK && withinBounds(x, y) && image[x][y] != 0 && image[x][y] != 2) {
      image[x][y] = 2;
    } else if (color == AMBER && withinBounds(x, y)) {
      image[x][y] = 3;
    }
  }
}

Display::shiftImage(int x) {
  int c = 0;
  for (int xx = boardWidth - 1 - x; xx >= 1; xx--) {
    for (int yy = 1; yy < boardHeight; yy++) {
      drawPixel(xx + x, yy, image[xx][yy] == 3 ? AMBER : BLACK);
      drawPixel(xx, yy, BLACK);
    }
  }
}

Display::drawLine(int startX, int startY, int endX, int endY, int color) {
  startX += TOP_LEFT_CORNER[0];
  startY += TOP_LEFT_CORNER[1];
  endX   += TOP_LEFT_CORNER[0];
  endY   += TOP_LEFT_CORNER[1];
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

Display::drawRect(int startX, int startY, int endX, int endY, int color) {
  display.drawRect(startX, startY, endX, endY, color);
};

Display::fillScreen(int color) {
  display.fillScreen(color);
};

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
