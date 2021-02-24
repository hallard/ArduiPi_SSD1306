#*********************************************************************
# This is the makefile for the Adafruit SSD1306 OLED library driver
#
#	02/18/2013 	Charles-Henri Hallard (http://hallard.me)
#							Modified for compiling and use on Raspberry ArduiPi Board
#							LCD size and connection are now passed as arguments on 
#							the command line (no more #define on compilation needed)
#							ArduiPi project documentation http://hallard.me/arduipi
#
#       2/23/2021       Kamil Piekutowski (https://github.com/KamilPiekutowski)
#                                                       Modified for compiling with Yocto.
#
# *********************************************************************

# Where you want it installed when you do 'make install'
PREFIX=$(DESTDIR)/usr

# Library parameters
# where to put the lib
LIBDIR=$(PREFIX)/lib
# lib name 
LIB=libssd1306
# shared library name
LIBNAME=$(LIB).so.1.0

# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast 

# make all
# reinstall the library after each recompilation
all: libssd1306

# Make the library
libssd1306: Adafruit_SSD1306.o Adafruit_GFX.o bcm2835.o 
	$(CC) -shared -Wl,-soname,$@.so.1 ${CCFLAGS}  -o ${LIBNAME} $^

# Library parts (use -fno-rtti flag to avoid link problem)
Adafruit_SSD1306.o: Adafruit_SSD1306.cpp
	$(CC) -Wall -fPIC -fno-rtti ${CCFLAGS} -c $^

Adafruit_GFX.o: Adafruit_GFX.cpp
	$(CC) -Wall -fPIC -fno-rtti ${CCFLAGS} -c $^

bcm2835.o: bcm2835.c
	$(CC) -Wall -fPIC ${CCFLAGS} -c $^

# Install the library to LIBPATH
install: 
	@if ( test ! -d $(PREFIX)/lib ) ; then mkdir -p $(PREFIX)/lib ; fi
	@if ( test ! -d $(PREFIX)/include ) ; then mkdir -p $(PREFIX)/include ; fi
	@cp -f Adafruit_*.h $(PREFIX)/include
	@cp -f ArduiPi_*.h $(PREFIX)/include
	@cp -f bcm2835.h $(PREFIX)/include
	
	
# Uninstall the library 
uninstall: 
	@echo "[Uninstall Library]"
	@rm -f ${LIBDIR}/${LIB}.*

	@echo "[Uninstall Headers]"
	@rm -rf  $(PREFIX)/include/Adafruit_SSD1306*
	@rm -rf  $(PREFIX)/include/ArduiPi_SSD1306*
	@rm -rf  $(PREFIX)/include/bcm2835.h
	
# clear build files
clean:
	rm -rf *.o ${LIB}.* ${LIBDIR}/${LIB}.*
