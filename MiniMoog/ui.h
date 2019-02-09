#ifndef UI_H
#define UI_H


#include "shared.h"
#include "signal_src.h"


#define XDISP			800
#define YDISP			600
#define SINE_BLOCK_S	10  //start coordinates of plotting the sine waveform
#define SINE_BLOCK_E	300 //end coordinates of plotting the sine waveform
#define TRI_BLOCK_S		310 //start coordinates of plotting the Triangle waveform
#define TRI_BLOCK_E		600	//end coordinates of plotting the Triangle waveform	 
#define SQUARE_BLOCK_S	610	//start coordinates of plotting the Square waveform
#define SQUARE_BLOCK_E	900 //end coordinates of plotting the Square waveform
#define SUM_BLOCK_S		190	//start coordinates of plotting the Square waveform
#define SUM_BLOCK_E		630 //end coordinates of plotting the Square waveform
#define THICK			2
#define BLACK           0, 0, 0
#define WHITE           255, 255, 255
#define PINC			255, 153, 204 
#define ELECTRIC        0.3, 0.2, 0.6


ALLEGRO_FONT*		font;
ALLEGRO_DISPLAY*	display;

ALLEGRO_COLOR c0; 
ALLEGRO_COLOR c1;  
ALLEGRO_COLOR c2;  
ALLEGRO_COLOR c3;

extern struct waves_buffer signal_params;


void DrawTask_Info(void);
void Draw_Task_Bounded_Boxes(void);
void PlotWaveforms(void);
void Draw_Instructions(void);
 
TASK allegro_draw(void* arg);


#endif
