ArduiPi SSD1306 OLED Library Driver
===================================

This is the port of the Adafruit_SSD1306 Arduino OLED driver to the Raspberry Pi

This has been ported to work with the ArduiPi Project. ArduiPi project is a shield for Raspberry Pi that brings Arduino low level extented I/O to Raspberry Pi.

This project is currently in progress and you can find all information about this shield and project here http://hallard.me/arduipi

Installation
============

Update your PI to latest version
--------------------------------

Be sure your Pi is up to date with the following commands

`sudo apt-get update  
sudo apt-get upgrade`

Requirement of developpement tools
----------------------------------

To be able to compile you will need the compiler and some others tools, issue a :

`sudo apt-get install build-essential git-core`

Requirement of bcm2835 library
------------------------------

To drive the Raspberry PI I/O ports I decided to use the small and pretty cool library bcm2835 from Mike
So you need to install this library before using the ArduiPi SSD1306 driver
you can find information on how to install this library and download link on Mike's Site
http://www.open.com.au/mikem/bcm2835/

Currently I am using the 1.22 version located here
http://www.open.com.au/mikem/bcm2835/bcm2835-1.22.tar.gz

Then simple as usual, extract, configure, compile :

`tar zxvf bcm2835-1.22.tar.gz`    
`cd bcm2835-1.22`     
`./configure`    
`make`   
`sudo make check`    
`sudo make install`   

Installation of SSD1306 Driver
------------------------------

The SSD1306 Driver is based on Adafruit Arduino library, I just ported the code to be able to compile and run on Raspberry Pi.

The driver is compiled into a library with all functions, including driving OLED with SPI or I2C and driving 128x32 and 128x64 LCD size.
The LCD size and driving mode is now on the new init function of the class, no need to compile with #define

Get all the file from github dedicated repo :

`git clone https://github.com/hallard/ArduiPi_SSD1306.git`

The buid install process is in the Makefile, so issue a:

`cd ArduiPi_SSD1306`  
`sudo make`  

The new build script will compile the library (libssd1306.a) and install it for you (this is why need sudo)
library in /usr/local/lib/ and library headers in /usr/local/include/

Fire the sample test code
-------------------------
The previous make will compile the sample code ssd1306 

Plug the OLED on your ArduiPi board, and you're in, launch the following command to use a SPI 128x32 OLED :
`./ssd1306_demo --spi -w 128 -t 32` 

For a I2C 128x64 OLED do a:  
`./ssd1306_demo -i2c -w 128 -t 64`


Using the SSD1306 Driver in your app
------------------------------------

Of course if you connect the OLED on Raspberry without ArduiPi board, you could change the pins on where is connected the Reset and D/C of the breakboard.
Theese definitions are located on the ArduiPi_SSD1306.h file and you need to recompile the library. 

But if you do not whant to do this, juste change the pin at the init stage of you code (explanation below)


If you need to add this library to your project do not forget to include the three file at the beginning of your code


`#include "ArduiPi_SSD1306.h"`  
`#include "Adafruit_GFX.h"`  
`#include "Adafruit_SSD1306.h"`  

Then instantiate the OLED display

`Adafruit_SSD1306 display;`  

And configure it (exmaple below) but look into the sample file ssd1306_demo.cpp

`display.init(OLED_SPI_DC,OLED_SPI_RESET,OLED_SPI_CS, 128, 32);`  
`display.begin(SSD1306_SWITCHCAPVCC);`  


And add the SSD1306 and bcm2835 libraries at compile time (see Makefile of this project) as this example below

`ssd1306_demo: ssd1306_demo.cpp`  
`	$(CC) $(CFLAGS) $^ -o $@ -Wl,--start-group -lbcm2835 -lssd1306 -Wl,--end-group`  
	
No not change these parameters since ssd1306 library depends on bcm2835 library, this command avoid link problems  	
`-Wl,--start-group -lbcm2835 -lssd1306 -Wl,--end-group`






