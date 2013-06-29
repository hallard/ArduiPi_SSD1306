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

Everything is now documented on my blog
http://hallard.me/adafruit-ssd1306-oled-display-driver-for-raspberry-pi/






