/* * * timer.h
 *
 *   Author: Anna Neander (2021)
 *
 *   For copyright and licensing, see file COPYING
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>

/*
  Timer: Initialise to
  TMR2ERIOD = clk_frequency / prescaler /timeout_frequency = 312
  timeout_frequency = 100 ms = 0.1 sek
*/

#define CLOCKFREQ 80000000   /* 80MHz */
#define PRESCALER 256       /*  */
#define TIMEOUTFREQ 100  /* T: 10 ms (0.01 sek) -> f: 100 */

#define TMR2PERIOD ((CLOCKFREQ / PRESCALER) / TIMEOUTFREQ)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big > 65535."
#endif



void timer_init( void );




#endif
