/*-------------------------------------------------------------
 * Allegro5 Based Libraries 
 * ------------------------------------------------------------
 */ 

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>

/*-------------------------------------------------------------
 * Application-Defined Libraries 
 * ------------------------------------------------------------
 */ 
#include "shared.h"
#include "common.c"
#include "ptask.h"
#include "keyboard.h"
#include "ui.h"
#include "signal_src.h"


/*-------------------------------------------------------------
 * GLOBAL MACROS
 * for screen and coordinates
 * ------------------------------------------------------------
 */ 

#define LOG_ENABLE            		  0		//For showing Debugging Information in run rime
#define XWIN 				          800	// window x resolution
#define YWIN				          600	// window y resolution

/*-------------------------------------------------------------
 * FUNTION PROTOTYPES
 * ------------------------------------------------------------
 */ 

void init(void);
void terminate(void);
void StartTasks(void);
void loading(void);



extern struct waves_buffer signal_params;
extern enum Waveform wave;
