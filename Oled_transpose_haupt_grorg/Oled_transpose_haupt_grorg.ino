#include <Wire.h>
#include <U8g2lib.h>
#include "MIDIUSB.h"

// Wyświetlacz SH1106 I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Bufor SysEx
const int SYSEX_MAX = 32;
byte sysexBuffer[SYSEX_MAX];
int sysexIndex = 0;
bool receivingSysEx = false;

// Transpozycja
int transposeValue = 0;     // aktualna wartość
int targetValue = 0;        // docelowa wartość (z SysEx)
unsigned long lastAnimTime = 0;
const int animInterval = 50; // ms między krokami animacji

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_fub35_tr); // duży font LCD
  displayTranspose(transposeValue);

  Serial.begin(115200);
  Serial.println("Start animowanego OLED bez blokowania");
}

void loop() {
  midiTask();
  updateAnimation();
}

// Odbiór USB-MIDI SysEx
void midiTask() {
  midiEventPacket_t rx;

  while ((rx = MidiUSB.read()).header != 0) {
    byte bytes[] = {rx.byte1, rx.byte2, rx.byte3};

    for (int i = 0; i < 3; i++) {
      byte b = bytes[i];
      if (b == 0) continue;

      if (b == 0xF0) { // początek SysEx
        receivingSysEx = true;
        sysexIndex = 0;
      } 
      else if (b == 0xF7) { // koniec SysEx
        receivingSysEx = false;
        processSysEx();
      } 
      else if (receivingSysEx && sysexIndex < SYSEX_MAX) {
        sysexBuffer[sysexIndex++] = b;
      }
    }
  }
}

// Przetwarzanie SysEx
void processSysEx() {
  if (sysexIndex < 4) return;

  int signByte = sysexBuffer[3];
  int valueByte = (sysexIndex > 4) ? sysexBuffer[4] : 0;

  if (signByte == 0x2D) { // minus
    targetValue = -(valueByte - 0x30);
  } else {                // 0 lub plus
    targetValue = signByte - 0x30;
  }

  // Debug
  Serial.print("SysEx: ");
  for (int i = 0; i < sysexIndex; i++) {
    Serial.print(sysexBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" -> Transpozycja docelowa: ");
  Serial.println(targetValue);
}

// Aktualizacja animacji w pętli bez blokowania
void updateAnimation() {
  unsigned long now = millis();
  if (transposeValue == targetValue) return;
  if (now - lastAnimTime < animInterval) return;

  lastAnimTime = now;

  // krok animacji
  if (transposeValue < targetValue) transposeValue++;
  else if (transposeValue > targetValue) transposeValue--;

  displayTranspose(transposeValue);
}

// Wyświetlanie na OLED
void displayTranspose(int value) {
  char buf[6];
  sprintf(buf, "%+d", value); // +N / -N

  u8g2.clearBuffer();

  int16_t x = (128 - u8g2.getStrWidth(buf)) / 2;
  int16_t y = (64 + 35) / 2;
  u8g2.drawStr(x, y, buf);

  u8g2.sendBuffer();
}
