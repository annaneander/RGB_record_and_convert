/* rgb_i2c.h

	 Author: Anna Neander (2021)
	 Based on example from https://github.com/is1200-example-projects/hello-temperature
   written by Axel Isaksson.

   For copyright and licensing, see file COPYING */


#ifndef RGB_SENSOR_I2C_H
#define RGB_SENSOR_I2C_H



/*
  7 bit I2C adressing protocol at adress 0x39

*/

/* Address of the rgb sensor on the I2C bus */
#define RGB_SENSOR_ADDR 0x39

/* Registers */
#define SENSOR_ON 0x80  /* bit 1*/
#define ENABLE_ALS 0x80 /* ambient light sensor bit 2*/

#define RED_DATA_L 0x96 /* low byte of red data*/
#define RED_DATA_H 0x97 /* high byte of red data*/
#define GREEN_DATA_L 0x96 /* low byte of green data*/
#define GREEN_DATA_H 0x97 /* high byte of green data*/
#define BLUE_DATA_L 0x96 /* low byte of blue data*/
#define BLUE_DATA_H 0x97 /* high byte of blue data*/

#define ON 0x3 /*turn sensor and als on*/

//#define CONTROL_BYTE  /**/
//#define DATA_BYTE



/*set up ic2 for RGB sensor */
void rgb_ic2_init( void );




#endif
