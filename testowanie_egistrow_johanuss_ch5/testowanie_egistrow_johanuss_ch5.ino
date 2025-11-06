#include <MIDI.h>

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// ─── USTAWIENIA ───────────────────────────────
const byte totalChannels = 4;      // kanały 1–4 dla CH-55
const byte stopChannel = 12;       // kanał dla rejestrów (działa u Ciebie)
const byte firstStop = 0;          // pierwszy numer rejestru
const byte lastStop  = 63;         // ostatni numer rejestru do testu
const unsigned long stopInterval = 800; // odstęp między kolejnymi rejestrami [ms]

// Akord C-dur (C4–E4–G4 = 60, 64, 67)
const byte chord[] = {60, 64, 67};
const byte chordSize = sizeof(chord);

// ─── SETUP ─────────────────────────────────────
void setup() {
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OFF);
  Serial.println("=== JOHANNUS CH-55 REGISTRY TEST ===");

  // Zagraj akord C-dur na kanałach 1–4
  for (byte ch = 1; ch <= totalChannels; ch++) {
    for (byte i = 0; i < chordSize; i++) {
      MIDI.sendNoteOn(61, 100, ch);
MIDI.sendNoteOn(65, 100, ch);
MIDI.sendNoteOn(68, 100, ch);
      delay(3);
    }
  }
  Serial.println("Akord C-dur ON (kanały 1–4)");

  delay(1000);
}

// ─── LOOP ──────────────────────────────────────
void loop() {
  static byte pcNumber = firstStop;
  static unsigned long lastTime = 0;

  unsigned long now = millis();
  if (now - lastTime >= stopInterval) {
    lastTime = now;

    Serial.print("Włączam rejestr PC#");
    Serial.println(pcNumber);

    // włącz dany rejestr (toggle)
    MIDI.sendProgramChange(pcNumber, stopChannel);

    pcNumber++;
    if (pcNumber > lastStop) {
      pcNumber = firstStop;
      Serial.println("=== Powrót do PC#0 ===");
      delay(1500);
    }
  }
}
