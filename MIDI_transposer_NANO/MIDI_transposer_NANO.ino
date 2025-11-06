#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// 11 przycisków: D2–D12 = transpozycja od -5 do +5
const int NUM_BUTTONS = 11;
const int buttonPins[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const int transposeValues[NUM_BUTTONS] = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};

int currentTranspose = 0;
int lastButton = -1;

bool buttonState[NUM_BUTTONS];
bool lastButtonState[NUM_BUTTONS];

void setup() {
  Serial.begin(31250);            // MIDI baudrate
  MIDI.begin(MIDI_CHANNEL_OMNI);  // odbiór wszystkich 
  MIDI.turnThruOff();

  // Ustaw piny przycisków
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttonState[i] = lastButtonState[i] = digitalRead(buttonPins[i]);
  }

  // Start z wciśniętym przyciskiem (np. ustalenie transpozycji)
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      currentTranspose = transposeValues[i];
      lastButton = i;
      break;
    }
  }

  // Ustaw obsługę komunikatów
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleCC);
}

void loop() {
  MIDI.read();  // odbieraj komunikaty MIDI

  // Sprawdź stan przycisków
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonState[i] = digitalRead(buttonPins[i]);

    // wykrycie nowego naciśnięcia
    if (buttonState[i] == LOW && lastButtonState[i] == HIGH) {
      if (lastButton != i) {
        currentTranspose = transposeValues[i];
        lastButton = i;
        allNotesOffExceptChannel(5);
      }
    }

    lastButtonState[i] = buttonState[i];
  }
}

// --- Obsługa MIDI ---

void handleNoteOn(byte channel, byte note, byte velocity) {
  if (channel == 5) {
    // kanał 5 bez zmian
    MIDI.sendNoteOn(note, velocity, channel);
  } else {
    int newNote = note + currentTranspose;
    newNote = constrain(newNote, 0, 127);
    MIDI.sendNoteOn(newNote, velocity, channel);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  if (channel == 5) {
    // kanał 5 bez zmian
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
