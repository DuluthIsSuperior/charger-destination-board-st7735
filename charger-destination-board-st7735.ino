#include "map.h"
#include "display.h"
#include <stdarg.h>

const char voltage[] PROGMEM = "";
const char runtime[] PROGMEM = "";
const char pere_marquette[] PROGMEM = "370 PERE MARQUETTE";
const char grand_rapids[] PROGMEM = "GRAND RAPIDS";
const char chicago[] PROGMEM = "CHICAGO";
const char illinois_zephyr[] PROGMEM = "ILLINOIS ZEPHYR";
const char blue_water[] PROGMEM = "BLUE WATER";
const char grvmrc[] PROGMEM = "GRVMRC";
const char wolverine[] PROGMEM = "WOLVERINE";

const char *const destinations[] PROGMEM = {voltage, runtime, pere_marquette, grand_rapids, chicago, illinois_zephyr, blue_water, grvmrc, wolverine};

bool scrolling = true;        // dictates whether the message inside the destination board scrolls
bool disableScrolling = true;
bool messageChanged = true;   // initalized to true to let the message initially show up
bool old_A0 = false;          // stores the old status of
char str[21];                 // local copy of the string from flash
int messageWidth = 0;         // width of the message in pixels
int x = 2;                    // top-left x coordinate of the message in the destination board
int y = 3;                    // top-left x coordinate of the message in the destination board
struct Map char_map;
long lastMoved = millis();    // used to keep track of the last time the message was scrolled across the screen
long lastMeasured = millis(); // used to keep track of the last time status was queried (volts, amps, etc.)
long startUp = millis();
int messageId = 1;            // index of the message in destinations[] that is currently being displayed

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  display.begin();
}

int power(int x, int y) {
  return y == 0 ? 1 : x * power(x, y - 1);  // this is here because I'm having problems with the pow function in the math library
}

/**
   Returns the characters converted to one integer given the index for the characterData array
   argc - the number of arguments this function expects to see
   ... - chars to conver to integers from left to right
*/
int charsToIntValue(int argc, ...) {
  va_list argp;           // store pointer to varargs list
  va_start(argp, argc);   // initalize varargs list with pointer and the last non-varargs argument in this method
  int multiplier = power(10, argc - 1);
  int result = 0;
  for (int i = 0; i < argc; i++) {
    int index = va_arg(argp, int);
    char c = pgm_read_word(&characterData[index]);
    result += (c - 48) * multiplier;
    multiplier /= 10;
  }
  va_end(argp);
  return result;
}

void printMessage(bool findWidth) {
  int tempX = x;
  if (findWidth) {
    messageWidth = 0;
  }

  for (int i = 0; i < sizeof(str) / sizeof(char) - 1; i++) {
    if (str[i] != 0) {
      int index = char_map.getCharacterData(str[i]);
      int numberOfLines = charsToIntValue(2, index + 1, index + 2);
      int offset = charsToIntValue(4, index + 4, index + 5, index + 6, index + 7);
      for (int i = 0; i < numberOfLines; i++) {
        int line[4] = {pgm_read_word(&lineData[offset + 0]), pgm_read_word(&lineData[offset + 1]), pgm_read_word(&lineData[offset + 2]), pgm_read_word(&lineData[offset + 3])};
        display.drawLine(line[0] + tempX, line[1] + y, line[2] + tempX, line[3] + y, AMBER);
        offset += 4;
      }
      int add = charsToIntValue(1, index + 3) + 1;  // read value for width of character, then add 1 for spacing between characters
      tempX += add;
      if (findWidth) {
        messageWidth += add;
      }
    } else {
      break;
    }
  }

  if (findWidth) {
    scrolling = messageWidth > (boardWidth - 6) && !disableScrolling;
  }
  if (!scrolling) {
    display.shiftImage(((boardWidth - messageWidth) / 2) - 2);
  }
  
  display.drawImage();
}

bool isStatus(int messageId) {
  return messageId >= 0 && messageId <= 1;
}

void loop() {
  if (messageChanged) {
    if (!isStatus(messageId)) {
      x = 2;
      memset(str, 0, sizeof(str));  // zeros out the string
      strncpy_P(str, pgm_read_word(&destinations[messageId]), 50);
      printMessage(true);
    }
    messageChanged = false;
    Serial.println(messageId);
  }
  if (scrolling && millis() - lastMoved >= 60) {
    if (x < -messageWidth) {
      x = BOTTOM_RIGHT_CORNER[0] + 3;
    } else {
      x--;
    }
    printMessage(false);
    lastMoved = millis();
  }
  if (messageId == 0 && (messageChanged || millis() - lastMeasured >= 100)) {
    disableScrolling = true;
    int value = analogRead(A1);
    float R1 = 47000.00;
    float R2 = 22000.00;
    float voltage = value * (5.0 / 1024) * ((R1 + R2) / R2);
    memset(str, 0, 8);  // zeros out the string
    sprintf(str, "%s%d.%02dV", (voltage < 10.00 ? "0" : ""), (int) voltage, (int) (voltage * 100.0) % 100);
    x = 2;
    printMessage(true);
    lastMeasured = millis();
    messageChanged = false;
  } else if (messageId == 1 && (messageChanged || millis() - lastMeasured >= 1000)) {
    disableScrolling = true;
    long time = millis();
    int days = (time / 86400000) % 9;
    int hours = (time / 3600000) % 24;
    int minutes = (time / 60000) % 60;
    int seconds = (time / 1000) % 60;
    memset(str, 0, sizeof(str));
    sprintf(str, "%d:%s%d:%s%d:%s%d", days, hours < 10 ? "0" : "", hours, minutes < 10 ? "0" : "", minutes, seconds < 10 ? "0" : "", seconds);
    x = 2;
    printMessage(true);
    lastMeasured = millis();
    messageChanged = false;
  } else {
    disableScrolling = false;
  }
  if (digitalRead(A0) == HIGH) {  // if using a button to test this, no code accounts for the button bounce problem
    if (!old_A0) {
      messageId++;
      if (messageId % (sizeof(destinations) / sizeof(char*)) == 0) {
        messageId = 0;
      }
      messageChanged = true;
      old_A0 = true;
    }
  } else {
    old_A0 = false;
  }
}
