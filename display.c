/* display.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson
	 Comments and modifications:  Anna Neander (2021)

   For copyright and licensing, see file COPYING */


#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
//#include "mipslab.h"  /* Declatations for these labs */
#include "display.h"


/* Text buffer for display output */
char textbuffer[4][16];


/*
 	set up display
*/
void display_init(void) {
  /* Output pins for display signals */
  PORTF = 0xFFFF;
  PORTG = (1 << 9);
  ODCF = 0x0;
  ODCG = 0x0;
  TRISFCLR = 0x70;
  TRISGCLR = 0x200;

  /* Set up SPI as control */
  SPI2CON = 0;  /* control */
  SPI2BRG = 4;  /* Baud rate generator */
  /* SPI2STAT bit SPIROV = 0; */
  SPI2STATCLR = 0x40;
  /* SPI2CON bit CKP = 1; */
  SPI2CONSET = 0x40;
  /* SPI2CON bit MSTEN = 1; */
  SPI2CONSET = 0x20;
  /* SPI2CON bit ON = 1; */
  SPI2CONSET = 0x8000;

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

/**
*	Store max 16 chars in textbuffer on line 0..3.
*	Content of textbuffer is used to update display in
*	function display_update().
*/
void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

/*
 Displays a bitmap image with offset (x) from top left corner
*/
void display_image(int x, const uint8_t *data) {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(DISPLAY_CMD_SET_PAGE);
		spi_send_recv(i);

		/* Start at column offset x */
		spi_send_recv(x & 0xF);  // ?? set low nybble of column  ??
		spi_send_recv(0x10 | ((x >> 4) & 0xF)); //?? set high nybble of column

		DISPLAY_CHANGE_TO_DATA_MODE;

		//for(j = 0; j < 32; j++)  //only 32 pxl atm
		//	spi_send_recv(~data[i*32 + j]);  //inverted image
	//}

//test för att printa fonten.
/* 4 första raderna
for(j = 0; j < 128; j++)
 spi_send_recv(~data[i*128 + j]);  //inverted image
}
/*
  /* 4 sista raderna*/
	for(j = 256; j < 640; j++)
		spi_send_recv(~data[i*128 + j]);  //inverted image
}

}

/*
	Update display with contents of textbuffer, via bitmap font to show text
*/
void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(DISPLAY_CMD_SET_PAGE);
		spi_send_recv(i);

		/* Start at the left column */
		spi_send_recv(0x0); //set low nybble of column
		spi_send_recv(0x10); //set high nybble of column

		DISPLAY_CHANGE_TO_DATA_MODE;

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
 	send one byte to update display?
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
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}





/*
  TODO: ANVÄNDS DENNA?

 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
