/* main.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall.
	 Latest update 2017-04-21 by F Lundevall.
	 Comments and modifications:  Anna Neander (2021)

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <stdbool.h>
#include "main.h"
#include "display.h"
#include "timer.h"


int timeout = 0x0;
bool status = 1;


int main(void) {

	chipkit_init();
	display_init();
	timer_init();
	rgb_i2c_init();
	enable_interrupt();

	display_string(0, "RGB ");
	display_string(1, "R");
	display_string(2, "G ");
	display_string(3, "B");
	display_update();

	status = send_data(0x34);
	if (status){
		display_string(3, "ID ok");
	}
	else{
		display_string(3, "ID inte ok");
	}
		//display_update();

	//display_image(96, icon);
	//display_image(0, font);

	while( 1 )
	{
	  work();
	}

	return 0;
}


/* set up chipkit */
void chipkit_init(void) {
	/*
This will set the peripheral bus clock to the same frequency
as the sysclock. That means 80 MHz, when the microcontroller
is running at 80 MHz. Changed 2017, as recommended by Axel.
*/
SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
SYSKEY = 0x0;  /* Lock OSCCON */

/* Set up output pins */
AD1PCFG = 0xFFFF; /* analog i/o pins as digital */
ODCE = 0x0; /* PORTE not open drain. (by DEFAULT?)*/
TRISECLR = 0xFF;  /* LED1...LED8 */
PORTE = 0x0;  /* Init LEDS to 0 */

/* Set up input pins (input by DEFAULT) */
TRISDSET = (1 << 8);  /* SW1...SW4 + Button2...Button4  */
TRISFSET = (1 << 1); /* Button1 */

}



/* This function is called repetitively from the main program */
void work( void ) {

}


/* INT0 = INT1 = 2; INT2 = 7;  INT3 =  INT4 = 35  INT*/
/* BTN1 = 4 (RF1); BTN2 = 34 (RD5); BTN3 = 36 (RD6); BTN4 = 37 (RD7)  */

/* Interrupt Service Routine*/
/* ATM: Lite Tester*/
void user_isr( void ) {

  if (IFS(0) & 0x100){  /* timer 2*/
    IFSCLR(0) = 0x100; /* clear timer2 flag */

		/* get input from buttons */
		int buttons = getbtns();
		if (buttons & 1)
			display_string(0,"knapp 1 ");
		if (buttons & 2)
			display_string(0,"knapp 2 ");
		if (buttons & 4)
			display_string(0,"knapp 3 ");
		if (buttons & 8)
			display_string(0,"knapp 4 ");
		//display_update();
		quicksleep(10000);
		timeout++;
  }

  if (IFS(0) & 0x800){ /* sw2 */
    IFSCLR(0) = 0x800;  /* clear sw2 flag */

  	display_string(0, "sw2" );
  	//display_update();
		quicksleep(1000000);
		timeout++;

  }

  if (timeout == 10) {
		//display_string(0, "timeout" );
  	//display_update();
		//display_image(0, font);
		quicksleep(3000000);
    timeout = 0;
 	}

}



/* -----------   some i/o functions for buttons and switches ---------     */

/* get status of switches SW4...SW1 in (3:0) */
int getsw( void ){
  return ((PORTD >> 8) & 0xF);
}

/* get status of push-buttons BTN4...BTN1 in (3:0)  */
int getbtns( void ){
  return ((PORTD >> 4) & 0xE) | ((PORTF >>1) & 0x1);
  //return ((((PORTD >> 5) & 0x7) << 1) | ((PORTF >> 1) & 0x1));
}
