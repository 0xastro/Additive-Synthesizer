
#include "ptask.h"
#include "signal_src.h"
#include "task_configs.h"


float           waveform1[SAMPLES_PER_BUFFER],
                waveform2[SAMPLES_PER_BUFFER],
                waveform3[SAMPLES_PER_BUFFER];

/*sampling time: 1/48 msec, 44100 is the default sampling rate of Mixer*/
int 	Audio_FS = 48000;
double  dt 		 = 1.0 / 48000;

/*-----------------------------------------------------------
 *----------------------------------------------------FIR_BPF
 *-----------------------------------------------------------
 * BandPass Filter function Compute the filter coeff for a 
 * certain band. It Takes the following inputs:
 * @fc_L:   low-cutoff frequency
 * @fc_H:   high-cutoff frequency
 * @FS:     Sampling frequency usually 48KKHz or 44.1KHz
 * @N:      Number of Tabs
 * The output is the filter coefficient which is stored in
 * h array 

 NewFilter Design, however the old one was better but meaningless
  Cn= (sin(nPIv2)-sin(nPIv1))/nPI, Cn is the general T.F
  C0=1-v2-v1;
  Hint v is the ratio between the cufoff freq to the Nyquist freq 
  (Fc_H)/Fs
  FS >= 2Fc_H
 
 --------------------------------------------------------*/
void FIR_BPF(float fc_L, float fc_H, float FS, int N) {
	
	//float   hi, hl;
	int 	coef, coef2, /*odd,*/ icount;

	coef =N;	
	/*Normalize the cutoff frequency to values between 0 and 1*/
	fc_L = (fc_L)/(Audio_FS/2);
	fc_H = (fc_H)/(Audio_FS/2);
	FS 	 = TWOPI*FS;

	/*Determine whether there's an even or odd number of coeffecient*/
	coef2	= coef>>1;
	//odd		= coef - (coef2<<1);
	//set the symmetry
	BPF.h[coef-1/2] = fc_H-fc_L;
	for (icount=1; icount<coef2; icount++) {  
		BPF.h[coef2+icount] = BPF.h[coef2-icount] =(sin(fc_H*icount*PI)-sin(fc_L*icount*PI))/(icount*PI);
	}

	if(LOG_ENABLE) {
	for(icount=0; icount <SAMPLES_PER_BUFFER; icount++)
		printf("h[%i]=%e\n",icount,BPF.h[icount]);
	}
}
/*-----------------------------------------------------------
 *--------------------------------------------additive_synths
 *-----------------------------------------------------------
 *
 * This function Integrates the buffers of the generated Waveforms
 * [sineWave_buf, squareWave_buf and triangleWave_buf]
 * into the @additive_buf array to be played by the audio card
 *
 --------------------------------------------------------*/
