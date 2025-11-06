#include "MIDIUSB.h"

const int SYSEX_MAX = 32;
byte sysexBuffer[SYSEX_MAX];
int sysexIndex = 0;
bool receivingSysEx = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Start logowania SysEx");
}

void loop() {
  midiTask();
}

void midiTask() {
  midiEventPacket_t rx;

  while ((rx = MidiUSB.read()).header != 0) {
    byte bytes[] = {rx.byte1, rx.byte2, rx.byte3};

    for (int i = 0; i < 3; i++) {
      byte b = bytes[i];
      if (b == 0) continue;

      if (b == 0xF0) {       // początek SysEx
        receivingSysEx = true;
        sysexIndex = 0;
      } 
      else if (b == 0xF7) {  // koniec SysEx
        receivingSysEx = false;
        printSysEx();
      } 
      else if (receivingSysEx && sysexIndex < SYSEX_MAX) {
        sysexBuffer[sysexIndex++] = b;
      }
    }
  }
}

void printSysEx() {
  Serial.print("SysEx: ");
  for (int i = 0; i < sysexIndex; i++) {
    Serial.print(sysexBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
