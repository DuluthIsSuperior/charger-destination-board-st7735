#include "display.h"
#include <avr/pgmspace.h>

Display display;

const int *const lineData[] PROGMEM = {  // character (number of points + starting offset)
  1, 0, 2, 0, 0, 1, 0, 6, 3, 1, 3, 6, 1, 4, 2, 4, // A
  0, 0, 0, 6, 1, 0, 2, 0, 1, 3, 2, 3, 1, 6, 2, 6, 3, 1, 3, 2, 3, 4, 3, 5, // B
  0, 1, 0, 5, 1, 0, 2, 0, 3, 1, 3, 1, 1, 6, 2, 6, 3, 5, 3, 5, // C
  0, 0, 0, 6, 1, 0, 2, 0, 1, 6, 2, 6, 3, 1, 3, 5, // D
  0, 0, 0, 6, 1, 0, 3, 0, 1, 3, 2, 3, 1, 6, 3, 6,  // E
  0, 0, 0, 6, 1, 0, 3, 0, 1, 3, 2, 3,  // F
  0, 1, 0, 5, 1, 0, 2, 0, 3, 1, 3, 1, 1, 6, 2, 6, 3, 3, 3, 5, 2, 3, 2, 3, // G
  0, 0, 0, 6, 1, 3, 2, 3, 3, 0, 3, 6,  // H
  0, 0, 0, 6, // I
  3, 0, 3, 5, 2, 6, 1, 6, 0, 5, 0, 5,  // J
  0, 0, 0, 6, 1, 2, 3, 0, 1, 4, 3, 6,  // K
  0, 0, 0, 6, 1, 6, 3, 6, // L
  0, 0, 0, 6, 1, 1, 2, 2, 3, 1, 3, 1, 4, 0, 4, 6, // M
  0, 0, 0, 6, 1, 0, 3, 6, 4, 0, 4, 6, // N
  0, 1, 0, 5, 1, 0, 2, 0, 3, 1, 3, 5, 1, 6, 2, 6, // O
  0, 0, 0, 6, 1, 0, 2, 0, 1, 3, 2, 3, 3, 1, 3, 2, // P
  0, 1, 0, 5, 1, 0, 2, 0, 1, 6, 1, 6, 3, 1, 3, 4, 2, 5, 3, 6, // Q
  0, 0, 0, 6, 1, 0, 2, 0, 1, 3, 2, 3, 3, 1, 3, 2, 1, 4, 3, 6, // R
  3, 1, 3, 1, 2, 0, 1, 0, 0, 1, 0, 2, 1, 3, 2, 3, 3, 4, 3, 5, 2, 6, 1, 6, 0, 5, 0, 5, // S
  0, 0, 4, 0, 2, 1, 2, 6, // T
  0, 0, 0, 5, 1, 6, 2, 6, 3, 0, 3, 5, // U
  2, 6, 0, 0, 3, 4, 4, 0, // V
  0, 0, 0, 6, 1, 5, 2, 4, 3, 5, 4, 6, 4, 5, 4, 0, // W
  1, 2, 3, 4, 3, 2, 1, 4, 0, 0, 0, 1, 4, 0, 4, 1, 0, 5, 0, 6, 4, 5, 4, 6, // X
  2, 6, 2, 3, 1, 3, 1, 2, 0, 1, 0, 0, 3, 3, 3, 2, 4, 1, 4, 0, // Y
  0, 0, 3, 0, 3, 1, 1, 3, 0, 4, 0, 5, 0, 6, 3, 6, // Z
  0, 1, 0, 5, 1, 0, 2, 0, 3, 1, 3, 5, 1, 6, 2, 6, // 0
  0, 1, 0, 1, 1, 0, 1, 5, 0, 6, 2, 6, // 1
  0, 1, 0, 1, 1, 0, 2, 0, 3, 1, 3, 2, 2, 3, 2, 3, 1, 4, 1, 4, 0, 5, 0, 5, 0, 6, 0, 6, 1, 6, 3, 6, // 2
  0, 1, 0, 1, 1, 0, 2, 0, 3, 1, 3, 2, 1, 3, 2, 3, 3, 4, 3, 5, 1, 6, 2, 6, 0, 5, 0, 5, // 3
  4, 4, 4, 4, 3, 6, 3, 0, 2, 4, 0, 4, 0, 3, 0, 3, 1, 2, 1, 2, 2, 1, 2, 1, 3, 0, 3, 0, // 4
  3, 0, 0, 0, 0, 1, 0, 2, 1, 2, 2, 2, 3, 3, 3, 5, 2, 6, 1, 6, 0, 5, 0, 5, // 5
  3, 1, 3, 1, 2, 0, 1, 0, 0, 1, 0, 5, 1, 6, 2, 6, 3, 5, 3, 4, 2, 3, 1, 3, // 6
  0, 0, 3, 0, 3, 1, 1, 6, // 7
  1, 0, 2, 0, 0, 1, 0, 2, 3, 1, 3, 2, 1, 3, 2, 3, 0, 4, 0, 5, 3, 4, 3, 5, 1, 6, 2, 6, // 8
  0, 1, 0, 2, 1, 0, 2, 0, 3, 1, 3, 5, 1, 3, 2, 3, 1, 6, 2, 6, // 9
  0, 0, 0, 0, // space
  0, 0, 0, 0, 2, 0, 2, 0, 1, 1, 1, 1, 0, 2, 0, 2, 2, 2, 2, 2, 1, 3, 1, 3, 0, 4, 0, 4, 2, 4, 2, 4, 1, 5, 1, 5, 0, 6, 0, 6, 2, 6, 2, 6, // unknown
  0, 5, 0, 6, 1, 5, 1, 6,  // .
  0, 4, 0, 5, 0, 1, 0, 2,  // :
};

