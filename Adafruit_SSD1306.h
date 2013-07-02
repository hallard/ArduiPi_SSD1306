/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution

02/18/2013 	Charles-Henri Hallard (http://hallard.me)
						Modified for compiling and use on Raspberry ArduiPi Board
						LCD size and connection are now passed as arguments on 
						the command line (no more #define on compilation needed)
						ArduiPi project documentation http://hallard.me/arduipi
 
*********************************************************************/

#include "./Adafruit_GFX.h"

#define BLACK 0
#define WHITE 1


/*=========================================================================
    SSDxxxx Common Displays
    -----------------------------------------------------------------------
		Common values to all displays
=========================================================================*/

//#define SSD_Command_Mode			0x80 	/* DC bit is 0 */ Seeed set C0 to 1 why ?
#define SSD_Command_Mode			0x00 	/* C0 and DC bit are 0 				 */
#define SSD_Data_Mode					0x40	/* C0 bit is 0 and DC bit is 1 */

#define SSD_Inverse_Display		0xA7

#define SSD_Display_Off				0xAE
#define SSD_Display_On				0xAF

#define SSD_Set_ContrastLevel	0x81

#define SSD_External_Vcc			0x01
#define SSD_Internal_Vcc			0x02


#define SSD_Activate_Scroll		0x2F
#define SSD_Deactivate_Scroll	0x2E

#define Scroll_Left						0x00
#define Scroll_Right					0x01

#define Scroll_2Frames		0x07
#define Scroll_3Frames		0x04
#define Scroll_4Frames		0x05
#define Scroll_5Frames		0x00
#define Scroll_25Frames		0x06
#define Scroll_64Frames		0x01
#define Scroll_128Frames	0x02
#define Scroll_256Frames	0x03

#define VERTICAL_MODE						01
#define PAGE_MODE								01
#define HORIZONTAL_MODE					02


/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SSD1306_DISPLAYALLON_RESUME	0xA4
#define SSD1306_DISPLAYALLON 				0xA5

#define SSD1306_Normal_Display	0xA6

#define SSD1306_SETDISPLAYOFFSET 		0xD3
#define SSD1306_SETCOMPINS 					0xDA
#define SSD1306_SETVCOMDETECT 			0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 	0xD5
#define SSD1306_SETPRECHARGE 				0xD9
#define SSD1306_SETMULTIPLEX 				0xA8
#define SSD1306_SETLOWCOLUMN 				0x00
#define SSD1306_SETHIGHCOLUMN 			0x10
#define SSD1306_SETSTARTLINE 				0x40
#define SSD1306_MEMORYMODE 					0x20
#define SSD1306_COMSCANINC 					0xC0
#define SSD1306_COMSCANDEC 					0xC8
#define SSD1306_SEGREMAP 						0xA0
#define SSD1306_CHARGEPUMP 					0x8D

// Scrolling #defines
#define SSD1306_SET_VERTICAL_SCROLL_AREA 							0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 							0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 								0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL		0x2A

/*=========================================================================
    SSD1308 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SSD1308_Normal_Display	0xA6

/*=========================================================================
    SSD1327 Displays
    -----------------------------------------------------------------------
    The driver is used in Seeed 96x96 display
=========================================================================*/
#define SSD1327_Normal_Display	0xA4


class Adafruit_SSD1306 : public Adafruit_GFX 
{
 public:
  Adafruit_SSD1306();

	// SPI Init
  boolean init(int8_t DC, int8_t RST, int8_t CS, uint8_t OLED_TYPE);
	
	// I2C Init
  boolean init(int8_t RST, uint8_t OLED_TYPE);

	boolean oled_is_spi_proto(uint8_t OLED_TYPE); /* to know protocol before init */
	boolean select_oled(uint8_t OLED_TYPE) ;
	
  void begin(void);
  void close(void);

  void ssd1306_command(uint8_t c);
  void ssd1306_command(uint8_t c0, uint8_t c1);
  void ssd1306_command(uint8_t c0, uint8_t c1, uint8_t c2);
  void ssd1306_data(uint8_t c);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void display();

  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);

  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);

  void drawPixel(int16_t x, int16_t y, uint16_t color);

	private:
	uint8_t *poledbuff;	// Pointer to OLED data buffer in memory
  int8_t _i2c_addr, dc, rst, cs;
	int16_t ssd1306_lcdwidth, ssd1306_lcdheight;
	uint8_t vcc_type;
	
	inline boolean isI2C(void);
	inline boolean isSPI(void);
  void fastSPIwrite(uint8_t c);
  void fastSPIwrite(char* tbuf, uint32_t len);
  void fastI2Cwrite(uint8_t c);
  void fastI2Cwrite(char* tbuf, uint32_t len);
  void slowSPIwrite(uint8_t c);

  //volatile uint8_t *dcport;
  //uint8_t dcpinmask;
};
