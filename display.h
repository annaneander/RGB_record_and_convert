/* * * display.h
*
*   Author: Anna Neander (2021). Based on code written by
*   F Lundevall (2015) and Axel Isaksson.
*   Written by (AN), modified (AN*).
*
*   For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>
//#include "display_data.h"


#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


/* ----------- display commands ------------ */
#define DISPLAY_CMD_SET_PAGE  (uint8_t)0x22


/* ----------- functions ------------------  */
void display_init(void);  /* (AN*)*/
void display_image(int x, int w, const uint8_t *data); /* (AN*)*/
void display_string(int line, char *s, int offset); /* (AN*)*/
void display_update(void);
void display_debug( volatile int *const addr );
void display_debug_2( volatile int *addr ); /* (AN*)*/
void display_debug_8( uint8_t *addr ); /* (AN*)*/

/*  ----- (AN) ---- */
void hello_display(void);
void display_rgbc(uint16_t* colors, bool lower);
void display_hex(uint16_t* colors);
void display_clear(void);
void display_clr_buffer(void);


/* ------- local helper function to display ----- */
char* num32asc_ret(int n ); /* (AN*) */
static void num32asc( char *s, int );
uint8_t spi_send_recv(uint8_t data);  // extern /static?
void quicksleep(int cyc); //extern / static?

char* uitoaconv(int num);  /* (AN*)  */
void concat(char *first, char *second); /*(AN)*/



/* ---  debug - remove --------*/
volatile int* debug;
//void display_debug_8( volatile uint8_t *addr ); /* (AN*)*/



#endif
