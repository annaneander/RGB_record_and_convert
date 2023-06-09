/* * * display.c
 *
 *   Author: Anna Neander (2021). Based on code written by
 *   F Lundevall (2015) and Axel Isaksson.
 *   Written by (AN), modified (AN*).
 *
 *   For copyright and licensing, see file COPYING
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#include "display.h"
#include "display_data.h"

/* Text buffer for display output */
char textbuffer[4][16];


/* set up display  (AN*) */
void display_init(void) {

	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = (0x7 << 4);
	TRISGCLR = (1 << 9);

	/* Example from chipkit ref: Baudrate of 15 -> 8Mhz, with 80Mhz PBCLK clock */

	/* Set up SPI as controller on port 2 */
	SPI2CON = 0;  /* turn off and reset control  */
	SPI2BRG = 4;  /* --> 10Mhz; which is max for shield display  */
	/*SPI2BRG = 15; //8Mhz, with 80Mhz PB clock  */
	SPI2STATCLR = (1 << 6);  /* SPI2STAT bit SPIROV = 0; No overflow */
	SPI2CONSET = (1 << 6);  /* SPI2CON bit CKP = 1; Idle = Clock is low */
	SPI2CONSET = (1 << 5);  /* SPI2CON bit MSTEN = 1;  Master/Controller */
	SPI2CONSET = (1 << 15); /* SPI2CON bit ON = 1;  PON */


	DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	/* Display off command */
	spi_send_recv(0xAE);

	/* Bring Reset low and then high */
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	/* Send the Set Charge Pump and Set Pre-Charge Period commands */
	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	/*  Turn on VCC and wait 100 ms */
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	/* Invert the display. This puts the display origin
	   in the upper left corner. */
	spi_send_recv(0xA1); /* remap columns */
	spi_send_recv(0xC8); /* remap rows */

	/* Select sequential COM configuration. This makes the display memory
	   non-interleaved.*/
	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	/* Display on */
	spi_send_recv(0xAF);
}

void hello_display(){
	display_string(0, " AMBIENT LIGHT ",0);
	display_string(1, "----- **** -----", 0);
	display_string(2, "SW 4: START ", 0);
	display_string(3, "BTN 4: MENU ", 0);
	display_update();
}


void display_menu(){
	display_string(0, "(SW2): 8/16 bit",0);
	display_string(1, "(3)+(2): gain ", 0);
	display_string(2, "(1): save", 0);
	display_string(3, "(4)+(1): fetch", 0);
	display_update();
}

void display_save(uint8_t nbr){
	display_clear();
	display_string(0, " nice", 7);
	display_string(1, "color", 7);
	display_string(2, "#", 9);
	display_string(2, uitoaconv(nbr), 10);
	display_string(3, "saved", 7);
	display_update();
}


/* (AN) update display with 8 (lower) or 16 bit RGBC values */
void display_rgbc(uint16_t* colors, bool lower){
	char *col[4] = {"LUX: ","  R: ", "  G: ", "  B: "};
	int depth = lower ? 8 : 0; /* divide by 256 to get rgb888 */
	int i;
	for (i = 0; i<4; i++) {
		display_string(i, *(col+i), 0);
		display_string(i, uitoaconv(*(colors+i) >> depth), 4);
	}
	display_update();
}


/* (AN) update display with hex values of color */
void display_hex(uint16_t* colors){
	char *col[3] = {"LUX: "," ","HEX: "};
	int i;
	int h = 0;
	display_clr_buffer();
	for (i = 0; i<3; i++) {
		display_string(i, *(col+i), 0);
	}

	display_string(0, uitoaconv(*(colors)), 4); //keep lux 16
	h = ((*(colors+1) >> 8)) << 16; /* 8 bit R */
	h |= ((*(colors+2) >> 8)) << 8; /* 8 bit G */
	h |= (*(colors+3) >> 8);  /* 8 bit G */
	display_string(2, num32asc_ret(h), 4);
	display_update();
}



/* (AN) clear all bits in local textbuffer and display */
void display_clear(void){
	uint8_t clear[128*5];
	int i;
	for (i = 0; i<128*5; i++)
		clear[i] = 0x00;
	display_image(0, 128, clear);
	display_clr_buffer();
	display_update();
}

/* (AN) clear local text buffer for display */
void display_clr_buffer(void){
	int i, j;
	for( i = 0; i <4; i++) {
		for(j = 0; j<16; j++)
			textbuffer[i][j] = ' ';
	}
}


