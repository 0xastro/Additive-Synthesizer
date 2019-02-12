
#include "ptask.h"
#include "signal_src.h"


double  dt = 1.0 / 44100;   //sampling time: 1/48 msec
float   waveform1[SAMPLES_PER_BUFFER],
		waveform2[SAMPLES_PER_BUFFER],
		waveform3[SAMPLES_PER_BUFFER];
long    xxx;


/*//////////////////////FIR_BPF///////////////////////////////
 *
 * BandPass Filter function Compute the filter coeff for a 
 * certain band. It Takes the following inputs:
 * @fc_L:   low-cutoff frequency
 * @fc_H:   high-cutoff frequency
 * @FS:     Sampling frequency usually 48KKHz or 44.1KHz
 * @N:      Number of Tabs
 * The output is the filter coefficient which is stored in
 * h array 
 *////////////////////////////////////////////////////////////

void FIR_BPF(float fc_L, float fc_H, float FS, int N) {
	
	float   hi,
			hl;
	int icount;
	
	for (icount = -(N-1)/2; icount <= (N-1)/2; icount++)
	{
		if(icount==0)
			BPF.h[icount+(N-1)/2]=2*(fc_H-fc_L)/FS;
		else {
			hi= (sin(TWOPI*(fc_H/FS)*icount)/(PI*icount));
			hl= (sin(TWOPI*(fc_L/FS)*icount)/(PI*icount));            
			BPF.h[icount+(N-1)/2]=(hi-hl)*(0.54+0.46*cos(TWOPI*icount/N));
		}
	}

	if(LOG_ENABLE) {
	for(icount=0; icount <SAMPLES_PER_BUFFER; icount++)
		log_printf("h[%i]=%e\n",icount,BPF.h[icount]);
	}
}


/*//////////////////////additive_synths///////////////////////
 *
 * This function Integrates the buffers of the generated Waveforms
 * [sineWave_buf, squareWave_buf and triangleWave_buf]
 * into the @additive_buf array to be played by the audio card
 *
 *////////////////////////////////////////////////////////////

void additive_synths(float *buf) {
	
	int i;

	for (i = 0; i < SAMPLES_PER_BUFFER; ++i){
		buf[i] = waveform1[i]+waveform2[i]+waveform3[i]; 
	}
	
	/* @signal_params.sum[i] is filled and used only for Drawing, 
	 * where our square wave is an ideal one without any transition 
	 * (falling/rising edges) so drawing it doesn't make any sense
	 * so we only draw sin+tri on the screen
	 */

	pthread_mutex_lock(&UIrsrc_mutx);
	for (i = 0; i < SAMPLES_PER_BUFFER; ++i){
		signal_params.sum[i] = waveform1[i]+waveform2[i];
	} 
	pthread_mutex_unlock(&UIrsrc_mutx);

}

/*//////////////////////audio_thread///////////////////////
 * TASK audio_thread:  
 *  >Setup Allegro Audio Setting
 *  >Perpare the output stream to be executed by calling 
 *   the [additive_synths] function under condional variable
 *   and mutex protection
 *  > And Finally, play the sound
 *
 * * P.S: Here I used 3 conditional variables, but I Think it 
 *      needs an improvement bu using only one conditional
 *      variable. I tried to do so, but the sound was poorly
 *      played  
 *////////////////////////////////////////////////////////////

