/****************************************************************
 * FILE NAME: minimoog.c                                        *
 *                                                              *
 * PURPOSE: Emulation of RealTime Additive Synthesizer          *
 *                                                              *
 * DEPENDENCIES:    Allegro5 for GUI                            *
 *                  Pthread for multithreading programming      *
 * Author: Astro                                                *
 ***************************************************************/

#include "minimoog.h"



int main(void) {
	pthread_mutexattr_init(&rsrc_attr);
	pthread_mutexattr_settype(&rsrc_attr, PTHREAD_MUTEX_ERRORCHECK);
	
	pthread_barrier_init(&rsrc_barr, NULL, 4);
	
	pthread_mutex_init(&Srsrc_mutx,  &rsrc_attr);
	pthread_mutex_init(&Trsrc_mutx,  &rsrc_attr);
	pthread_mutex_init(&Rrsrc_mutx,  &rsrc_attr);
	pthread_mutex_init(&UIrsrc_mutx, &rsrc_attr);

	init();

	srand(time(NULL));

	StartTasks();
	
	terminate();    
	return 0;
}



void init(void) {
 
	if (!al_init()) {
		abort_example("Could not init Allegro.\n");
	}

	if(!al_install_keyboard()) {
		abort_example("Could not inistall Keyboard.\n");
	}    

	if (!al_install_audio()) {
		abort_example("Could not init sound!\n");
	}
	
	//al_set_config_value(al_get_system_config(),"audio","driver","alsa");
	
	if (!al_reserve_samples(0)){
		abort_example("Could not set up voice and mixer.\n");
	}

	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
  
}


void StartTasks(void) {

	/*Init Mutexes to protect those set of jobs
	 *[Reader @audio_thread] &[ Writers @sine_src, @triangle_src, @square_src]
	 *[Writer @audio_thread] &[ Reader @allegro_draw]
	 */

	/*Tasks Configuration have been defined in @tasks_configs.h*/
	task_create(sine_src,			SIN_ID,		SIN_PER,	SIN_DL,		SIN_PRIO);
	task_create(square_src,			SQU_ID,		SQU_PER,	SQU_DL,		SQU_PRIO); 
	task_create(triangle_src,		TRI_ID,		TRI_PER,	TRI_DL,		TRI_PRIO);  
	task_create(audio_thread,		AUDIO_ID,	AUDIO_PER,	AUDIO_DL,	AUDIO_PRIO); 
	task_create(allegro_draw,		UI_ID,		UI_PER,		UI_DL,		UI_PRIO);
	task_create(keyboard_thread,	KEY_ID,		KEY_PER,	KEY_DL,		KEY_PER);  

}



void terminate(void){
	
	for (int i = 0; i < NUM_TASKS; ++i){
		pthread_join(tid[i],NULL);
	}

	pthread_mutex_destroy(&Trsrc_mutx);
	pthread_mutex_destroy(&Rrsrc_mutx);
	pthread_mutex_destroy(&Srsrc_mutx);

	al_uninstall_audio();  
  
}

