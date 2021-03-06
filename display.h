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
void display_init(void);
void display_image(int x, int w, const uint8_t *data);
void display_string(int line, char *s, int offset);
void display_update(void);
void hello_display(void);
void display_rgbc(uint16_t* colors, bool lower);
void display_clear(void);
void display_clr_buffer(void);


void display_debug( volatile int *const addr );
void display_debug_2( volatile int *addr );
void display_debug_8( volatile uint8_t *addr );


/* ------- local helper function to display ----- */
static void num32asc( char *s, int );
uint8_t spi_send_recv(uint8_t data);  // static?
void quicksleep(int cyc); //static?

char* itoaconv(int num);
void concat(char *first, char *second);
static void uint16_t_asc( char * s, int n );

volatile int* debug;

#endif