TASK audio_thread(TASK arg){

	task_sepcs.id_audio=get_task_index(arg);
	task_sepcs.prio_audio=get_task_period(arg);
	
	int Audio_FS=44100;
	int fragmentCount=2;

	ALLEGRO_EVENT_QUEUE     *queue;
	ALLEGRO_AUDIO_STREAM    *Output_Stream;
	ALLEGRO_MIXER           *mixer; 

	mixer			= al_get_default_mixer();
	Output_Stream	= al_create_audio_stream(fragmentCount, SAMPLES_PER_BUFFER, Audio_FS, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_1); 
	queue			= al_create_event_queue();

	al_attach_audio_stream_to_mixer(Output_Stream, mixer);
	al_register_event_source(queue, al_get_audio_stream_event_source(Output_Stream));

	set_activation(task_sepcs.id_audio); //this is wrong!!!
	
	do{

		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		
		/*Enter the critical Section, reading @sineWave_buf*/
		pthread_mutex_lock(&Srsrc_mutx);       		
		/*Do the work.....
		*Perform Reading from the shared buffer*/ 
		
		memmove(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		
		//Checking if both arrays are identical, If not recopy again
		while (memcmp(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0){
			memmove(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}
		
		/*End the critical Section*/
		pthread_mutex_unlock(&Srsrc_mutx);
		

		pthread_mutex_lock(&Trsrc_mutx);
		memmove(waveform2, signal_params.triangleWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		//Checking if arrays are identical, If not recopy again
		while (memcmp(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0){
			memmove(waveform2, signal_params.triangleWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}
		pthread_mutex_unlock(&Trsrc_mutx);
			
		pthread_mutex_lock(&Rrsrc_mutx);
		memmove(waveform3, signal_params.squareWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		//Checking if arrays are identical, If not recopy again
		while (memcmp(waveform3, signal_params.squareWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0){
			memmove(waveform3, signal_params.squareWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}
		pthread_mutex_unlock(&Rrsrc_mutx);
			
		
		if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT ) {

			additive_buf = al_get_audio_stream_fragment(Output_Stream);                
			/* where @al_set_audio_stream_fragment needs to be called iff 
			 * after each successful call of @al_get_audio_stream_fragment
			 */
			if (!additive_buf) {
				continue;
			}
			
			additive_synths((float*)additive_buf);
				
			if(!al_set_audio_stream_fragment(Output_Stream, additive_buf)){
				log_printf("Error setting stream fragment.\n");
			}

		}

		wait_for_period(task_sepcs.id_audio);
		pthread_barrier_wait(&rsrc_barr);
	
	}while(true);



	/*Killing*/
	al_drain_audio_stream(Output_Stream);
	al_destroy_event_queue(queue);  
	al_destroy_mixer(mixer);
	pthread_exit(NULL);

}



/*//////////////////////sine_src///////////////////////
 * TASK sine_src:  
 *      the task perform the the generation of sine
 *      signal based on the user defined frequency
 *      where the signal is generated using the standard
 *      [ sin() ] function called from math.h lib
 *
 *//////////////////////////////////////////////////////

 TASK sine_src(TASK arg) {
	double  w, ti;
	int     i; 

	task_sepcs.id_sinWave=get_task_index(arg);
	task_sepcs.prio_sinWave=get_task_period(arg);
	set_activation(task_sepcs.id_sinWave);
	
	do{

		w = TWOPI * signal_params.freq_sin;
		t_sine+= dt * SAMPLES_PER_BUFFER;

		//Enter the critical Section
		while (pthread_mutex_trylock(&Srsrc_mutx) == 0){              
		/*Do the work.....
		 *Perform Writing to the buffer*/
		for (i = 0; i < SAMPLES_PER_BUFFER; ++i) { 
			ti = t_sine + i * dt;
			signal_params.sineWave_buf[i] =sin(w * ti);
		}
		pthread_mutex_unlock(&Srsrc_mutx);
		break;
		}
		//Exit the critical Section

		
		wait_for_period(task_sepcs.id_sinWave);
		pthread_barrier_wait(&rsrc_barr);


	}while(true);
	pthread_exit(NULL);
}




/*//////////////////////triangle_src///////////////////////
 * TASK triangle_src:  
 *      the task perform the the generation of triangle  
 *      wave signal based on the user defined frequency
 *      from the standard function [ sin() ]
 *//////////////////////////////////////////////////////
TASK triangle_src(TASK arg) {
	double w, tx, tu;
	unsigned i;
	task_sepcs.id_triWave=get_task_index(arg);
	task_sepcs.prio_triWave=get_task_period(arg);
	
	set_activation(task_sepcs.id_triWave);
   
	do {
		pthread_mutex_lock(&Trsrc_mutx);
		w = TWOPI * signal_params.freq_tri;
		t_triangle+= dt * SAMPLES_PER_BUFFER;

		for (i = 0; i < SAMPLES_PER_BUFFER; i++) {
			tx = w * (t_triangle + i * dt) + PI/2.0;
			tu = fmod(tx/PI, 2.0);
			if (tu <= 1.0)
				signal_params.triangleWave_buf[i] =  signal_params.tri_switch*(1.0 - 2.0 * tu);
			else
				signal_params.triangleWave_buf[i] =  signal_params.tri_switch*(-1.0 + 2.0 * (tu - 1.0));
		}
		pthread_mutex_unlock(&Trsrc_mutx);

		wait_for_period(task_sepcs.id_triWave);
		pthread_barrier_wait(&rsrc_barr);

	}while(true);
	pthread_exit(NULL);
}




TASK square_src(TASK arg) {
    
    double w, x, ti;
    unsigned i;
    
    task_sepcs.id_squWave=get_task_index(arg);
    task_sepcs.prio_squWave=get_task_period(arg);
    set_activation(task_sepcs.id_squWave);    

	do {
		pthread_mutex_lock(&Rrsrc_mutx);

		w = TWOPI * signal_params.freq_square;
        t_square+= dt * SAMPLES_PER_BUFFER;  

		for (i = 0; i < SAMPLES_PER_BUFFER; i++) {
            
            ti = t_square + i * dt;
            x = sin(w * ti);
            signal_params.squareWave_buf[i] = (x >= 0.0) ? (signal_params.square_switch*1.0) : (signal_params.square_switch* -1.0);
 		}
		pthread_mutex_unlock(&Rrsrc_mutx);

		wait_for_period(task_sepcs.id_squWave);
		pthread_barrier_wait(&rsrc_barr);

	}while(true);
	pthread_exit(NULL);
}

