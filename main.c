/* * * main.c
 *
 *   Author: Anna Neander (2021)
 *
 *   For copyright and licensing, see file COPYING
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "main.h"
#define MAX_COLORS (20)/* max number of colors to save */

uint8_t timeouts = 0; /* nbr of timer2 timeouts */
bool ready = 0; /* 1/10 sek has passed */
uint8_t gainflag = 0;
bool gain_ready = 0;
uint16_t colors[4] = {0}; /* buffer - 16 bit color values of CRGB */
uint16_t fav_colors[MAX_COLORS][4] = {0}; /* 20 favorite colors can be saved */
static uint8_t number_colors = 0;

bool rgb888 = 0; /* display 8 or  16 bit */
bool save = 0; /* save color menu */
bool menu = 0; /* display menu */
bool gain = 0;
bool gain_set = 0;
bool start = 0;

int main(void) {

	bool status = 0;

	chipkit_init();
	display_init();
	timer_init();
	i2c_init();
	enable_interrupt();
	quicksleep(100);

	if(hello_rgbc()) {


		//hello_display();


		while (1) { //&& startflag

			while(ready) /* update values and display every 1/20 sek */
			{
				/*get value of 16 bit rgbc */
				i2c_get_rgbc(colors);

				/* update display*/
				display_rgbc(colors, rgb888);

				/* debug */
				//display_clear();


				if (save)
					save_colors();

				if (menu)
					show_menu();

				if (gain)
					set_gain();

			}

		} /* while (1) */

	}   /* if (hello) */
	else {
		// display_string(0, "Sensor not",0);
		// display_string(1, "connected.",0);
		// //display_string(2, "Restart or SW4",0);
		// display_string(2, "Run manual mode",0);
		// display_update();

		/* ---------- debug --------- */
	//	display_debug(&I2C1STAT);  //0x400  buss collison.
	//	display_debug_2(&I2C1CON);
		//0x9020 -> 5- nack sent/recieved?, 12 (ckl relase control bit), 15 (on)

		/* set up IC2 connection with buttons */
		/* manuell styrning: låt tex SW4 stänga av I2C och koppla två knappar till SCL och SDA (med LED som indik). Hur sätta portarna open drain? Eller måste A4/A5 användas?
		   SCL - 19/A5 - PMALH / PMA1 / U2RTS/ AN14 /  RB14
		   SDA - 18/A4 - TCK/ PMA11/ AN12/ RB12
		 */

		/* disable 12C */
		i2c_off();

		/*-------- debug------*/
	//	quicksleep(100);
	//	display_debug_2(&I2C1STAT);


	}
	return 0;
} /* main */



/* ----- save values in array for now ----- */
void save_colors(){
	if (number_colors < MAX_COLORS) { /* if button 4 is down */
		int i;
		for(i = 0; i<4; i++) {
			*(fav_colors[number_colors]+i) = *(colors+i);
		}
		display_save(++number_colors);
		while (save); /* while button 4 is down */
	}
}


/* ----- menu ---- */
void show_menu(){
	bool fetching = 0;
	int counter = 0;
	while (menu) { /* while button 1 is down */
		!fetching && display_menu();

		/* ---- display saved values ------ */
		if (**fav_colors && save) { /*if there are some saved colors */
			fetching = 1;
			display_rgbc(*(fav_colors+counter), 1); /* show 8 bit colors */
			display_string(0,"#",10);
			display_string(0,uitoaconv(counter + 1),11);
			display_update();
			while(save); /* while button 4 is down */
			counter = (counter < number_colors-1) ? counter+1 : 0;
		}
	}
}


/* change gain settings in sensor */
/* 1X: 0    4X:1   16X:2   64X: 3 */
void set_gain(){
	display_clear();
	uint8_t old_gain = read_from_reg(A_GAIN);
	display_string(0,"Gain",0);
	display_update();

	display_string(2,"Previous:",0);
	display_string(2,"x",9);
	display_string(2,uitoaconv(gain_converter(old_gain)),10);
	display_update();

	while(gain) { /* while button 3 down */
		if (gain_set && gain_ready) {
			old_gain++;
			if (old_gain > 3)
				old_gain = 0;
			display_string(3,"New:",0);
			display_string(3,"x",9);
			display_string(3,uitoaconv(gain_converter(old_gain)),10);
			display_update();
			gain_ready = 0;
		}
		write_to_reg(A_GAIN, old_gain);
	}
}

/* a small helper to display right gain*/
uint8_t gain_converter(uint8_t gain){
	if (gain == 0)
		return 1;
	if (gain == 1)
		return 4;
	if (gain == 2)
		return 16;
	if (gain == 3)
		return 64;
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


/* Interrupt Service Routine*/
void user_isr( void ) {

	if (IFS(0) & 1 << 8) {  /* timer 2 timeout 1/100 sek */
		/* get input from buttons */
		int buttons = getbtns();
		save = (buttons & 1); /* B1: save/fetch color */
		gain_set = (buttons & 2); /* B2: change gain*/
		gain =  (buttons & 4); /* B3: toogle set gain 3*/
		menu = (buttons & 8);  /* B4: display menu */
		timeouts++;
		IFSCLR(0) = 1 << 8; /* clear timer2 flag */
	}
	if (IFS(0) & 1 << 11) { /* sw2 */
		/* toogle 16 / 8 bit color value display*/
		rgb888 = !rgb888;
		IFSCLR(0) = 1 << 11;  /* clear sw2 flag */
	}

	/* check if inter from SW4  */
	if (IFS(0) & 1 << 19) { /* SW4 */
		/* on/off sensor */
		start = !start;
		IFSCLR(0) = 1 << 19;  /* clear Sw4 flag */
	}

	if (timeouts == 5) { /*  1/20 sek  */
		timeouts = 0;
		ready = !ready;
		gainflag++;
	}
	if (gainflag == 5){ /* 5 -> every second */
		gainflag = 0;
		gain_ready = 1;
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
