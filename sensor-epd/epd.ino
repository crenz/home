#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
GxEPD2_3C<GxEPD2_750c_Z90, GxEPD2_750c_Z90::HEIGHT / 4> display(GxEPD2_750c_Z90(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5)); // GDEH075Z90 880x528


void epdSetup() {
    msgln("epd", "Setup display " + String(display.width()) + "x" + String(display.height()));
    delay(100);
    display.init(115200);
    return;
    
    display.setFullWindow();
    display.fillScreen(GxEPD_WHITE);
    display.display(true);
    display.powerOff();
}

void epdLoop() {
  
}

void epdDrawChunk(uint16_t color, uint8_t chunk, byte * data, int len) {
    int chunkRows = display.height() / 4;
    for (int y = (chunk * chunkRows); y < ((chunk + 1) * chunkRows); y++) {
       for (int x = 0; x < display.width(); x++) {
           int bytePos = y * display.width() + (x / 8);
           byte pixel = data[bytePos];
           if (pixel && pow(2, x % 8)) {
              display.drawPixel(x, y, color);
           }
        }
    }
}

void epdProcessMessage(char topic[], char payload[], int payload_length) {
    // we expect 8 messages: /black/1 to /black/4 and /red/1 to /red/4
    if (strstr(topic, "/black/1") != NULL) {
        msgln("epd", "Black, #1");
        display.setFullWindow();
//        display.setPartialWindow(0, 0, size, size);
        epdDrawChunk(GxEPD_BLACK, 1, (byte*) payload, payload_length);
    } else if (strstr(topic, "/black/2") != NULL) {
        msgln("epd", "Black, #2");
        epdDrawChunk(GxEPD_BLACK, 2, (byte*) payload, payload_length);
    } else if (strstr(topic, "/black/3") != NULL) {
        msgln("epd", "Black, #3");
        epdDrawChunk(GxEPD_BLACK, 3, (byte*) payload, payload_length);
    } else if (strstr(topic, "/black/4") != NULL) {
        msgln("epd", "Black, #4");
        epdDrawChunk(GxEPD_BLACK, 4, (byte*) payload, payload_length);
    } else if (strstr(topic, "/red/1") != NULL) {
        msgln("epd", "Red, #1");
        epdDrawChunk(GxEPD_RED, 1, (byte*) payload, payload_length);
    } else if (strstr(topic, "/red/2") != NULL) {
        msgln("epd", "Red, #2");
        epdDrawChunk(GxEPD_RED, 2, (byte*) payload, payload_length);
    } else if (strstr(topic, "/red/3") != NULL) {
        msgln("epd", "Red, #3");
        epdDrawChunk(GxEPD_RED, 3, (byte*) payload, payload_length);
    } else if (strstr(topic, "/red/4") != NULL) {
        msgln("epd", "Red, #4");
        epdDrawChunk(GxEPD_RED, 4, (byte*) payload, payload_length);        
        display.display(true);
 //       display.displayWindow(0, 0, size, size, false);
        display.powerOff();
    } else {
        msgln("epd", "Unknown message");
    }
}
