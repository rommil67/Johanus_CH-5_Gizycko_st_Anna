#include <Control_Surface.h>

// MIDI over USB (choisis l'interface adaptée à ta carte)
USBMIDI_Interface midi;

// Programmeur : liste 0..127 (General MIDI numbering 0..127)
ProgramChanger<128> programChanger = {
  {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
  },
  CHANNEL_1 // canal MIDI (ou Channel_1 selon ta version; CHANNEL_1 fonctionne dans les exemples officiels)
};

// EncoderSelector : l'encodeur contrôle la sélection du ProgramChanger
// pins {2, 3} -> A et B de l'encodeur ; 4 pulses per step, clamp (pas de wrap)
EncoderSelector<128> encoderSelector {
  programChanger,     // <-- IMPORTANT : on passe la référence au ProgramChanger
  {2, 3},             // pins A,B de l'encodeur
  4,                  // pulses par pas (ajuste selon ton encodeur)
  Wrap::Wrap        // clamp (ou Wrap::Wrap si tu veux revenir à 0 après 127)
};

void setup() {
  Control_Surface.begin(); // initialise la librairie (call begin sur tous les éléments)
}

void loop() {
  Control_Surface.loop(); // actualise la librairie
}
