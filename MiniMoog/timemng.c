#include "timemng.h"


/*//////////////////////time_copy///////////////////////
 *
 * This function copies a source time variable @ts into a
 * destination variable pointed by @td
 *//////////////////////////////////////////////////////

void time_copy(tspec *td, tspec ts) {
	
	td->tv_sec	= ts.tv_sec;
	td->tv_nsec	= ts.tv_nsec;
}

/*//////////////////////time_add_ms/////////////////////
 *
 * This function adds a value @ms expressed in millisec-
 * -onds to the time variable pointed by @t
 *
 *//////////////////////////////////////////////////////

void time_add_ms(tspec *t, int ms) {
	
	t->tv_sec+=		(ms/1000);
	t->tv_nsec+=	((ms%1000) * 1000000);
	
	if (t->tv_nsec > 1000000000) {
		t->tv_nsec -= 1000000000;
		t->tv_sec += 1;
	}
}

/*//////////////////////time_cmp////////////////////////
 *This function compares two time variables @t1 and @t2 
 * and  returns (0) 	if they are equal,
 * 	    returns (1) 	if @t1 > @t2
 *		returns (-1) 	if @t1 < @t2 
 *
 *//////////////////////////////////////////////////////

int time_cmp(tspec t1, tspec t2) {
	
	if (t1.tv_sec > t2.tv_sec) {
		return 1;
	}
	if (t1.tv_sec < t2.tv_sec) {
		return -1;
	}
	if (t1.tv_nsec > t2.tv_nsec) {
		return 1;
	}
	if (t1.tv_nsec < t2.tv_nsec) {
		return -1;
	}
	 
	return 0;
}
