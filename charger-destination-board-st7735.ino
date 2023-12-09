#include "map.h"

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

const char voltage_format[] PROGMEM = "%s%d.%02dV";
const char runtime_format[] PROGMEM = "%d:%s%d:%s%d:%s%d";

bool scrolling = true;        // dictates whether the message inside the destination board scrolls
bool disableScrolling = true; // if debugging with messageId != 1, then set this to false
bool messageChanged = true;   // initalized to true to let the message initially show up
char str[21];                 // local copy of the string from flash
int messageWidth = 0;         // width of the message in pixels
int x = 2;                    // top-left x coordinate of the message in the destination board (changes as the message scrolls)
int y = 5;                    // top-left y coordinate of the message in the destination board (changes as the message scrolls)
struct Map char_map;
long lastMoved = millis();    // used to keep track of the last time the message was scrolled across the screen
long lastMeasured = millis(); // used to keep track of the last time status was queried (volts, amps, etc.)
int messageId = 1;            // index of the message in destinations[] that is currently being displayed

void setup() {
    Serial.begin(9600);
    while (!Serial) {}
    Serial.println(F("Serial established"));
    pinMode(A0, INPUT);
    pinMode(5, INPUT);
    display.begin();
}

void printMessage(bool findWidth) {
    int tempX = x;
    if (findWidth) {
        messageWidth = 0;
    }

    for (int i = 0; i < sizeof(str) / sizeof(char) - 1; i++) {
        if (str[i] != 0) {
            int index = char_map.getCharacterData(str[i]);
            int numberOfLines = pgm_read_word(&characterData[index + 1]);
            int offset = (pgm_read_word(&characterData[index + 3]) * 100) + pgm_read_word(&characterData[index + 4]);
            for (int i = 0; i < numberOfLines; i++) {
                display.drawLine(
                    pgm_read_word(&lineData[offset + 0]) + tempX,
                    pgm_read_word(&lineData[offset + 1]) + y,
                    pgm_read_word(&lineData[offset + 2]) + tempX,
                    pgm_read_word(&lineData[offset + 3]) + y,
                    AMBER
                );
                offset += 4;
            }
            int add = pgm_read_word(&characterData[index + 2]) + 1;  // read value for width of character, then add 1 for spacing between characters
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
    if (!scrolling) { // if scrolling is disabled, center the message inside of the destination board
        display.shiftImage(((boardWidth - messageWidth) / 2) - 2);
    }

    display.drawImage();
}

bool isStatus(int messageId) {
    return messageId >= 0 && messageId <= 1;
}

void changeMessage() {
    messageId++;
    // if the message ID exceeds the size of the messages array, reset it to 0
    if (messageId % (sizeof(destinations) / sizeof(char*)) == 0) {
        messageId = 0;
    }
    messageChanged = true;
}

bool old5 = digitalRead(5) == HIGH;
int zeroCount = 0;
void loop() {
    if (messageChanged) {
        if (!isStatus(messageId)) {
            x = 2;
            memset(str, 0, sizeof(str));  // zeros out the string
            strncpy_P(str, pgm_read_word(&destinations[messageId]), 50);  // doesn't work for some reason
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
        sprintf_P(str, voltage_format, (voltage < 10.00 ? "0" : ""), (int) voltage, (int) (voltage * 100.0) % 100);
        x = 2;
        printMessage(true);
        lastMeasured = millis();
        messageChanged = false;
    } else if (messageId == 1 && (messageChanged || millis() - lastMeasured >= 1000)) {
        disableScrolling = true;
        long time = millis(); // millis always returns the system up time
        int days = (time / 86400000) % 9;
        int hours = (time / 3600000) % 24;
        int minutes = (time / 60000) % 60;
        int seconds = (time / 1000) % 60;
        memset(str, 0, sizeof(str));
        sprintf_P(str, runtime_format, days, hours < 10 ? "0" : "", hours, minutes < 10 ? "0" : "", minutes, seconds < 10 ? "0" : "", seconds); // _P functions let us provide direct pointers to flash memory
        x = 2;
        printMessage(true);
        lastMeasured = millis();
        messageChanged = false;
    } else {
        disableScrolling = false;
    }
    // bool current5 = digitalRead(5) == HIGH;
    // if (current5) {
    //     Serial.print(1);
    // } else {
    //     Serial.print(0);
    // }
    // if (++zeroCount == 100) {
    //     Serial.println();
    //     zeroCount = 0;
    // }

    // // advance to the next message if function is toggled from the decoder
    // if (current5 && !old5) {
    //     // HIGH if F3 on decoder is off
    //     old5 = true;
    //     changeMessage();
    //     Serial.println(zeroCount);
    //     zeroCount = 0;
    // } else if (!current5) {
    //     // 1M pulls signal down properly
    //     zeroCount++;
    //     if (old5 && zeroCount > 2000) {
    //         old5 = false;
    //         changeMessage();
    //     }
    // }
}
