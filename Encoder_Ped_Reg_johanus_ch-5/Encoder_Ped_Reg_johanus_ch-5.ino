#include <Control_Surface.h>

// --------------------------- MIDI interface ---------------------------
HardwareSerialMIDI_Interface midi { Serial }; 

// MIDI Thru (echo wszystkich komunikatów RX→TX)
struct EchoCallbacks : MIDI_Callbacks {
  void onChannelMessage(MIDI_Interface &iface, ChannelMessage msg) override { iface.send(msg); }
  void onSysExMessage(MIDI_Interface &iface, SysExMessage msg) override { iface.send(msg); }
  void onSysCommonMessage(MIDI_Interface &iface, SysCommonMessage msg) override { iface.send(msg); }
  void onRealTimeMessage(MIDI_Interface &iface, RealTimeMessage msg) override { iface.send(msg); }
} echoCallbacks;

/// --------------------------- Pedal matrix (8x4) ---------------------------
// Kanał 4, nuty od 36
const AddressMatrix<8, 4> noteAddresses1 = {{
  {36,  44,  52,  60},
  {37,  45,  53,  61},
  {38,  46,  54,  62},
  {39,  47,  55,  63},
  {40,  48,  56,  64},
  {41,  49,  57,  65},
  {42,  50,  58,  66},
  {43,  51,  59,  67}
}};

// ✅ poprawiona składnia tworzenia macierzy
NoteButtonMatrix<8, 4> matrix = {
  {17, 19, 21, 23, 25, 27, 29, 31},  // 8 wierszy
  {16, 18, 20, 22},                  // 4 kolumny
  noteAddresses1,
  CHANNEL_4
};

// --------------------------- Manual matrix (8x3) ---------------------------
// Kanał 5, nuty od 100
const uint8_t manual_rows[] = {32, 34, 36, 38, 40, 42, 44, 46}; // 8 wierszy
const uint8_t manual_cols[] = {A0, A4, A6};                     // 3 kolumny

const AddressMatrix<8, 3> manual_addresses = {{
  {100, 101, 102},
  {103, 104, 105},
  {106, 107, 108},
  {109, 110, 111},
  {112, 113, 114},
  {115, 116, 117},
  {118, 119, 120},
  {121, 122, 123}
}};

NoteButtonMatrix<8, 3> manualMatrix = {
  { manual_rows[0], manual_rows[1], manual_rows[2], manual_rows[3],
    manual_rows[4], manual_rows[5], manual_rows[6], manual_rows[7] },
  { manual_cols[0], manual_cols[1], manual_cols[2] },
  manual_addresses,
  Channel_5
};

// --------------------------- Note buttons (4x) ---------------------------
const uint8_t notePins[4] = {2, 4, 6, 8};
NoteButton noteBtns[] = {
  { notePins[0], {61, Channel_5} },
  { notePins[1], {62, Channel_5} },
  { notePins[2], {63, Channel_5} },
  { notePins[3], {64, Channel_5} }
};

// --------------------------- Potentiometers (4x) ---------------------------//
CCPotentiometer pot1 { A1, {20, Channel_8} };
//CCPotentiometer pot2 { A3, {21, Channel_9} };
//CCPotentiometer pot3 { A5, {22, Channel_10} };
//CCPotentiometer pot4 { A7, {23, Channel_11} };

// --------------------------- LED indicator for A1 ---------------------------
const uint8_t ledPins[] = {A15, 33, 35, 37, 39};
const uint8_t ledCount = sizeof(ledPins) / sizeof(ledPins[0]);

void setup() {
//  Serial.begin(115200);
  midi.begin();
  midi.setCallbacks(echoCallbacks);
  Control_Surface.begin();

  // LED setup
  for (uint8_t i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

 // Serial.println(F("MIDI controller ready on Serial3 (TX3=D14, RX3=D15)"));
}

void loop() {
  // --- MIDI i wszystkie kontrolery (matryce, potencjometry itd.) ---
  Control_Surface.loop();

  // --- Pasek LED - wizualizacja potencjometru A1 bez delay() ---
  static uint32_t lastUpdate = 0;
  const uint16_t updateInterval = 10; // co 10 ms odświeżanie LED

  uint32_t now = millis();
  if (now - lastUpdate >= updateInterval) {
    lastUpdate = now;

    int val = analogRead(A1); // 0–1023
    uint8_t level = map(val, 0, 1023, 0, ledCount);
    if (level < 1) level = 1; // zawsze przynajmniej 1 LED zapalona

    for (uint8_t i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], (i < level) ? LOW : HIGH);
    }
  }
}


