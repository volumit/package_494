#include<stddef.h>
#include<errno.h>
#include<stdio.h>
#include<sys/stat.h>
#include<sys/times.h>
#include<sys/time.h>

/* the times fcuntion is no longer part of the libos, because it depends
   on the used TRriCore derivate, the crystal oszillator etc.
   to get a system time you have to write your own times the following
   implementation is an example for the TC1796 with a quartz frequency
   of 20 MHz
*/
#if defined(__TC1796__) 
#include <tc1796b/scu.h>
#include <tc1796b/stm.h>
#endif

#define FREQ 75000000	/* clock rate of processor in Hz */


clock_t times (struct tms *buf)
{
	clock_t clk = (clock_t ) (STM_TIM2.reg);
	if (buf != NULL) {
		buf->tms_utime = clk;
		buf->tms_stime = 0;
		buf->tms_cutime = 0;
		buf->tms_cstime = 0;
	}
	return clk;
};
