/* * * main.c
*
*   Author: Anna Neander (2021)
*
*   For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "main.h"

int timeout = 0x0;
uint16_t colors[4] = {0}; /* 16 bit color values of CRGB */
uint8_t colors_8[4] = {0}; /* 8 bit color values of CRGB */

int main(void) {

	bool status = 0;

	chipkit_init();
	display_init();
	timer_init();
	i2c_init();
	enable_interrupt();
	hello_display();

	/* TODO ?: use timer interrupt for something */
	if(hello_rgbc()) {
		quicksleep(4000000);
		display_string(0, "sensor connected",0);
		display_update();
		quicksleep(4000000);
		display_clear();
		/* display some menu */

		//quicksleep(40000000);

		while( 1 )
		{
			/*get value of 16 bit rgbc */
			status = i2c_get_rgbc(colors);

			/*perform conversions * /

			/* update display*/
			display_rgbc(colors);

			quicksleep(100000);

			/* save values */

		}

	}
	else {
		display_string(3, "not connected",0);
		display_update();
		quicksleep(4000000);

	}

	return 0;
}


/* set up chipkit */
void chipkit_init(void) {

OSCCONSET = 1 << 19; /* peripheral bus clock to 40Mhz; 1:2
/*
Set in <20:19> to 1:1, 1:2, 1:4 or 1:8.
Comment out last line will set the peripheral bus clock to
the same frequency as the sysclock. That means 80 MHz,
when the microcontroller is running at 80 MHz.
Then baud rates for SPI & I2C need to be changed as well. */

/* Set up output pins */
	AD1PCFG = 0xFFFF; /* analog i/o pins as digital */
	ODCE = 0x0; /* PORTE not open drain. (by DEFAULT?)*/
	TRISECLR = 0xFF; /* LED1...LED8 */
	PORTE = 0x0; /* Init LEDS to 0 */

/* Set up input pins (input by DEFAULT) */
	TRISDSET = (0xFE << 4); /* SW1...SW4 + Button2...Button4  */
	TRISFSET = (1 << 1); /* Button1 */

}



/* INT0 = INT1 = 2; INT2 = 7;  INT3 =  INT4 = 35  INT*/
/* BTN1 = 4 (RF1); BTN2 = 34 (RD5); BTN3 = 36 (RD6); BTN4 = 37 (RD7)  */

/* Interrupt Service Routine*/
/* ATM: Lite Tester*/
void user_isr( void ) {
	timeout++;
	if (IFS(0) & 1 << 8) {  /* timer 2*/
		/* get input from buttons */
		int buttons = getbtns();
		if (buttons & 1)
			display_string(0,"knapp 1 ", 0);
		if (buttons & 2)
			display_string(0,"knapp 2 ", 0);
		if (buttons & 4)
			display_string(0,"knapp 3 ", 0);
		if (buttons & 8)
			display_string(0,"knapp 4 ", 0);
		//display_update();
		quicksleep(10000);
		IFSCLR(0) = 1 << 8; /* clear timer2 flag */
	}

	if (IFS(0) & 1 << 11) { /* sw2 */
		display_string(0, "sw2", 0);
		//display_update();
		quicksleep(1000000);
		IFSCLR(0) = 1 << 11;  /* clear sw2 flag */
	}
	if (timeout == 10) {
		//display_string(0, "timeout" );
		//display_update();
		//display_image(0, font);
		//quicksleep(3000000);
		timeout = 0;
	}

}


/* -----------   some i/o functions for buttons and switches ---------     */

/* return status of switches SW4...SW1 in <3:0> */
int getsw( void ){
	return ((PORTD >> 8) & 0xF);
}

/* return status of push-buttons BTN4...BTN1 in <3:0>  */
int getbtns( void ){
	return ((PORTD >> 4) & 0xE) | ((PORTF >>1) & 0x1);
	//return ((((PORTD >> 5) & 0x7) << 1) | ((PORTF >> 1) & 0x1));
}
