/* * * rgb_i2c.h * * *
*  Author: Anna Neander (2021).
*  Based on example written by Axel Isaksson:
*  https://github.com/is1200-example-projects/hello-temperature
*  Written by (AN), modified (AN*).
*
*  Sensor APDS9960 @ 0x39; Max 400 kHz
*  7 bit I2C adressing protocol.
*
*  For copyright and licensing, see file COPYING
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


#ifndef RGB_SENSOR_I2C_H
#define RGB_SENSOR_I2C_H

/*  internal I2C registers and bits */
#define I2CRCV   (I2C1RCV)
#define I2CTRN   (I2C1TRN)
#define I2CCON   (I2C1CON)
#define ACKSTAT  (I2C1STAT & (1 << 15))
#define TRSSTAT  (I2C1STAT & (1 << 14))
#define CL_OVERFLOW (I2C1STATCLR = 1 << 6)
#define BCL (I2C1STAT & (1 << 10))
#define WRCOL (I2C1STAT & (1 << 7))

#define START (I2C1CONSET = 1 << 0)
#define RESTART (I2C1CONSET = 1 << 1)
#define STOP (I2C1CONSET = 1 << 2)
#define ACKEN (I2C1CONSET = 1 << 4)
#define ACK (I2C1CONCLR = 1 << 5)
#define NACK (I2C1CONSET = 1 << 5)
#define RCVENBL (I2C1CONSET = 1 << 3)
#define CLKSTRETCH (I2C1CONSET = 1 << 6)


/* ------- Sensor registers ----------- */

#define RGBC_ADDRESS (0x39)
#define RGBC_ID 0x92

#define RGBC_ON (0x80)  /* <0> */
#define ENABLE 0x80 /* PON <0>, ALS <1>, WEN <3> AIEN <4>,*/

#define ATIME  (0x81)  /* measure time  */
/* OBS även reg (0x82) anges som adress för detta..?  */

#define WTIME  (0x83)  /* delay between measurements */
#define AILTL  (0x84)  /* low threshold, lower byte  */
#define AILTH  (0x85)  /* low threshold, upper byte  */
#define AIHTL  (0x86)  /* high threshold, lower byte  */
#define AIHTH  (0x87)  /* high threshold, upper byte  */

#define APERS  (0x8C)  /* <3:0> ALS Interrupt Persistence  */

#define WLONG  (0x8D)  /* <1> Wait Long Enable. Sets to max 8.45 s*/

#define A_GAIN  (0x8F)  /* <1:0> ALS Gain Ctrl */
#define SPSIEN  (0x90)  /* <6> Clear diode Saturation Interrupt Enable */

/* Reset by adress accessing. Meaning writing op to register done by only writing 2 bytes. One to adress and set write bit (0) and then register  */
#define CI_CLEAR  (0xE6)  /* <7:0> Clear ALS interrupt */


#define STATUS  (0x93)
#define CPSAT  (0x93)  /* <7> Clear diode Saturation */
#define AINT  (0x93)  /* <4> ALS Interrupt */
#define AVALID  (0x93)  /* <0> ALS Valid */


#define CLEAR_DATA_L 0x94 /* low byte of clear data*/
#define CLEAR_DATA_H 0x95 /* high byte of clear data*/
#define RED_DATA_L 0x96 /* low byte of red data*/
#define RED_DATA_H 0x97 /* high byte of red data*/
#define GREEN_DATA_L 0x98 /* low byte of green data*/
#define GREEN_DATA_H 0x99 /* high byte of green data*/
#define BLUE_DATA_L 0x9A /* low byte of blue data*/
#define BLUE_DATA_H 0x9B /* high byte of blue data*/

#define WRITE_SENSOR (RGBC_ADDRESS << 1)  /* 0x72  LSB = 0 */
#define READ_SENSOR (RGBC_ADDRESS << 1 | 1) /* 0x73  LSB = 1 */
#define RGB_ALS_ON (0x1E) /* PON, AEN, AIEN, WEN */




/* ---------- by A. Isaksson (AN*)-------- */
void i2c_idle(void);
bool i2c_send(uint8_t data);
void i2c_recv(void);
void i2c_ack(void);
void i2c_nack(void);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);

/* ---------- end by A. Isaksson ----------- */

void i2c_init( void );
void i2c_off( void );
void i2c_on( void);
bool hello_rgbc(void);
bool i2c_get_rgbc(uint16_t*);


/* ------- local functions ------  */
uint8_t read_from_reg(uint8_t reg);
bool read_block(uint8_t reg, int j, uint16_t* data);
bool write_to_reg(uint8_t reg, uint8_t data);

uint8_t* p_read_from_reg(uint8_t reg);


#endif
