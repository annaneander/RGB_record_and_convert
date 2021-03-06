/* timer.c

   Author: Anna Neander (2021)
   For copyright and licensing, see file COPYING

   */

#include "timer.h"

/* Init Timer 2 and enable interrupts from Timer 2 and SW2 */
void timer_init( void )
{
  /*
    Prescale 1:256. This also stops timer (bit 15 = 0) and
    selects internal PBCLK as source (bit 1 = 0).
  */
  T2CON = 0x70;
  TMR2 = 0x0; /* clear/reset timer register */
  PR2 = TMR2PERIOD;  /* timeout period */

  IPC(2) = 0x12; /* prio <4:2> to 4 and subprio <1:0> to 2 */
  IFSCLR(0) = 1 << 8; /* Clear timer2 interrupt flag. */
  IECSET(0) = 1 << 8;   /* Enable timer2 interrupts*/

  /* INT2 prio (28:26) to 3 and subprio (25:24) to 1 */
  IPC(2) = 0x0D000012; /* + tmr2  (4:0)  */
  IFSCLR(0) = 1 << 11; /* clear INT2 interrupt flag. */

  IECSET(0) = 1 << 11; /* enable INT2 interrupts */
  T2CONSET = 1 << 15;   /* start timer */

  return;
}
