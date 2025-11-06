#include <MIDI.h>

// Użycie portu sprzętowego Serial2 (TX2 = pin 16)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// Zakres nut (61 nut = 36–96)
const byte noteStart = 36;
const byte noteCount = 61;
const byte totalChannels = 16;
const byte ccMax = 120;

const unsigned long interval = 300; // odstęp między komunikatami [ms]

void setup() {
  Serial.begin(115200);
  MIDI.begin(MIDI_CHANNEL_OFF); // tylko wysyłanie
  Serial.println("=== JOHANNUS MIDI TESTER ===");
  Serial.println("Włączanie 61 nut na 16 kanałach...");

  // --- Włącz wszystkie nuty na 16 kanałach ---
  for (byte ch = 1; ch <= totalChannels; ch++) {
    for (byte i = 0; i < noteCount; i++) {
      byte note = noteStart + i;
      MIDI.sendNoteOn(note, 100, ch);
      delay(1);
    }
    Serial.print("Kanał ");
    Serial.print(ch);
    Serial.println(" -> wszystkie nuty ON");
  }

  Serial.println("Gotowe! Rozpoczynam sekwencję CC...");
  delay(2000);
}

void loop() {
  static byte ccNumber = 1;
  static unsigned long lastTime = 0;

  unsigned long now = millis();
  if (now - lastTime >= interval) {
    lastTime = now;

    Serial.print("Wysyłam CC#");
    Serial.print(ccNumber);
    Serial.print(" = 127 na wszystkich kanałach... ");

    // Wysyłaj ten sam CC# na wszystkich 16 kanałach
    for (byte ch = 1; ch <= totalChannels; ch++) {
      MIDI.sendControlChange(ccNumber, 127, ch);
      delay(2); // minimalna przerwa, aby uniknąć przeciążenia
    }

    Serial.println("OK");

    // Przejdź do następnego CC
    ccNumber++;
    if (ccNumber > ccMax) {
      ccNumber = 1; // restart po CC#120
      Serial.println("=== Powrót do CC#1 ===");
      delay(1000);
    }
  }
}
