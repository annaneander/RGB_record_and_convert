/* rgb_i2c.h

	 Author: Anna Neander (2021)
	 Based on example from https://github.com/is1200-example-projects/hello-temperature
   written by Axel Isaksson.

   For copyright and licensing, see file COPYING
*/


#ifndef RGB_SENSOR_I2C_H
#define RGB_SENSOR_I2C_H

/* 7 bit I2C adressing protocol with apds9960 sensor at adress 0x39 */
#define RGB_SENSOR_ADDR (0x39)

/* ------- Sensor registers ----------- */
#define ON (0x80)  /* <0> */
#define ENABLE 0x80 /* PON <0>, ALS <1>, WEN <3> AIEN <4>,*/

#define ATIME  (0x81)  /* time to measure (in ..) */
/* OBS även reg (0x82) anges som adress för detta  */

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

#define WRITE_SENSOR (0x72) /* LSB = 0 --> 0x39 << 1  */
#define READ_SENSOR (0x73) /* LSB = 1 --> 0x39 << 1 | 0x1 */
#define RGB_ALS_ON (0x1E) /* PON, AEN, AIEN, WEN */

#define DEVICE_ID 0x92
#define DEVICE_STATUS 0x93


/* ---------- start functions from example ----- */
void i2c_idle(void);
bool i2c_send(uint8_t data);
uint8_t i2c_recv(void);
void i2c_ack(void);
void i2c_nack(void);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);

/* ---------- end functions from example ----------- */

void rgb_i2c_init( void );
bool send_data(uint8_t data); //bara testfunkt atm

bool write_to_reg(uint8_t reg, uint8_t data);
volatile uint8_t* p_read_from_reg(uint8_t reg);

uint8_t read_from_reg(uint8_t reg);
void rgbc_from_reg(uint8_t reg);


#endif
