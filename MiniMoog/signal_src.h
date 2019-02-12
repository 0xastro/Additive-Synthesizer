#ifndef SIGNAL_SRC_H
#define SIGNAL_SRC_H


#include "shared.h"
#include "ptask.h"

//#define BPF_ENABLED 0

struct FIR_param {
    int     fc_H;                       //High Cutoff Freq
    int     fc_L;                       //Low Cutoff Freq
    float   h[SAMPLES_PER_BUFFER];      //Filter Coeficient array
    int     FS;                         //Sampling Freq
    int     N;                          //filter taps
}BPF,BPF2,BPF3;


struct waves_buffer {
    float           sineWave_buf[SAMPLES_PER_BUFFER];
    float           squareWave_buf[SAMPLES_PER_BUFFER];
    float           triangleWave_buf[SAMPLES_PER_BUFFER];
    float           sum[SAMPLES_PER_BUFFER];
    float           conv[SAMPLES_PER_BUFFER];
    unsigned int    frequency;
    float           freq_sin;
    float           freq_square;
    float           freq_tri;
    int             sine_switch;
    int             square_switch;
    int             tri_switch;
    float           gain;
}signal_params;



/*RESERVED*/
/*
enum Waveform {
    WAVEFORM_SINE,
    WAVEFORM_SQUARE, 
    WAVEFORM_TRIANGLE
} wave;
*/

double  t_sine, t_square, t_triangle;
float*   additive_buf;

//Wave generation function prototypes

TASK generate_wave(TASK arg);


TASK audio_thread(TASK arg);

TASK sine_src(TASK arg);

TASK square_src(TASK arg);

TASK triangle_src(TASK arg);

void additive_synths(float *buf);

void FIR_BPF(float flo, float fhi, float FS, int N);

/*RESERVED*/
//TASK generate_wave(void* signal_source);


#endif

