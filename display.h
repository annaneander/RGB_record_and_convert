/* * * display.h
*   Author: F Lundevall (2015), based on code written by Axel Isaksson.
*   Additions, comments and modifications:  Anna Neander (2021)
*
*   For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
//#include "main.h"
#include "display_data.h"


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
void display_init(void);
void display_image(int x, const uint8_t *data);
void display_string(int line, char *s);
void display_update(void);
void hello_display(void);

void display_debug( volatile int * const addr );
void display_debug_2( volatile int*  addr );
void display_debug_8( volatile uint8_t* addr );

/* ------- local helper function to display ----- */
static void num32asc( char * s, int );
uint8_t spi_send_recv(uint8_t data);  // static?
void quicksleep(int cyc); //static?

char * itoaconv( int num );  //används?

/* ------- bitmap array containing font-------- */
//extern const uint8_t const font[128*8];  //  1024 bytes, en rad på displayen?
/* Declare bitmap array containing icon */
//extern const uint8_t const icon[128]; // 32*4 bytes
/* Declare text buffer for display output */
//extern char textbuffer[4][16];


#endif
