#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// --- Piny wybieraka ---
const int commonPin = 11;                  // wspólny pin wybieraka -> INPUT_PULLUP
const int selectorPins[7] = {3, 4, 5, 6, 7, 8, 9}; // pozostałe 7 pinów -> OUTPUT LOW

const int transposeValues[7] = {-3, -2, -1, 0, 1, 2, 3};
int currentTranspose = 0;
int lastPosition = -1;

void setup() {
  Serial.begin(31250);            // MIDI baudrate
  MIDI.begin(MIDI_CHANNEL_OMNI);  // odbiór wszystkich kanałów
  MIDI.turnThruOff();

  // ustaw wspólny pin jako INPUT_PULLUP
  pinMode(commonPin, INPUT_PULLUP);

  // ustaw pozostałe piny jako OUTPUT LOW
  for (int i = 0; i < 7; i++) {
    pinMode(selectorPins[i], OUTPUT);
    digitalWrite(selectorPins[i], LOW);
  }

  // startowa pozycja wybieraka
  scanSelector();

  // ustaw obsługę komunikatów
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
}

void loop() {
  MIDI.read();  // odbieraj komunikaty MIDI
  scanSelector(); // sprawdzaj aktualną pozycję wybieraka
}

// --- Funkcja skanująca wybierak ---
void scanSelector() {
  int detectedPosition = -1;

  for (int i = 0; i < 7; i++) {
    // ustaw tylko jeden pin jako OUTPUT LOW
    pinMode(selectorPins[i], OUTPUT);
    digitalWrite(selectorPins[i], LOW);

    // pozostałe piny jako INPUT, nie wymuszamy LOW
    for (int j = 0; j < 7; j++) {
      if (j != i) pinMode(selectorPins[j], INPUT);
    }

    // odczyt wspólnego pinu
    if (digitalRead(commonPin) == LOW) {
      detectedPosition = i;
      break; // pierwsza aktywna pozycja
    }

    // przywrócenie skanowanego pinu do INPUT
    pinMode(selectorPins[i], INPUT);
  }

  // aktualizacja transpozycji tylko jeśli wykryto zmianę
  if (detectedPosition != -1 && lastPosition != detectedPosition) {
    currentTranspose = transposeValues[detectedPosition];
    lastPosition = detectedPosition;
    allNotesOffExceptChannel(5);
  }
}


// --- Obsługa MIDI ---
void handleNoteOn(byte channel, byte note, byte velocity) {
  if (channel == 5) {
    MIDI.sendNoteOn(note, velocity, channel);
  } else {
    int newNote = note + currentTranspose;
    newNote = constrain(newNote, 0, 127);
    MIDI.sendNoteOn(newNote, velocity, channel);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == 5) {
    MIDI.sendNoteOff(note, velocity, channel);
  } else {
    int newNote = note + currentTranspose;
    newNote = constrain(newNote, 0, 127);
    MIDI.sendNoteOff(newNote, velocity, channel);
  }
}

void handleCC(byte channel, byte control, byte value) {
  MIDI.sendControlChange(control, value, channel);
}

// --- NoteOff dla wszystkich kanałów oprócz wskazanego ---
void allNotesOffExceptChannel(byte excludedChannel) {
  for (byte ch = 1; ch <= 16; ch++) {
    if (ch == excludedChannel) continue;
    for (byte n = 0; n < 128; n++) {
      MIDI.sendNoteOff(n, 0, ch);
    }
  }
}
