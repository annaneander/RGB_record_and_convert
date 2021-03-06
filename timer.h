#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>



/*
  Timer: Initialise go
  TMR2ERIOD = clk_frequency / prescaler /timeout_frequency
  timeout_frequency = 100 ms = 0.1 sek
*/

#define CLOCKFREQ 80000000   /* 80MHz */
#define PRESCALER 256       /*  */
#define TIMEOUTFREQ 10  /* T: 100 ms (0.1 sek) -> f: 10 */

#define TMR2PERIOD ((CLOCKFREQ / PRESCALER) / TIMEOUTFREQ)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big > 65535."
#endif


void timer_init( void );




#endif