// String format: Symbol (first character), number of lines, width, starting line data offset (first two digits), starting line data offset (last two digits)
// char is represented as an unsigned 8 bit integer - max number is 255 which is why the offset is split
const int LENGTH_OF_CHAR_DATA = 5;
const int NUMBER_OF_SYMBOLS = 40;
const char *const characterData[LENGTH_OF_CHAR_DATA * NUMBER_OF_SYMBOLS] PROGMEM = {
  // X indicates that I have not seen how this character is rendered on the real unit
   'A', 4,4, 0, 0,  // can't start with a leading 0 for offset (i.e. 00, 00 doesn't work) since an integer that starts with a leading 0 indicates an octal representation)
   'B', 6,4, 0,16,   // X
   'C', 5,4, 0,40,
   'D', 4,4, 0,60,   // X
   'E', 4,4, 0,76,
   'F', 3,4, 0,92,   // X
   'G', 6,4, 1, 4,
   'H', 3,4, 1,28,
   'I', 1,1, 1,40,
   'J', 3,4, 1,44,  // X
   'K', 3,4, 1,56,
   'L', 2,4, 1,68,
   'M', 4,5, 1,76,
   'N', 3,5, 1,88,
   'O', 4,4, 2, 4,
   'P', 4,4, 2,20,
   'Q', 5,4, 2,36,
   'R', 5,4, 2,56,
   'S', 7,4, 2,76,
   'T', 2,5, 3, 4,
   'U', 3,4, 3,12,
   'V', 2,5, 3,24,
   'W', 4,5, 3,32,  // X
   'X', 6,5, 3,48,  // X
   'Y', 5,5, 3,72,
   'Z', 4,4, 3,92,
   '0', 4,4, 4, 8, // X
   '1', 3,3, 4,24, // X
   '2', 8,4, 4,36, // X
   '3', 7,4, 4,68,
   '4', 7,5, 4,96, // X
   '5', 6,4, 5,24, // X
   '6', 6,4, 5,48, // X
   '7', 2,4, 5,72, // X
   '8', 7,4, 5,80, // X
   '9', 5,4, 6, 8,
   ' ', 0,1, 6,28,
  '\0',11,3, 6,32,
   '.', 2,2, 6,76,  // X
   ':', 2,1, 6,84,  // X
};

struct Map {
  int getCharacterData(char character) {
    for (int i = 0; i < sizeof(characterData) / sizeof(char*); i += LENGTH_OF_CHAR_DATA) {
      if (pgm_read_word(&characterData[i]) == character) {
        return i;
      }
    }
    display.setCursor(0, 18);
    display.setTextColor(0xFF000);
    display.setTextSize(1);
    display.printText("ERROR");
    display.setCursor(0, 26);
    display.printText("UNKNOWN CHAR");
    return 716;
  }
};
