/* * * rgb_i2c.c * * *
 *  Author: Anna Neander (2021).
 *  Based on example written by Axel Isaksson:
 *  https://github.com/is1200-example-projects/hello-temperature
 *  Written by (AN), modified (AN*).
 *
 *  Sensor APDS9960 @ 0x39; Max 400 kHz
 *
 *  For copyright and licensing, see file COPYING
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



#include <stdbool.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#include "rgb_i2c.h"

#include "display.h"


void i2c_init(void){

	/* ------------- Set up I2C ---------- */

	I2C1CON = 0x0;   /* turn off and reset i2c */
	/* PBCLK is set to 40Mhz in OSCCON <20:19>.
	   Then 0x00C5 --> 100 kHz ;
	   Change to 80 Mhz --> BRG: 0x018A --> 100 kHz */
	I2C1BRG = 0x00C5;   /* baud rate generator clk --> 100 kHz*/
	I2C1STAT = 0x0;    /* clear acken, recieve, stop, restart, start */
	//CLKSTRETCH;  /* enbl clockstretch; only in slave mode!*/
//	I2C1CONSET = 1 << 13;   /* Stop in idle mode */
	I2C1CONSET = 1 << 15;   /* Turn on i2c. Sets pins to tri-state */

	/* ---------------------------- */

	/* Enable external interrupts to pin 2 - INT1. Pin2 is input by default - obs samma som SW2  */
	// flagga: IFS(0) <7>
	// enable: IEC(0) <7>
	// prio: IPC1 <28:26>  ; sub: <25:24>



	/* ----- write to control registers in sensor ----- */

	/*
	   TODO:
	   the Sleep After Interrupt bit, SAI ? set?
	 */

	write_to_reg(ATIME, 0xC0); /* Measure time: 175 ms for max (2^16 bit) color range */
	write_to_reg(A_GAIN, 0x1); /* Gain control: 4x */
	write_to_reg(WTIME, 0xAC); /* delay; set to 236 ms; default 2.78 ms */
	//  write_to_reg(WLONG , 0x); /* <1> Wait Long Enable --> time x12. Default off */

	//write_to_reg(A_PERS, 0x); /* persistence */
	//  write_to_reg(AILTL, 0x);  /* low threshold, lower byte  */
	//  write_to_reg(AILTH, 0x); /* low threshold, higher byte  */

//	write_to_reg(ENABLE, 0x1B); /* Wait <3>, Interrupt<4>, ALS <1>, Start <0>*/
	write_to_reg(ENABLE, 0xB); /* Wait, Start  */
}

/*  address should be here?  I2C1ADD */

/* check if the right sensor is connected */
bool hello_rgbc(){
	quicksleep(100);
	bool status = (read_from_reg(RGBC_ID) == 0xAB);
	if (!status && BCL) { /* check busscollision */
		/* also check write to trn-collision WRCOL? */

		I2C1CONCLR = 1 << 15;    /*turn i2c off -> i/o bits controlled by PORT functs*/
		quicksleep(10000);
		i2c_init();     /*detta funkar men nödvändigt? */
		//I2C1CONCLR = 1 << 15;/*turn i2c off  */
		quicksleep(10000);
		//i2c_start();
		//I2C1CONSET = 1 << 15; /*turn i2c on -> SDA & SCL as tristate ports, controlled by I2C module*/
		//quicksleep(100);
	}
	status = (read_from_reg(RGBC_ID) == 0xAB);
	return status;
}


/* ---------------- by A. Isaksson (AN* + added comments)------------------ */

/* Wait for I2C bus to become idle. When flags for ackEN, recieve,
   stop, restart, start and TRSTAT (controller transmit in progress)
   are cleared. I2CON-flags are cleared by module. */
void i2c_idle() {
	while(I2C1CON & 0x1F || TRSSTAT);
}

/* Send one byte on I2C bus, returns inverted ack/nack status of transaction */
bool i2c_send(uint8_t data) {
	i2c_idle();
	I2CTRN = data;  /* put data to send in transfer register */
	i2c_idle();
	return !(ACKSTAT);
}

/*  Receive one byte from I2C bus to reg I2CRV */
void i2c_recv() {
	i2c_idle();
	RCVENBL; /* recieve data enbled. Clears automatically */
	i2c_idle();
	CL_OVERFLOW;  /* clear buffer overflow if set, if attempts to write when RCV was full */
}

