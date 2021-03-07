## A light meter with colour conversions 


### Objective and Requirements
The purpose of the project is to use a small ambient light sensor to record data about light and display this to a user. The user can interact with the sensor and change settings, as well as save colours, although not in permanent storage. The project uses an external sensor connected to a uC32 Chipkit to read clear light and colours in three channels and shows information on a small OLED display.
	
#### The requirements for the projects are
- The sensor communicates over I2C with the Chipkit (via the I/O shield).
- A user can save (and display) up to 20 favourite colours (in full 16 bits) and retrieve them. This memory is (at present) not persistent.  
- The user can display clear light and colours in 8 and 16 bits RGB, as well as in HEX.
- The light meter shows a menu with options for the user.
- The user can change gain settings for the sensor, for various light conditions.
- The program makes use of a timer and interrupts to retrieve data and display information, as well as to control input from a user.

#### Solution 
In the project I was using a uC32 board with a PIC32MX340F512 microcontroller from Digilent, together with an I/O shield from the same brand. I was also using the RGB and gesture sensor APDS-9960 from Sparkfun, as well as some soldering equipment, cables and a voltmeter. This was kind of helpful in the beginning, to understand if the hardware was connected to the board. I used the built in I2C module in the shield, which sets up ports automagically. As far as I understand, this module also handles many interrupts used in the I2C communication. The I2C protocol that I have implemented uses a 7-bit addressing standard and after writing some settings to registers it reads continuously from the same 8 registers. 

All code is written in C (apart from 10 lines of asm) and the MCB32 toolchain was used to compile and assemble. External libraries used are stdint, stdbool and a header with register definitions for the Chipkit.

#### Verification 
To verify that the I2C communication was working I was reading different status flags in registers to check that they were set in the right way. I was only using software verification at this stage, mainly printouts from the code to the OLED display on the shield. To verify calculations I was using the same method. I have not verified the accuracy of the hardware, i.e. the sensor readings.       

#### Contributions
I have written all code, apart from some parts that were provided in previous lab exercises in the course. I have also used an example on how to read from a sensor, suggested by the course literature. This should be clearly marked in the code. 

#### Reflections
The project was very interesting and rewarding as well as frustrating. It is always difficult as a beginner, when one does not know where to start and how. Navigating manuals and data sheets (that are sometimes not accurate) and trying to use software that is not working were the hardest parts. The hands on access to hardware and the possibility to practice C were the most rewarding parts of the project. It was a very fun way to learn more about hardware and communication protocols.
