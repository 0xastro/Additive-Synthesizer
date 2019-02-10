#ifndef KEYBOARD
#define KEYBOARD

//#include "shared.h"
#include "signal_src.h"
#include "ptask.h" 



//Macros for Synthesiser Notes (Notes 4)
#define NOTE_A		440.00
#define NOTE_B		493.88
#define NOTE_C		261.63
#define NOTE_D		293.66
#define NOTE_E		329.63
#define NOTE_F		349.23
#define NOTE_G		392.00


ALLEGRO_KEYBOARD_STATE keyState;


void log_key(char const *how, int keycode, int unichar, int modifiers);
void sin_notes(void);
void square_notes(void);
void triangle_notes(void);
void WavesON_OFF(void);

TASK keyboard_thread(void* key);



#endif