/* (AN) adds (second) to end of (first). (first) should be right size */
void concat(char *first, char *second){
	while(*first++);
	while(*first++ = *second++); /* including end 0*/
}



/** (AN*)
 * Store max 16 chars in textbuffer on line 0..3.
 * Offset from left with x positions.
 * Does not overwrite text buffer before pos x.
 * Content of textbuffer is used to update display in
 * function display_update().
 */
void display_string(int line, char *s, int offset) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = (0 + offset); i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

/* (AN*)
   Displays a bitmap array image (data) with width (w) and offset (x)
   from top left corner. Height is 32 pxl.
 */
void display_image(int x, int w, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		quicksleep(10);

		/* set row */
		spi_send_recv(DISPLAY_CMD_SET_PAGE);
		spi_send_recv(i);

		/* Start at column offset x */
		spi_send_recv(x & 0xF);  // set low nybble of column
		spi_send_recv(0x10 | ((x >> 4) & 0xF)); // set high nybble of column

		DISPLAY_CHANGE_TO_DATA_MODE;
		quicksleep(10);

		for(j = 0; j < w; j++)
			spi_send_recv(data[i*w + j]);
	}

	/* Test: För att printa fler rader av fonten. Ändra index pga array längre än displayminnet */
	// for(j = 256; j < 640; j++)
	// 	spi_send_recv(~data[i*w + j]);  //NB. inverted image
	//}
}


/*
 * itoa - rewritten for max 24 bit (2^24 = 17 777 215) unsigned (AN).
 * Author: Axelsson / Lundevall.
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 *
 */
#define ITOA_BUFSIZ ( 9 )
char* uitoaconv(int num )
{
	register int i;
	static char itoa_buffer[ ITOA_BUFSIZ ];
	itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;  /* Insert the end-of-string marker. */
	{
		i = ITOA_BUFSIZ - 2;  /* Location for first ASCII digit. */
		do {
			itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit.*/
			num = num / 10; /* Remove digit from number. */
			i--;  /* Move index to next empty position. */
		} while( num > 0 );

	}
	/* Since the loop always sets the index i to the next empty position,
	 * we must add 1 in order to return a pointer to the first occupied position. */
	return(itoa_buffer + i + 1);
}



/* Just rewritten to get return and remove start '0' (AN*)
   Converts a number to hexadecimal ASCII digits. */
char* num32asc_ret(int n )
{
	int i;
	int j = 0;
	int k = 0;
	static char t[6];
	for( i = 28; i >= 0; i -= 4 )
		t[j++] = "0123456789ABCDEF"[ (n >> i) & 15 ];
	while(t[k++] == '0');
	return &t[k-1];
}


/* only changed the lines and types (AN*)  */
void display_debug_2( volatile int* addr )
{
	display_string( 2, "Addr", 0);
	display_string( 3, "Data", 0 );
	num32asc( &textbuffer[2][6], (int) addr );
	num32asc( &textbuffer[3][6], *addr );
	display_update();
}

void display_debug_8( uint8_t* addr )
{
	int ad = (*addr + 0x0);
	display_string( 2, "Addr", 0);
	display_string( 3, "Data", 0 );
	num32asc( &textbuffer[2][6], (int) addr );
	num32asc( &textbuffer[3][6], ad );
	display_update();
}



/* ---------- Below all written by Isaksson / Lundevall ------------ */


/*
   Update display with contents of textbuffer,
   via bitmap font to show text
 */
void display_update(void) {
	int i, j, k;
	int c;

	for(i = 0; i < 4; i++) { /* for each row */
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		quicksleep(10);
		spi_send_recv(DISPLAY_CMD_SET_PAGE);
		spi_send_recv(i);  /* set row nbr */

		/* Start at the left column */
		spi_send_recv(0x0); //set low nybble of column
		spi_send_recv(0x10); //set high nybble of column

		DISPLAY_CHANGE_TO_DATA_MODE;
		quicksleep(10);
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}


/*
   helper function
   send one byte to update display
 */
uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;  /* update display buffer with one byte*/
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

/* Helper function.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
	int i;
	for( i = 28; i >= 0; i -= 4 )
		*s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}


/*
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


/*
   Debug.
   The two middle lines of the display show
   an address and its current contents.
   Parameter: the address to read and display.
 */
void display_debug( volatile int * const addr )
{
	display_string( 0, "Addr", 0 );
	display_string( 1, "Data", 0 );
	num32asc( &textbuffer[0][6], (int) addr );
	num32asc( &textbuffer[1][6], *addr );
	display_update();
}
