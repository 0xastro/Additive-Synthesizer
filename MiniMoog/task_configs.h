#ifndef TASK_CONFIGS_H
#define TASK_CONFIGS_H


#define NUM_TASKS		8

/*SinTask PARAMETERS*/
#define SIN_ID          0
#define SIN_PER         20
#define SIN_DL          20
#define SIN_PRIO        2

/*SquareTask PARAMETERS*/
#define SQU_ID          1
#define SQU_PER         10
#define SQU_DL          10
#define SQU_PRIO        3

/*TriangleTask PARAMETERS*/
#define TRI_ID          2
#define TRI_PER         10
#define TRI_DL          10
#define TRI_PRIO        4

/*AudioTask PARAMETERS*/
#define AUDIO_ID        3
#define AUDIO_PER       20  //200 no DLM
#define AUDIO_DL        20  
#define AUDIO_PRIO      5  

/*GraphicsTask PARAMETERS*/
#define UI_ID           4
#define UI_PER          50
#define UI_DL           50
#define UI_PRIO         6


/*KeyboardTask PARAMETERS*/
#define KEY_ID          5
#define KEY_PER         20
#define KEY_DL          20
#define KEY_PRIO        1

/*Number of Syncronized threads on the barrier*/
#define NBarrier        4


int 	LOG_ENABLE;
int 	TASK_END;
int 	BPF_ENABLED;
int 	BARRIER_ENABLE;

#endif
