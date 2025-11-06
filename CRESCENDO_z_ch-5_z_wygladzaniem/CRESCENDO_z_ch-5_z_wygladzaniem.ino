#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();

// --- Ustawienia ---
const byte MIDI_CHANNEL = 5;   // kanał MIDI (1–16)
const int potPin = A0;         // wejście potencjometru
const int DEADZONE = 8;        // martwa strefa (różnica ADC)
const int VELOCITY = 100;      // stała głośność nut
const byte FILTER_SIZE = 8;    // liczba próbek do średniej (im więcej, tym płynniej)
const unsigned long UPDATE_INTERVAL = 5; // odświeżanie potencjometru i LED (ms)

// --- Tablica nut ---
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

// --- Zmienne pomocnicze ---
int lastIndex = -1;
int lastPotValue = 0;
int lastLedLevel = -1;
unsigned long lastUpdateTime = 0;

// --- Bufor do filtra średniej kroczącej ---
int potBuffer[FILTER_SIZE];
byte potIndex = 0;
bool bufferFilled = false;

// --- Setup ---
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(31250); // prędkość MIDI

  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Wstępne zainicjowanie bufora
  for (int i = 0; i < FILTER_SIZE; i++) {
    potBuffer[i] = analogRead(potPin);
  }
}

// --- Funkcja: filtr średniej kroczącej ---
int readFilteredPot() {
  potBuffer[potIndex] = analogRead(potPin);
  potIndex++;
  if (potIndex >= FILTER_SIZE) {
    potIndex = 0;
    bufferFilled = true;
  }

  long sum = 0;
  int samples = bufferFilled ? FILTER_SIZE : potIndex;
  for (int i = 0; i < samples; i++) sum += potBuffer[i];
  return sum / samples;
}

// --- Pętla główna ---
void loop() {
  // --- MIDI THRU ---
  if (MIDI.read()) {
    byte type = MIDI.getType();
    byte chan = MIDI.getChannel();
    byte d1 = MIDI.getData1();
    byte d2 = MIDI.getData2();
    MIDI.send(type, d1, d2, chan);
  }

  // --- Odczyt potencjometru co kilka ms ---
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;

    int potValue = readFilteredPot();

    if (abs(potValue - lastPotValue) > DEADZONE) {
      lastPotValue = potValue;

      int index = map(potValue, 0, 1023, 0, notesCount);
      if (index < 0) index = 0;
      if (index > notesCount) index = notesCount;

      // --- Obsługa nut ---
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

      // --- Pozycja 0 — wyłącz wszystko ---
      if (index == 0 && lastIndex != 0) {
        for (int i = 0; i < notesCount; i++) {
          MIDI.sendNoteOff(notes[i], 0, MIDI_CHANNEL);
        }
        lastIndex = 0;
      }

      // --- LED wskaźnik ---
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

