/* timer.c (from lab 3)

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   Author (AN) and modification (*AN*):  Anna Neander (2021)
   For copyright and licensing, see file COPYING

   */

#include <stdbool.h>

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "timer.h"


/*
  Timer: Efter exempel i övning 2 för att låta kompilatorn beräkna perioden och
  ge fel om värdet > 2^16 (AN)
*/
#define TMR2PERIOD ((80000000 / 256) / 10)
#if TMR2PERIOD > 0xffff
#error "Timer period is too big > 65535."
#endif


/* Enable interrupts from Timer 2 and SW2 */
void timer_init( void )
{
  /*
    Prescale to 256. This also stops timer (bit 15 = 0) and
    selects internal PBCLK as source (bit 1 = 0).
  */
  T2CON = 0x70;

  /* clear/reset timer register */
  TMR2 = 0x0;

  /*
    Set period to (80 000 000 / 256) / 10 = 31250
    ––>  0111 1010  0001  0010  –—>   0x7A12
  */
  PR2 = TMR2PERIOD;

  /* Set IPC(2) prio (4:2) to 7 and subprio (1:0) to 3 */
  IPC(2) = 0x1f;  /* IPCSET(2) = 0x1f; */

  /* Clear the timer interrupt status flag. */
  IFSCLR(0) = 0x100;  /* samma sak som *(IFS +1) = 0x100; ?? */

  /* Enable timer2 interrupts*/
  IECSET(0) = 0x100; /* IEC(0) = (1 << 8); */


  /* set interrupt for sw2 (INT2). Values for tmr2 are set here again so the code above for prio and enable can be removed */

  IFSCLR(0) = 0x800;  /* clear status flag */

  //Set INT2 prio (28:26) to 7 and subprio(25:24) to 4
  IPC(2) = 0x1f00001f; /* + tmr2  (4:0)  */

  /* enable INT2 interrupts: IEC(0) (bit 11) */
  IECSET(0) = 0x800;
  //IEC(0) =  0x900;  /* + tmr 2  (bit 8) */

  /* start timer */
  T2CONSET = 0x8000;

  return;
}
