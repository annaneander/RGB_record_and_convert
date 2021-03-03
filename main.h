/* main.h
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
   Update 2015-08-28 by F Lundevall
   Author (AN) and modification (*AN*):  Anna Neander (2021)

   For copyright and licensing, see file COPYING */

   #ifndef MAIN_H
   #define MAIN_H

   #include <stdint.h>   /* Declarations of uint_32 and the like */
   #include <pic32mx.h>  /* Declarations of system-specific addresses etc */
   #include <stdbool.h>
   #include "display.h"
   #include "timer.h"


void chipkit_init(void);

void work(void);


/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);

void enable_interrupt(void);
void user_isr( void );


#endif
