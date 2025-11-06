#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

unsigned long lastMillis = 0;
uint8_t pcIndex = 0;
uint8_t noteIndex = 0;

const unsigned int pcInterval  = 120;  // ms między Program Change
const unsigned int noteOnTime  = 120;  // czas trwania nuty
const unsigned int noteOffTime = 50;   // przerwa między nutami

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI); // THRU działa automatycznie
  Serial.begin(31250);           // standard MIDI

  // ---- 1. Program Change 0..51 na kanale 12 ----
  for (pcIndex = 0; pcIndex <= 51; pcIndex++) {
    MIDI.sendProgramChange(pcIndex, 12);

    // przepuszczanie MIDI w trakcie oczekiwania
    unsigned long start = millis();
    while (millis() - start < pcInterval) {
      MIDI.read();
    }
  }

  // ---- 2. 61 nut chromatycznych od 36 na kanałach 1→4 ----
  for (uint8_t channel = 1; channel <= 4; channel++) {
    for (noteIndex = 0; noteIndex < 61; noteIndex++) {

      uint8_t note = 36 + noteIndex;

      MIDI.sendNoteOn(note, 100, channel);
      unsigned long onStart = millis();
      while (millis() - onStart < noteOnTime) {
        MIDI.read(); // przepuszcza komunikaty
      }

      MIDI.sendNoteOff(note, 0, channel);
      unsigned long offStart = millis();
      while (millis() - offStart < noteOffTime) {
        MIDI.read();
      }

    } // koniec pętli noteIndex
  } // koniec pętli channel
} // koniec setup()

void loop() {
  // po zakończeniu testu działa tylko MIDI THRU
  MIDI.read();
} // koniec loop()
