#include <MIDI.h>

// --------------------------- MIDI setup ---------------------------
MIDI_CREATE_DEFAULT_INSTANCE();

// --------------------------- Tablica translacji ---------------------------
// Note number → Program Change number (wszystkie elementy inicjalizujemy 0)
int8_t noteToProgram[128] = {0};

// Kanały MIDI
constexpr uint8_t MONITOR_CHANNEL = 5;   // Odbiór NoteOn/Off
constexpr uint8_t OUTPUT_CHANNEL  = 12;  // Wyjście ProgramChange

// --------------------------- Callbacki ---------------------------
void handleNoteOn(byte channel, byte note, byte velocity) {
  // MIDI Thru (wszystko dalej)
  MIDI.sendNoteOn(note, velocity, channel);

  // Dodatkowo, jeśli kanał 5:
  if (channel == MONITOR_CHANNEL) {
    int8_t program = noteToProgram[note];
    if (program >= 0) {
      MIDI.sendProgramChange(program, OUTPUT_CHANNEL);
    }
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  // MIDI Thru
  MIDI.sendNoteOff(note, velocity, channel);

  // Dodatkowo, jeśli kanał 5:
  if (channel == MONITOR_CHANNEL) {
    int8_t program = noteToProgram[note];
    if (program >= 0) {
      MIDI.sendProgramChange(program, OUTPUT_CHANNEL);
    }
  }
}

// --------------------------- Setup ---------------------------
void setup() {
  // --------------------------- Ręczne przypisania tablicy ---------------------------
  noteToProgram[70] = 0;  noteToProgram[71] = 1;  noteToProgram[68] = 2;  noteToProgram[69] = 3;
  noteToProgram[66] = 4;  noteToProgram[67] = 5;  noteToProgram[65] = 6;  noteToProgram[64] = 7;
  noteToProgram[63] = 8;  noteToProgram[62] = 9;  noteToProgram[61] = 10; noteToProgram[60] = 11;

  noteToProgram[36] = 12; noteToProgram[38] = 13; noteToProgram[37] = 14; noteToProgram[40] = 15;
  noteToProgram[39] = 16; noteToProgram[42] = 17; noteToProgram[41] = 18; noteToProgram[43] = 19;
  noteToProgram[45] = 20; noteToProgram[44] = 21;

  noteToProgram[46] = 22; noteToProgram[47] = 23; noteToProgram[49] = 24; noteToProgram[48] = 25;
  noteToProgram[50] = 26; noteToProgram[51] = 27; noteToProgram[58] = 28; noteToProgram[52] = 29;
  noteToProgram[54] = 30; noteToProgram[59] = 31; noteToProgram[53] = 32; noteToProgram[55] = 33;
  noteToProgram[57] = 34; noteToProgram[56] = 35;

  noteToProgram[95] = 36; noteToProgram[91] = 37; noteToProgram[93] = 38; noteToProgram[94] = 39;
  noteToProgram[89] = 40; noteToProgram[92] = 41; noteToProgram[90] = 42; noteToProgram[88] = 43;
  noteToProgram[82] = 44; noteToProgram[86] = 45; noteToProgram[87] = 46; noteToProgram[81] = 47;
  noteToProgram[85] = 48; noteToProgram[84] = 49; noteToProgram[83] = 50; noteToProgram[80] = 51;

  noteToProgram[79] = 52; noteToProgram[78] = 53; noteToProgram[77] = 54; noteToProgram[76] = 55;
  noteToProgram[74] = 56; noteToProgram[73] = 57; noteToProgram[62] = 58; noteToProgram[61] = 59; 
  noteToProgram[72] = 60;

  // --------------------------- Inicjalizacja MIDI ---------------------------
  MIDI.begin(MIDI_CHANNEL_OMNI);  // Odbieraj wszystkie kanały
  Serial.begin(31250);            // standardowe MIDI baudrate

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
}

// --------------------------- Loop ---------------------------
void loop() {
  // Odbiór i przekazanie dalej wszystkich komunikatów
  if (MIDI.read()) {
    // Automatyczne echo (Thru)
    midi::MidiType type = MIDI.getType();

    // Thru dla wszystkiego poza NoteOn/Off (bo te już wysłaliśmy ręcznie)
    if (type != midi::NoteOn && type != midi::NoteOff) {
      byte channel = MIDI.getChannel();
      byte data1   = MIDI.getData1();
      byte data2   = MIDI.getData2();
      MIDI.send(type, data1, data2, channel);
    }
  }
}
