/******************************************************************
 This is the common ArduiPi include file for ArduiPi board
 
02/18/2013 	Charles-Henri Hallard (http://hallard.me)
						Modified for compiling and use on Raspberry ArduiPi Board
						LCD size and connection are now passed as arguments on 
						the command line (no more #define on compilation needed)
						ArduiPi project documentation http://hallard.me/arduipi

 Written by Charles-Henri Hallard for Fun .
 All text above must be included in any redistribution.
						
 ******************************************************************/

#ifndef _ArduiPi_SSD1306_H
#define _ArduiPi_SSD1306_H

#include <stdio.h>
#include <stdarg.h>  
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <bcm2835.h>

// Configuration Pin for ArduiPi board
#define OLED_SPI_RESET RPI_V2_GPIO_P1_22 /* GPIO 25 pin 22  */
#define OLED_SPI_DC    RPI_V2_GPIO_P1_18 /* GPIO 24 pin 18  */
#define OLED_SPI_CS    BCM2835_SPI_CS0 	 /* Chip Select CE0 */
#define OLED_I2C_RESET 4	

// Arduino Compatible type
typedef uint8_t boolean;
typedef uint8_t byte;

// Arduino Compatible Macro
#define _BV(bit) (1 << (bit))

// GCC Missing
#define max(a,b) (a>b?a:b)
#define min(a,b) (a<b?a:b)

#endif
