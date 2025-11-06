/*
 * Trzy matryce 8x8 (192 przyciski) na Arduino Mega 2560
 * Każda ma oddzielne przypisania nut (36–99),
 * w ostatnim rzędzie 4 pierwsze nuty = 105–108
 * Kanały: 1, 2, 3
 * MIDI przez Serial (DIN / Hairless MIDI)
 * Biblioteka: Control Surface (https://github.com/tttapa/Control-Surface)
 */

#include <Control_Surface.h>

// --- Interfejs MIDI (Serial) ---
HardwareSerialMIDI_Interface midiser = Serial;
MIDI_PipeFactory<1> pipes;

// --- Potencjometry (Crescendo) ---
//CCPotentiometer pot1 { A0, {MIDI_CC::Channel_Volume, CHANNEL_11} };       // CC7 Volume
//CCPotentiometer pot2 { A1, {MIDI_CC::Channel_Volume, CHANNEL_12} };       // CC7 Volume
//CCPotentiometer pot2 { A1, {MIDI_CC::Expression_Controller, CHANNEL_1} }; // CC11 Expres
// CCPotentiometer potentiometer1(A0, {MIDI_CC::Channel_Volume, CHANNEL_16});
// CCPotentiometer potentiometer2(A1, {MIDI_CC::Channel_Volume, CHANNEL_15});
/*
// --- Matryca 1 (A0–A15, kanał 1) ---
const AddressMatrix<8, 8> noteAddresses1 = {{
  {36, 44, 52, 60, 68, 76, 84, 92},
  {37, 45, 53, 61, 69, 77, 85, 93},
  {38, 46, 54, 62, 70, 78, 86, 94},
  {39, 47, 55, 63, 71, 79, 87, 95},
  {40, 48, 56, 64, 72, 80, 88, 96},
  {41, 49, 57, 65, 73, 81, 89, 97},
  {42, 50, 58, 66, 74, 82, 90, 98},
  {43, 51, 59, 67, 75, 83, 91, 99}, // zmienione
}};
NoteButtonMatrix<8, 8> matrix1 = {
  {A14, A12, A10, A8, A6, A4, A2, A0},       // wiersze
  {A15, A13, A11, A9, A7, A5, A3, A1}, // kolumny
  noteAddresses1,
  CHANNEL_3,
};
*/
// Piny wierszy i kolumn
constexpr uint8_t rows[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
constexpr uint8_t cols[8] = {A8, A9, A10, A11, A12, A13, A14, A15};

// Tablica numerów Program Change 0–63
constexpr uint8_t programNumbers[8][8] = {
  {0,1,2,3,4,5,6,7},
  {8,9,10,11,12,13,14,15},
  {16,17,18,19,20,21,22,23},
  {24,25,26,27,28,29,30,31},
  {32,33,34,35,36,37,38,39},
  {40,41,42,43,44,45,46,47},
  {48,49,50,51,52,53,54,55},
  {56,57,58,59,60,61,62,63}
};

// Tworzymy macierz przycisków
ButtonMatrix<8,8> buttonMatrix{rows, cols};

// Tablica ProgramChangeButton dla każdej pozycji
ProgramChangeButton pcButtons[8][8];
// --- Matryca 2 (32–47, kanał 2) ---
const AddressMatrix<8, 8> noteAddresses2 = {{
  {36, 44, 52, 60, 68, 76, 84, 92},
  {37, 45, 53, 61, 69, 77, 85, 93},
  {38, 46, 54, 62, 70, 78, 86, 94},
  {39, 47, 55, 63, 71, 79, 87, 95},
  {40, 48, 56, 64, 72, 80, 88, 96},
  {41, 49, 57, 65, 73, 81, 89, 97},
  {42, 50, 58, 66, 74, 82, 90, 98},
  {43, 51, 59, 67, 75, 83, 91, 99}, // zmienione
}};
NoteButtonMatrix<8, 8> matrix2 = {
  {32, 34, 36, 38, 40, 42, 44, 46}, // wiersze
  {33, 35, 37, 39, 41, 43, 45, 47}, // kolumny
  noteAddresses2,
  CHANNEL_2,
};

// --- Matryca 3 (14–31, kanał 3) ---
const AddressMatrix<8, 8> noteAddresses3 = {{
  {36, 44, 52, 60, 68, 76, 84, 92},
  {37, 45, 53, 61, 69, 77, 85, 93},
  {38, 46, 54, 62, 70, 78, 86, 94},
  {39, 47, 55, 63, 71, 79, 87, 95},
  {40, 48, 56, 64, 72, 80, 88, 96},
  {41, 49, 57, 65, 73, 81, 89, 97},
  {42, 50, 58, 66, 74, 82, 90, 98},
  {43, 51, 59, 67, 75, 83, 91, 99}, // zmienione
}};
NoteButtonMatrix<8, 8> matrix3 = {
  {30, 28, 26, 24, 22, 20, 18, 16}, // wiersze
  {31, 29, 27, 25, 23, 21, 19, 17}, // kolumny
  noteAddresses3,
  CHANNEL_1,
};

// --- Setup ---
void setup() {
  Control_Surface.begin();
  midiser >> pipes >> midiser; // MIDI IN -> OUT
  midiser.begin();
}

// --- Loop ---
void loop() {
  Control_Surface.loop();
  midiser.update();
}
