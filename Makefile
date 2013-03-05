#*********************************************************************
# This is the makefile for the Adafruit SSD1306 OLED library driver
#
#	02/18/2013 	Charles-Henri Hallard (http://hallard.me)
#							Modified for compiling and use on Raspberry ArduiPi Board
#							LCD size and connection are now passed as arguments on 
#							the command line (no more #define on compilation needed)
#							ArduiPi project documentation http://hallard.me/arduipi
#
# *********************************************************************


SHELL=/bin/sh
CFLAGS=-Wall -O2 -fno-rtti
CC=g++

# Libs needed
# you need to install bcm2835 library
# from http://www.open.com.au/mikem/bcm2835/
ALL_LIBS=-lbcm2835 -lssd1306

# As bcm2835 library is used into ssd1306 lib we need this options
LIBS=-Wl,--start-group $(ALL_LIBS) -Wl,--end-group

# Where you want it installed when you do 'make install'
PREFIX=/usr/local

all: Adafruit_SSD1306.o\
		Adafruit_GFX.o \
		libssd1306.a \
		ssd1306_demo

# ===== Compile Library
# Adafruit SSD1306 code 
Adafruit_SSD1306.o:Adafruit_SSD1306.cpp 
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@ 
# Adafruit GFX Code 
Adafruit_GFX.o:Adafruit_GFX.cpp 
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@ 

# ===== Link library
# library file for OLED
libssd1306.a:Adafruit_SSD1306.o Adafruit_GFX.o 
	ar rcs $@ $^  
	ranlib $@
# Install the library
	mv -f  $@ $(PREFIX)/lib
	chmod ugo-x $(PREFIX)/lib/$@
	ldconfig

# ===== Compile final executable with the library created
ssd1306_demo: ssd1306_demo.cpp 
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) 
	
# ===== Install
install: ssd1306_demo
	if ( test ! -d $(PREFIX)/bin ) ; then mkdir -p $(PREFIX)/bin ; fi
	if ( test ! -d $(PREFIX)/bin/arduipi ) ; then mkdir -p $(PREFIX)/bin/arduipi ; fi
	cp -f  *.h $(PREFIX)/include
	chmod ugo-x $(PREFIX)/include/*.h

clean: 
	rm -f *.o *.a ssd1306_demo




