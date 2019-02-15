#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>



#define SAMPLES_PER_BUFFER			1024
#define STREAM_FREQUENCY			44100
#define PI                    		ALLEGRO_PI
#define TWOPI               		(2.0 * PI)

float*	buf;

typedef void* TASK;

struct tasks_param{

	char id_audio;
	char prio_audio;
	char per_audio;
	char dlmiss_audio;

	char id_keyboard;
	char prio_keyboard;
	char per_keyboard;
	char dlmiss_keyboard;
	
	char id_display;
	char prio_display;
	char per_display;
	char dlmiss_display;

	char id_sinWave;
	char prio_sinWave;
	char per_sinWave;
	char dlmiss_sinWave;
	
	char id_squWave;
	char prio_squWave;
	char per_squWave;
	char dlmiss_squWave;
	
	char id_triWave;
	char prio_triWave;
	char per_triWave;
	char dlmiss_triWave;	

}task_sepcs;


#endif
