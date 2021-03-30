#ifndef display_h
#define display_h

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <limits.h>

#define cs   4
#define rst  3
#define dc   2
const int TOP_LEFT_CORNER[] = {54, 8};
const int boardWidth = 80;  // prototypically 144 - model is 10.25mm
const int boardHeight = 17; // prototypically 28 - model is 3mm
// prototypical ratio = 144 / 28 = 36 / 7 = 36:7
const int BOTTOM_RIGHT_CORNER[] = {TOP_LEFT_CORNER[0] + boardWidth - 1, TOP_LEFT_CORNER[1] + boardHeight - 1};
byte image[boardWidth][boardHeight]; // [x][y]: 0 = black, 1 = amber, 2 = change to black, 3 = change to amber (using byte since it's shorter than an int)
// different screens with the same driver produce different output - your mileage may vary
const int BLACK = 0x0000;
const int AMBER = 0x0CDF; // BGR?

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
    display.setRotation(1);
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    display.fillScreen(BLACK);
    drawRect(TOP_LEFT_CORNER[0] - 1, TOP_LEFT_CORNER[1] - 1, BOTTOM_RIGHT_CORNER[0] + 1, BOTTOM_RIGHT_CORNER[1] + 1, AMBER);
    running = true;
  }
};

bool withinBounds(int x, int y) {
  return x >= 0 && x < boardWidth && y >= 0 && y < boardHeight;
}

int getValue(byte b, int shift) {
  int value = (b >> shift + 1) & 1 ? 2 : 0;
  value += (b >> shift) & 1 ? 1 : 0;
  return value;
}

Display::drawPixel(int x, int y, int color) {
  if (withinBounds(x, y)) {
    int value = getValue(image[x][y], 0);
    if (color == BLACK && value != 0 && value != 2) {
      image[x][y] &= ~(3u << 0);
      image[x][y] |= 2u << 0;
    } else if (color == AMBER) {
      image[x][y] |= 3u << 0;
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

/**
 * Used to draw a line onto the screen
 * Source: http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
 * Parameters:
 * startX, startY - starting X and Y coordinates respectively
 * endX, endY - ending X and Y coordinates respectively
 * color - color to draw the pixel as (only amber or black are supported at the moment)
 */
Display::drawLine(int x0, int y0, int x1, int y1, int color) {
  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;
  int dy = abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1; 
  int err = (dx > dy ? dx : -dy) / 2;
 
  while (1) {
    drawPixel(x0, y0, color);
    if (x0 == x1 && y0 == y1) { 
      break;
    }
    if (err > -dx) { 
      err -= dy;
      x0 += sx;
    }
    if (err < dy) {
      err += dx;
      y0 += sy;
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
      int value = getValue(image[x][y], 0);
      if (value == 2 || value == 1) {
        display.drawPixel(x + TOP_LEFT_CORNER[0], y + TOP_LEFT_CORNER[1] - 1, BLACK);
        image[x][y] &= ~(3u << 0);
      } else if (value == 3) {
        display.drawPixel(x + TOP_LEFT_CORNER[0], y + TOP_LEFT_CORNER[1] - 1, AMBER);
        image[x][y] &= ~(3u << 0);
        image[x][y] |= 1u << 0;
      }
    }
  }
};

#endif
