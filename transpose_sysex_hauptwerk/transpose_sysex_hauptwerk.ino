#include <Wire.h>
#include <U8g2lib.h>
#include "MIDIUSB.h"

// Inicjalizacja wyświetlacza SH1106 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

const int SYSEX_MAX = 32;   // maksymalnie 32 bajty do wyświetlenia
byte sysexBuffer[SYSEX_MAX];
int sysexIndex = 0;
bool receivingSysEx = false;

void setup() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "SysEx Hauptwerk:");
  u8g2.sendBuffer();
}

void loop() {
  midiTask();
}

void midiTask() {
  midiEventPacket_t rx;
  
  while ((rx = MidiUSB.read()).header != 0) {
    byte b = rx.byte1;

    if (b == 0xF0) {       // początek SysEx
      receivingSysEx = true;
      sysexIndex = 0;
    } 
    else if (b == 0xF7) {  // koniec SysEx
      receivingSysEx = false;
      displaySysEx();
    } 
    else if (receivingSysEx) {
      if (sysexIndex < SYSEX_MAX) {
        sysexBuffer[sysexIndex++] = b;
      }
    }

    // Obsługa pozostałych bajtów w pakiecie (byte2, byte3)
    byte bytes[] = {rx.byte2, rx.byte3};
    for (int i = 0; i < 2; i++) {
      b = bytes[i];
      if (b == 0) continue;
      if (b == 0xF0) {
        receivingSysEx = true;
        sysexIndex = 0;
      } 
      else if (b == 0xF7) {
        receivingSysEx = false;
        displaySysEx();
      } 
      else if (receivingSysEx && sysexIndex < SYSEX_MAX) {
        sysexBuffer[sysexIndex++] = b;
      }
    }
  }
}

// Funkcja wyświetlająca zawartość sysex w HEX
void displaySysEx() {
  char line[64]; 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 12, "SysEx Hauptwerk:");

  int y = 24;
  for (int i = 0; i < sysexIndex; i++) {
    sprintf(line, "%02X ", sysexBuffer[i]);
    u8g2.drawStr((i % 16) * 8, y, line);
    if ((i + 1) % 16 == 0) y += 12;
  }

  u8g2.sendBuffer();
}
