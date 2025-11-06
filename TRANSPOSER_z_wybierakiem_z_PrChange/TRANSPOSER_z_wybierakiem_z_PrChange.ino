#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// --- Wybierak 1x7 ---
const int commonPin = 2;                 // wspólny pin -> INPUT_PULLUP
const int selectorPins[7] = {3, 4, 5, 6, 7, 8, 9}; // pozostałe piny -> OUTPUT LOW
const int transposeValues[7] = {-3, -2, -1, 0, 1, 2, 3};
int currentTranspose = 0;
int lastPosition = -1;

// --- Statyczna mapa nut -> Program Change ---
// [0] = nuta MIDI, [1] = numer Program Change
const int noteTable[2][61] = {
  {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
   76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
   20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
   40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60}
};

int getPCfromNote(int note) {
  if (note < 36 || note > 96) return -1; // poza zakresem
  return noteTable[1][note - 36];
}

void setup() {
  Serial.begin(31250);
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();

  // --- Wybierak ---
  pinMode(commonPin, INPUT_PULLUP);
  for (int i = 0; i < 7; i++) {
    pinMode(selectorPins[i], OUTPUT);
    digitalWrite(selectorPins[i], LOW);
  }

  // Startowa pozycja
  scanSelector();

  // --- MIDI handlers ---
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
}

void loop() {
  MIDI.read();
  scanSelector();
}

// --- Funkcja skanująca wybierak ---
void scanSelector() {
  int detectedPosition = -1;

  for (int i = 0; i < 7; i++) {
    // ustaw tylko jeden pin jako OUTPUT LOW
    pinMode(selectorPins[i], OUTPUT);
    digitalWrite(selectorPins[i], LOW);

    // pozostałe piny jako INPUT
    for (int j = 0; j < 7; j++) {
      if (j != i) pinMode(selectorPins[j], INPUT);
    }

    // odczyt wspólnego pinu
    if (digitalRead(commonPin) == LOW) {
      detectedPosition = i;
      break;
    }

    // przywracamy pin jako INPUT
    pinMode(selectorPins[i], INPUT);
  }

  // Aktualizacja transpozycji tylko jeśli zmiana
  if (detectedPosition != -1 && lastPosition != detectedPosition) {
    currentTranspose = transposeValues[detectedPosition];
    lastPosition = detectedPosition;
    allNotesOffExceptChannel(5);
  }
}

// --- Obsługa MIDI ---
void handleNoteOn(byte channel, byte note, byte velocity) {
    int newNote = note;

    // Kanał 5: wyślij PC na kanale 12
    if (channel == 5) {
        int pc = getPCfromNote(note);
        if (pc >= 0) MIDI.sendProgramChange(pc, 12);
    } 
    // Wszystkie kanały oprócz 5: transpozycja
    else {
        newNote = note + currentTranspose;
        newNote = constrain(newNote, 0, 127);
    }

    MIDI.sendNoteOn(newNote, velocity, channel);
}

void handleNoteOff(byte channel, byte note, byte velocity) {
    int newNote = note;

    // Kanał 5: wyślij PC na kanale 12
    if (channel == 5) {
        int pc = getPCfromNote(note);
        if (pc >= 0) MIDI.sendProgramChange(pc, 12);
    } 
    // Wszystkie kanały oprócz 5: transpozycja
    else {
        newNote = note + currentTranspose;
        newNote = constrain(newNote, 0, 127);
    }

    MIDI.sendNoteOff(newNote, velocity, channel);
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
