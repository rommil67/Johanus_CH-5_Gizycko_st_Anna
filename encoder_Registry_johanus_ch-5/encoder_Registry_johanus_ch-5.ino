/**
 * for ATmega328p or Arduino uno, nano, pro mini
 * Autor: Ro. Milew
 *
 * The third processor for registers converted Yamaha E-10AR to a MIDI console
 * @brief   This example demonstrates the use of push buttons that send note 
 *          events.
 * 
 * ### Connections
 * 
 *  - 2:  momentary push button (to ground)
 *  - 3:  momentary push button (to ground)
 * 
 * The internal pull-up resistors for the buttons will be enabled automatically.
 * 
 * ### Behavior
 * 
 * Pressing the first button will send note on/off events on middle C.
 * Pressing the second button will send note on/off events for middle C sharp.
 * 
 * Written by PieterP, 28-01-2019  
 * https://github.com/tttapa/Control-Surface
 */

// Include the Control Surface library
#include <Control_Surface.h>

// Instantiate a MIDI over USB interface.
//USBDebugMIDI_Interface midi;
//HardwareSerialMIDI_Interface midi;
//HardwareSerialMIDI_Interface midi = Serial;
HardwareSerialMIDI_Interface midiser = Serial;
//HairlessMIDI_Interface midi;

MIDI_PipeFactory<1> pipes;

using namespace MIDI_Notes;

// Instantiate the momentary push buttons that send out MIDI Note events.
NoteButton buttons[] = {
    //{2, note(C, 4)}, // digital input pin, note number
    //{3, note(Db, 4)},
    
    // If you want to set the channel specifically, you can use:
    // for UNO , nano
    // pierwsza listwa goldpin (dół płytki) 1x24 (12+12)
    {A1, {note(C, 2), CHANNEL_5}},
    {A3, {note(Db, 2), CHANNEL_5}},
    {A5, {note(D, 2), CHANNEL_5}},
    {A7, {note(Eb, 2), CHANNEL_5}},
    {A9, {note(E, 2), CHANNEL_5}},
    {A11, {note(F_, 2), CHANNEL_5}},
    {A13, {note(Gb, 2), CHANNEL_5}},
    {A15, {note(G, 2), CHANNEL_5}},
    {33, {note(Ab, 2), CHANNEL_5}},
    {35, {note(A, 2), CHANNEL_5}},
    {37, {note(Bb, 2), CHANNEL_5}},
    {39, {note(B, 2), CHANNEL_5}},
    {41, {note(C, 3), CHANNEL_5}},
    {43, {note(Db, 3), CHANNEL_5}},
    {45, {note(D, 3), CHANNEL_5}},
    {47, {note(Eb, 3), CHANNEL_5}},
    {17, {note(E, 3), CHANNEL_5}},
    {19, {note(F_, 3), CHANNEL_5}},
    {21, {note(Gb, 3), CHANNEL_5}},
    {23, {note(G, 3), CHANNEL_5}},
    {25, {note(Ab, 3), CHANNEL_5}},
    {27, {note(A, 3), CHANNEL_5}},
    {29, {note(Bb, 3), CHANNEL_5}},
    {31, {note(B, 3), CHANNEL_5}},
    // druga listwa goldpin (góra) 1x24 (12+12)
    {A0, {note(C, 4), CHANNEL_5}},
    {A2, {note(Db, 4), CHANNEL_5}},
    {A4, {note(D, 4), CHANNEL_5}},
    {A6, {note(Eb, 4), CHANNEL_5}},
    {A8, {note(E, 4), CHANNEL_5}},
    {A10, {note(F_, 4), CHANNEL_5}},
    {A12, {note(Gb, 4), CHANNEL_5}},
    {A14, {note(G, 4), CHANNEL_5}},
    {32, {note(Ab, 4), CHANNEL_5}},
    {34, {note(A, 4), CHANNEL_5}},
    {36, {note(Bb, 4), CHANNEL_5}},
    {38, {note(B, 4), CHANNEL_5}},
    {40, {note(C, 5), CHANNEL_5}},
    {44, {note(Db, 5), CHANNEL_5}},
    {46, {note(D, 5), CHANNEL_5}},
    {48, {note(Eb, 5), CHANNEL_5}},
    {2, {note(E, 5), CHANNEL_5}},
    {3, {note(F_, 5), CHANNEL_5}},
    {4, {note(Gb, 5), CHANNEL_5}},
    {5, {note(G, 5), CHANNEL_5}},
    {6, {note(Ab, 5), CHANNEL_5}},
    {7, {note(A, 5), CHANNEL_5}},
    {9, {note(Bb, 5), CHANNEL_5}},
    {11, {note(B, 5), CHANNEL_5}},
    //trzecia listwa goldpin (dół) 1 x 12
    {8, {note(C, 6), CHANNEL_5}},
    {10, {note(Db, 6), CHANNEL_5}},
    {12, {note(D, 6), CHANNEL_5}},
    {14, {note(Eb, 6), CHANNEL_5}},
    {16, {note(E, 6), CHANNEL_5}},
    {18, {note(F_, 6), CHANNEL_5}},
    {20, {note(Gb, 6), CHANNEL_5}},
    {22, {note(G, 6), CHANNEL_5}},
    {24, {note(Ab, 6), CHANNEL_5}},
    {26, {note(A, 6), CHANNEL_5}},
    {28,  {note(Bb, 6), CHANNEL_5}},
    {30,  {note(B, 6), CHANNEL_5}},
    //{46,  {note(C, 7), CHANNEL_5}},
  
    
};

void setup() {
    // Initialize everything
    Control_Surface.begin();
    midiser >> pipes >> midiser; //all incoming midi from Serial is looped back
     midiser.begin();
}

void loop() {
    // Update the control surface
    Control_Surface.loop();
    midiser.update();
}