void additive_synths(float *buf) {
	
	int i;
	
	for (i = 0; i < SAMPLES_PER_BUFFER; ++i){
		buf[i] = signal_params.gain* (2*waveform1[i]+2*waveform2[i]+waveform3[i]); 
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
/*-----------------------------------------------------------
 *----------------------------------------------audio_thread
 *-----------------------------------------------------------

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
 --------------------------------------------------------*/

TASK audio_thread(TASK arg) {
	
	int fragmentCount		= 2;
	ALLEGRO_EVENT_QUEUE     *queue;
	ALLEGRO_AUDIO_STREAM    *Output_Stream;
	ALLEGRO_MIXER           *mixer; 
	ALLEGRO_VOICE			*voice;
	queue 					= al_create_event_queue();
	Output_Stream			= al_create_audio_stream(fragmentCount, SAMPLES_PER_BUFFER, Audio_FS, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_1); 
	mixer 					= al_create_mixer(Audio_FS, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_1);
	voice 					= al_create_voice(Audio_FS, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
	
	al_attach_audio_stream_to_mixer(Output_Stream, mixer);
	al_attach_mixer_to_voice(mixer, voice);

	al_register_event_source(queue, al_get_audio_stream_event_source(Output_Stream));

	task_sepcs.id_audio=get_task_index(arg);
	task_sepcs.prio_audio=get_task_priority(arg);
	set_activation(task_sepcs.id_audio);
	
	while(!TASK_END) {
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);
		
		pthread_mutex_lock(&Srsrc_mutx);       		
		while (memcmp(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0) {
			memmove(waveform1, signal_params.sineWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}	
		pthread_mutex_unlock(&Srsrc_mutx);	
		/*---------------------------------------------------------------------------------------------------------*/
		pthread_mutex_lock(&Trsrc_mutx);
		while (memcmp(waveform2, signal_params.triangleWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0) {
			memmove(waveform2, signal_params.triangleWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}
		pthread_mutex_unlock(&Trsrc_mutx);			
		/*---------------------------------------------------------------------------------------------------------*/
		pthread_mutex_lock(&Rrsrc_mutx);
		while (memcmp(waveform3, signal_params.squareWave_buf, SAMPLES_PER_BUFFER * sizeof(float)) != 0) {
			memmove(waveform3, signal_params.squareWave_buf, SAMPLES_PER_BUFFER * sizeof(float));
		}
		pthread_mutex_unlock(&Rrsrc_mutx);
		/*---------------------------------------------------------------------------------------------------------*/
		
		if (event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT ) {
			additive_buf = al_get_audio_stream_fragment(Output_Stream);                
			/* where @al_set_audio_stream_fragment needs to be called iff 
			 * after each successful call of @al_get_audio_stream_fragment
			 */
			if (!additive_buf) {
				continue;
			}
			additive_synths((float*)additive_buf);			
			if(!al_set_audio_stream_fragment(Output_Stream, additive_buf)) {
				log_printf("Error setting stream fragment.\n");
			}
		}
		
		wait_for_period(task_sepcs.id_audio);
		if(BARRIER_ENABLE){
			pthread_barrier_wait(&rsrc_barr);
		}	
	}
	/*Cleaning*/
	al_drain_audio_stream(Output_Stream);
	al_destroy_event_queue(queue);  
	al_destroy_mixer(mixer);
	al_destroy_voice(voice);
	
	log_printf("DEBUG\t\tDestroying Audio Stream\n");
	log_printf("DEBUG\t\tDestroying Mixer\n");
	log_printf("DEBUG\t\tDestroying Voice\n");
	log_printf("DEBUG\t\tDestroying Event queue\n");

	pthread_exit(NULL);

}
/*-----------------------------------------------------------
 *---------------------------------------------------sine_src
 *-----------------------------------------------------------
 * TASK sine_src:  
 *      the task perform the the generation of sine
 *      signal based on the user defined frequency
 *      where the signal is generated using the standard
 *      [ sin() ] function called from math.h lib
 *
 --------------------------------------------------------*/

 TASK sine_src(TASK arg) {
	double  w, ti;	
	int 	i, m;
	BPF.fc_L 	= 200;
    BPF.fc_H 	= 2000; 
    BPF.FS 		= 44100; 
    BPF.N 		= 1024;   
    
    FIR_BPF(BPF.fc_L, BPF.fc_H, BPF.FS, BPF.N);

	task_sepcs.id_sinWave=get_task_index(arg);
	task_sepcs.prio_sinWave=get_task_period(arg);
	set_activation(task_sepcs.id_sinWave);
	
	while(!TASK_END) {

		pthread_mutex_lock(&Srsrc_mutx);             
		w = TWOPI * signal_params.freq_sin;
		t_sine+= dt * SAMPLES_PER_BUFFER;
        if(BPF_ENABLED) {
            float x[SAMPLES_PER_BUFFER];
            m=(BPF.N)-1;
            for (i = 0; i < SAMPLES_PER_BUFFER; ++i) { 
                ti = t_sine + i * dt;
                x[i] =signal_params.sine_switch*(sin(w * ti + 0));
            }
            for (i = 0; i<=m; i++){
                signal_params.sineWave_buf[i]=(signal_params.sineWave_buf[i]+x[i]*BPF.h[m-i]);
            }
        } 
        else{
        	for (i = 0; i < SAMPLES_PER_BUFFER; ++i) { 
				ti = t_sine + i * dt;
				signal_params.sineWave_buf[i] =signal_params.sine_switch*sin(w * ti);
			}       	
        }
		pthread_mutex_unlock(&Srsrc_mutx);	
    		
		wait_for_period(task_sepcs.id_sinWave);
		if(BARRIER_ENABLE){
			pthread_barrier_wait(&rsrc_barr);
		}
	}
	pthread_exit(NULL);
}
/*-----------------------------------------------------------
 *-----------------------------------------------triangle_src
 *-----------------------------------------------------------
 * TASK triangle_src:  
 *      the task perform the the generation of triangle  
 *      wave signal based on the user defined frequency
 *      from the standard function [ sin() ]
 --------------------------------------------------------*/
TASK triangle_src(TASK arg) {
	double	w, tx, tu;
	int 	i, m;
	BPF3.fc_L 	= 200;
    BPF3.fc_H 	= 2000; 
    BPF3.FS 	= 44100; 
    BPF3.N 		= 1024;   
    FIR_BPF(BPF3.fc_L, BPF3.fc_H, BPF3.FS, BPF3.N);

	task_sepcs.id_triWave=get_task_index(arg);
	task_sepcs.prio_triWave=get_task_period(arg);	
	set_activation(task_sepcs.id_triWave);
   
	while(!TASK_END) {
		
		pthread_mutex_lock(&Trsrc_mutx);
		w = TWOPI * signal_params.freq_tri;
		t_triangle+= dt * SAMPLES_PER_BUFFER;
        if(BPF_ENABLED) {
            float s[SAMPLES_PER_BUFFER];
            m=(BPF3.N)-1;
            for (i = 0; i < SAMPLES_PER_BUFFER; ++i) { 
                tx = w * (t_triangle + i * dt) + PI/2.0;
                tu = fmod(tx/PI, 2.0);
                if (tu <= 1.0)
                    s[i] = signal_params.tri_switch*(1.0 - 2.0 * tu);
                else
                    s[i] = signal_params.tri_switch*(-1.0 + 2.0 * (tu - 1.0));
            }
            for (i = 0; i<=m; i++){
                signal_params.triangleWave_buf[i]=(signal_params.triangleWave_buf[i]+s[i]*BPF3.h[m-i]);
            }
        }
		else { 
			for (i = 0; i < SAMPLES_PER_BUFFER; i++) {
				tx = w * (t_triangle + i * dt) + PI/2.0;
				tu = fmod(tx/PI, 2.0);
				if (tu <= 1.0){
					signal_params.triangleWave_buf[i] =  signal_params.tri_switch*(1.0 - 2.0 * tu);
				}
				else{
					signal_params.triangleWave_buf[i] =  signal_params.tri_switch*(-1.0 + 2.0 * (tu - 1.0));
				}
			}
		}
		pthread_mutex_unlock(&Trsrc_mutx);

		wait_for_period(task_sepcs.id_triWave);
		if(BARRIER_ENABLE){
			pthread_barrier_wait(&rsrc_barr);
		}
	}
	pthread_exit(NULL);
}
/*-----------------------------------------------------------
 *-------------------------------------------------square_src
 *-----------------------------------------------------------
  * TASK square_src:  
 *      the task perform the the generation of square 
 *      wave signal based on the user defined frequency
 *      where the signal is generated in a simple way
 *      by defining a simple threshold >@0 
 *      if sin(FREQ_BIN) >= 0
 *          set HIGH
 *      else 
 *          set LOW
 *      In this way, we generate an Ideal square waveform
 *
 * * P.S: In practical DSP (Digital Signal Processing):
 *          square wave is generated using the fourier 
 *          series by summing along a specific range of
 *          sinsoidal harmonics
 --------------------------------------------------------*/

TASK square_src(TASK arg) {
    
    double		w, x, ti;
	int 		i, m;
	BPF2.fc_L 	= 200;
    BPF2.fc_H 	= 2000; 
    BPF2.FS 		= 44100; 
    BPF2.N 		= 1024;   

    FIR_BPF(BPF2.fc_L, BPF2.fc_H, BPF2.FS, BPF2.N);

    task_sepcs.id_squWave=get_task_index(arg);
    task_sepcs.prio_squWave=get_task_period(arg);

    set_activation(task_sepcs.id_squWave);    

	while(!TASK_END) {

		pthread_mutex_lock(&Rrsrc_mutx);
		w = TWOPI * signal_params.freq_square;
        t_square+= dt * SAMPLES_PER_BUFFER;  
        if(BPF_ENABLED) {
            float s[SAMPLES_PER_BUFFER];
            m=(BPF2.N)-1;
            for (i = 0; i < SAMPLES_PER_BUFFER; ++i) { 
                ti = t_sine + i * dt;
                s[i] = (x >= 0.0) ? (signal_params.square_switch*1.0) : (signal_params.square_switch* -1.0);
            }
            for (i = 0; i<=m; i++){
                signal_params.squareWave_buf[i]=(signal_params.squareWave_buf[i]+s[i]*BPF2.h[m-i]);
            }
        } 
		else {
			for (i = 0; i < SAMPLES_PER_BUFFER; i++) {
	            ti = t_square + i * dt;
	            x = sin(w * ti);
	            signal_params.squareWave_buf[i] = (x >= 0.0) ? (signal_params.square_switch*1.0) : (signal_params.square_switch* -1.0);
 			}
 		}
		pthread_mutex_unlock(&Rrsrc_mutx);	
        //task_sepcs.dlmiss_squWave = deadline_miss(task_sepcs.id_squWave);        

		wait_for_period(task_sepcs.id_squWave);
		if(BARRIER_ENABLE){
			pthread_barrier_wait(&rsrc_barr);
		}
	}
	pthread_exit(NULL);
}

