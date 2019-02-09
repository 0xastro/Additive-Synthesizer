#ifndef TIMEMNG_H
#define TIMEMNG_H

#include <time.h>

/*timespec is a structure containing only 2 variables
 * 	@time_t tv_sec;  " seconds "
 * 	@long tv_nsec;   " nanoseconds ""
 */
typedef struct timespec tspec;


void	time_copy(tspec *td, tspec ts);

void	time_add_ms(tspec *t, int ms);

int		time_cmp(tspec t1, tspec t2);


#endif