/* Send acknowledge conditon on the bus */
void i2c_ack() {
	i2c_idle();
	ACK;
	ACKEN; /* enable ack/nack */
}

/* Send not-acknowledge conditon on the bus */
void i2c_nack() {
	i2c_idle();
	NACK;
	ACKEN;
}

/* Send start conditon on the bus */
void i2c_start() {
	i2c_idle();
	START;
	i2c_idle();
}

/* Send restart conditon on the bus */
void i2c_restart() {
	i2c_idle();
	RESTART;
	i2c_idle();
}

/* Send stop conditon on the bus */
void i2c_stop() {
	/* fastnar här och ser ingen nack före stop i protokollet för sensorn så provar utan. ev kolla flaggorna */
	//i2c_idle();
	STOP;  /* --> slave sets stop <4> and clears start <3> in I2CCON  */
	i2c_idle();

}

/* check ACKSTAT after transmit? */
/* check write to trn-collision WRCOL?  - needs to be cleared manually  */


/* ---------------- END  by A. Isaksson (AN* + added comments)------------------ */



/* write one byte (data) to register (reg) in sensor */
bool write_to_reg(uint8_t reg, uint8_t data){
	bool status = 0;
	do {
		i2c_start();
	} while(!i2c_send(WRITE_SENSOR));
	status = (i2c_send(reg) && i2c_send(data));
	i2c_stop();
	return status;
}

/* read one byte from register (reg) in sensor */
uint8_t read_from_reg(uint8_t reg){
	bool status = 0;
	//TODO skriv om ?
	do {
		i2c_start();
	} while(!i2c_send(WRITE_SENSOR));
	status = i2c_send(reg);
	if (status) {
		i2c_restart();
		status = i2c_send(READ_SENSOR);       /* Send read */
		i2c_recv();  //check ackstat?
		i2c_nack();  //check ackstat?  //ev kommentera ut denna?
		i2c_stop();  //check ackstat?
	} else{
		*debug = I2CCON; //debug
	}
	return I2CRCV;
}


/* Läser in 16 bit värden av RGBC i pekare (c)
 * Returnerar status av läsning från sensor */
/* TODO: check if sensor saturated and other flags and reset*/
bool i2c_get_rgbc(uint16_t* c){
	// bool read_status = 0; /*splittat pga debug*/
	// read_status = read_block(CLEAR_DATA_L, 4, c);
	// return read_status;
	return (read_block(CLEAR_DATA_L, 4, c));
}


/* Read (j) number of 2 bytes to adress of (data) from register in sensor starting with (reg). Return status of transfer. */
bool read_block(uint8_t reg, int j, uint16_t* data){
	bool status = 0;
	do {
		i2c_start();
	} while(!i2c_send(WRITE_SENSOR));  /* Send write */
	status = i2c_send(reg);  /* register to access */
	do {
		i2c_restart();
	} while(!i2c_send(READ_SENSOR)); /* Send read */

	// Alla värden på CRGB låses och hämtas om läsningen börjar med adressen för C_LOW --> rätt färgvärde hämtas.
	int i;
	for(i = 0; i<4; i++) {
		i2c_recv();
		*(data+i) = I2CRCV <<8;
		i2c_ack();
		i2c_recv();
		*(data+i) |= I2CRCV;
		if(i<3)
			i2c_ack();
	}
	i2c_nack(); //ev kommentera ut denna?
	i2c_stop();
	return status;
}




/* -------------------- används inte -----------------       */

/* read one byte from register (reg) in sensor */
uint8_t* p_read_from_reg(uint8_t reg){
	uint8_t* data = 0x0;
	bool status = 0;
	do {
		i2c_start();
	} while(!i2c_send(WRITE_SENSOR));  /* Send write */
	if (!i2c_send(reg)) {
		// TODO:Hantera ev nack från slave
	}
	do {
		i2c_restart();
	} while(!i2c_send(READ_SENSOR)); /* Send read */
	i2c_recv(); //hantera ev fel!
	i2c_nack();
	i2c_stop();
	//display_debug(0xBF805060);  //I2CRCV
	//display_debug_8(&I2CRCV);  //   (I2C1RCV)
	*data = (I2CRCV);
	return data;
}
