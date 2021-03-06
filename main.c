/* * * main.c
*
*   Author: Anna Neander (2021)
*
*   For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "main.h"

int timeout = 0;
uint16_t colors[4] = {0}; /* buffer - 16 bit color values of CRGB */
uint16_t fav_colors[20][4] = {0}; /* 20 favorite colors can be saved */
static uint8_t number_colors = 0;

bool rgb888 = 0; /* display 8 or 16 bit */
bool save = 0; /* save color menu */
bool menu = 0; /* display menu */

int main(void) {

	bool status = 0;

	chipkit_init();
	display_init();
	timer_init();
	i2c_init();
	enable_interrupt();
	hello_display();

	quicksleep(4000000);
	/*   use clock delay ? */

	/* TODO ?: use timer interrupt for something */
	if(hello_rgbc()) {

		display_string(0, "sensor connected",0);
		display_update();
		//quicksleep(4000000);
		//while(timeout);
		display_clear();

		/* display some menu */

		//quicksleep(40000000);

		while( 1 ) //!timeout ?
		{
			/*get value of 16 bit rgbc */
			status = i2c_get_rgbc(colors);

			/*perform conversions * /

			/* update display*/
			display_rgbc(colors, rgb888);

			quicksleep(100000);

			/* save values in array for now */
			if (save && number_colors < 20){
				*fav_colors[number_colors] = *colors;
				display_save(++number_colors);
				while (save);
				}

			/* menu */
				if (menu){
					display_menu();
						/* fetch saved values  */
						if (save){
							display_string(0,"display clo",0);
							display_update();
							quicksleep(4000000);
							int i;
							for(i = 0; i<number_colors; i++){
							display_rgbc(fav_colors[i] , rgb888);
							//delay

						}

						}
					}
					while (menu);


		}

	}
	else {
		display_string(3, "not connected",0);
		display_update();

		display_debug(&I2C1STAT);  //0x400  buss collison.
		display_debug_2(&I2C1CON); //0x73  recieved ??
		//display_debug(&I2C1RCV);
		//0x9020 -> 5- nack sent/recieved?, 12 (ckl relase control bit), 15 (on)

		//display_debug_2(&I2C1STAT)  --> 08008 = start/restart och nack



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



/* SW1 (2)= INT1; SW2 (7) = INT2; SW3 (8) = INT3; SW4 (35) = INT4 */
/* BTN1 = 4 (RF1); BTN2 = 34 (RD5); BTN3 = 36 (RD6); BTN4 = 37 (RD7)  */

/* manuell styrning: låt tex SW4 stänga av I2C och koppla två knappar till SCL och SDA (med LED som indik). Hur sätta portarna open drain? Eller måste A4/A5 användas?
SCL - 19/A5 - PMALH / PMA1 / U2RTS/ AN14 /  RB14
SDA - 18/A4 - TCK/ PMA11/ AN12/ RB12

*/

/* Interrupt Service Routine*/
void user_isr( void ) {

	if (IFS(0) & 1 << 8) {  /* timer 2 timeout 1/10 sek */

		/* get input from buttons */
		int buttons = getbtns();
		save = (buttons & 1); /* save color */

		if (buttons & 2)
			display_string(0,"knapp 2 ", 0);
			/* decrease gain ?  */
		if (buttons & 4)
			/*  increase gain ? */
			display_string(0,"knapp 3 ", 0);

		menu = (buttons & 8);
			/* display menu ?*/

		timeout++;
		IFSCLR(0) = 1 << 8; /* clear timer2 flag */
	}

	if (IFS(0) & 1 << 11) { /* sw2 */
		/* toogle 16 / 8 bit color value display*/
		rgb888 = !rgb888;
		IFSCLR(0) = 1 << 11;  /* clear sw2 flag */

	}
	if (timeout == 10) { /*  1 sek  */
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
