/* * * main.h * * *
*
*  Author: Anna Neander (2021)
*
*  For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>
#include "display.h"
#include "timer.h"
#include "rgb_i2c.h"


void chipkit_init(void);
void save_colors(void);
void show_menu(void);
void set_gain();
uint8_t gain_converter(uint8_t gain);


/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);

void enable_interrupt(void);
void user_isr( void );


#endif
