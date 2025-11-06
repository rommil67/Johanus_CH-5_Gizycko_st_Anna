#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// --- Ustawienia ---
const byte MIDI_CHANNEL = 5;  // kanał MIDI (1–16)
const int potPin = A0;        // wejście potencjometru
const int DEADZONE = 8;       // martwa strefa
const int VELOCITY = 100;     // stała głośność nut

// 🔸 Tablica nut (dowolna kolejność i długość)
byte notes[] = {
  36, 38, 40, 41, 43, 45, 47, 48,
  50, 52, 53, 55, 57, 59, 60, 62,
  64, 65, 67, 69, 71, 72, 74, 76,
  77, 79, 81, 83, 84, 86, 88, 89,
  91, 93, 95, 96, 98, 100, 101, 103,
  105, 107, 108
};
const int notesCount = sizeof(notes) / sizeof(notes[0]);

// --- LED wskaźnik (5 diod) ---
const byte ledPins[5] = {2, 3, 4, 5, 6};

// --- Zmienne stanu ---
int lastIndex = -1;        // ile nut było aktywnych ostatnio
int lastPotValue = 0;      // ostatni odczyt potencjometru
int lastLedLevel = -1;     // ostatnia liczba zapalonych diod

// --- Czas do nieblokującego odświeżania ---
unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 5; // co ile ms odświeżać potencjometr/LED

// --- Setup ---
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI); // nasłuchuj wszystkie kanały
  Serial.begin(31250);           // prędkość MIDI

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
}

// --- Pętla główna ---
void loop() {
  // --- MIDI THRU (pełna prędkość, bez przerw) ---
  if (MIDI.read()) {
    byte type = MIDI.getType();
    byte chan = MIDI.getChannel();
    byte d1 = MIDI.getData1();
    byte d2 = MIDI.getData2();
    MIDI.send(type, d1, d2, chan);
  }

  // --- Odczyt potencjometru i LED co kilka ms ---
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;

    int potValue = analogRead(potPin);
    if (abs(potValue - lastPotValue) > DEADZONE) {
      lastPotValue = potValue;

      // przelicz wartość potencjometru na ilość nut
      int index = map(potValue, 0, 1023, 0, notesCount);
      if (index < 0) index = 0;
      if (index > notesCount) index = notesCount;

      // zmiana liczby aktywnych nut
      if (index != lastIndex) {
        if (index > lastIndex) {
          for (int i = lastIndex + 1; i < index; i++) {
            MIDI.sendNoteOn(notes[i], VELOCITY, MIDI_CHANNEL);
          }
        } else {
          for (int i = lastIndex - 1; i >= index; i--) {
            MIDI.sendNoteOff(notes[i], 0, MIDI_CHANNEL);
          }
        }
        lastIndex = index;
      }

      // pozycja 0 – wyłącz wszystkie
      if (index == 0 && lastIndex != 0) {
        for (int i = 0; i < notesCount; i++) {
          MIDI.sendNoteOff(notes[i], 0, MIDI_CHANNEL);
        }
        lastIndex = 0;
      }

      // wskaźnik LED
      int ledLevel = map(potValue, 0, 1023, 0, 5);
      if (ledLevel != lastLedLevel) {
        for (int i = 0; i < 5; i++) {
          digitalWrite(ledPins[i], (i < ledLevel) ? HIGH : LOW);
        }
        lastLedLevel = ledLevel;
      }
    }
  }
}

