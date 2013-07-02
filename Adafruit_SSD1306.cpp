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
All text above, and the splash screen below must be included in any redistribution

02/18/2013 	Charles-Henri Hallard (http://hallard.me)
						Modified for compiling and use on Raspberry ArduiPi Board
						LCD size and connection are now passed as arguments on 
						the command line (no more #define on compilation needed)
						ArduiPi project documentation http://hallard.me/arduipi
07/01/2013 	Charles-Henri Hallard 
						Reduced code size removed the Adafruit Logo (sorry guys)
						Buffer for OLED is now dynamic to LCD size
						Added support of Seeed OLED 64x64 Display

*********************************************************************/

#include "./ArduiPi_SSD1306.h" 
#include "./Adafruit_GFX.h"
#include "./Adafruit_SSD1306.h"

  
inline boolean Adafruit_SSD1306::isSPI(void) {
	return (cs != -1 ? true : false);
}
inline boolean Adafruit_SSD1306::isI2C(void) {
	return (cs == -1 ? true : false);
}
// Low level I2C and SPI Write function
inline void Adafruit_SSD1306::fastSPIwrite(uint8_t d) {
	bcm2835_spi_transfer(d);
}
inline void Adafruit_SSD1306::fastI2Cwrite(uint8_t d) {
	bcm2835_spi_transfer(d);
}
inline void Adafruit_SSD1306::fastSPIwrite(char* tbuf, uint32_t len) {
	bcm2835_spi_writenb(tbuf, len);
}
inline void Adafruit_SSD1306::fastI2Cwrite(char* tbuf, uint32_t len) {
	bcm2835_i2c_write(tbuf, len);
}

// the most basic function, set a single pixel
void Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	uint8_t * p = poledbuff ;
	
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

		// check rotation, move pixel around if necessary
	switch (getRotation()) 
	{
		case 1:
			swap(x, y);
			x = WIDTH - x - 1;
		break;
		
		case 2:
			x = WIDTH - x - 1;
			y = HEIGHT - y - 1;
		break;
		
		case 3:
			swap(x, y);
			y = HEIGHT - y - 1;
		break;
	}  

	// Get where to do the change in the buffer
	p = poledbuff + (x + (y/8)*ssd1306_lcdwidth );
	
	// x is which column
	if (color == WHITE) 
		*p |=  _BV((y%8));  
	else
		*p &= ~_BV((y%8)); 
}

// Display instantiation
Adafruit_SSD1306::Adafruit_SSD1306() 
{
	// Init all var, and clean
	// Command I/O
  rst = 0 ;
  dc  = 0 ;
  cs =  0 ;
	
	// Lcd size
  ssd1306_lcdwidth  = 0;
  ssd1306_lcdheight = 0;
	
	// Empty pointer to OLED buffer
	poledbuff = NULL;
}


// When not initialized program using this library may
// know protocol for correct init call, he could just know
// oled number in driver list
boolean Adafruit_SSD1306::oled_is_spi_proto(uint8_t OLED_TYPE) 
{
	switch (OLED_TYPE)
	{
		case OLED_ADAFRUIT_SPI_128x32:
		case OLED_ADAFRUIT_SPI_128x64:
			return true;
		break;
	}
		
	// default 
	return false;
	
}

// initializer for OLED Type
boolean Adafruit_SSD1306::select_oled(uint8_t OLED_TYPE) 
{
	// Default type
	ssd1306_lcdwidth  = 128;
	ssd1306_lcdheight = 64;
	_i2c_addr = 0x00;
	
	// default OLED are using internal boost VCC converter
	vcc_type = SSD_Internal_Vcc;

	// Oled supported display
	// Setup size and I2C address
	switch (OLED_TYPE)
	{
		case OLED_ADAFRUIT_SPI_128x32:
			ssd1306_lcdheight = 32;
		break;

		case OLED_ADAFRUIT_SPI_128x64:
		;
		break;
		
		case OLED_ADAFRUIT_I2C_128x32:
			ssd1306_lcdheight = 32;
			_i2c_addr = ADAFRUIT_I2C_ADDRESS;
		break;

		case OLED_ADAFRUIT_I2C_128x64:
			_i2c_addr = ADAFRUIT_I2C_ADDRESS;
		break;
		
		case OLED_SEEED_I2C_128x64:
			_i2c_addr = SEEEED_I2C_ADDRESS ;
			vcc_type = SSD_External_Vcc;
		break;

		case OLED_SEEED_I2C_96x96:
			ssd1306_lcdwidth  = 96;
			ssd1306_lcdheight = 96;
			_i2c_addr = SEEEED_I2C_ADDRESS ;
		break;
		
		// houston, we have a problem
		default:
			return false;
		break;
	}

	// De-Allocate memory for OLED buffer if any
	if (poledbuff)
		free(poledbuff);
		
	// Allocate memory for OLED buffer
	poledbuff = (uint8_t *) malloc ( (ssd1306_lcdwidth * ssd1306_lcdheight / 8 )); 
	if (!poledbuff)
    return false;

	// Init Raspberry PI GPIO
  if (!bcm2835_init())
    return false;
		
	return true;
	
}

// initializer for SPI - we indicate the pins used and OLED type
//
boolean Adafruit_SSD1306::init(int8_t DC, int8_t RST, int8_t CS, uint8_t OLED_TYPE) 
{
  rst = RST;	// Reset Pin
  dc = DC;		// Data / command Pin
	cs = CS ;		// Raspberry SPI chip Enable (may be CE0 or CE1)
	
	// Select OLED parameters
	if (!select_oled(OLED_TYPE))
		return false;

	// Init & Configure Raspberry PI SPI
	bcm2835_spi_begin(cs);
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                
	
	// 16 MHz SPI bus, but Worked at 62 MHz also	
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16); 

	// Set the pin that will control DC as output
	bcm2835_gpio_fsel(dc, BCM2835_GPIO_FSEL_OUTP);

  // Setup reset pin direction as output
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);

	return ( true);
}

// initializer for I2C - we only indicate the reset pin and OLED type !
boolean Adafruit_SSD1306::init(int8_t RST, uint8_t OLED_TYPE) 
{
  dc = cs = -1; // DC and chip Select do not exist in I2C
  rst = RST;

	// Select OLED parameters
	if (!select_oled(OLED_TYPE))
		return false;

	// Init & Configure Raspberry PI I2C
	if (bcm2835_i2c_begin()==0)
		return false;
		
	bcm2835_i2c_setSlaveAddress(_i2c_addr) ;
		
	// Set clock to 400 KHz
	// does not seem to work, will check this later
	// bcm2835_i2c_set_baudrate(400000);

  // Setup reset pin direction as output
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);
	
	return ( true);
}

void Adafruit_SSD1306::close(void) 
{
	// De-Allocate memory for OLED buffer if any
	if (poledbuff)
		free(poledbuff);
		
	poledbuff = NULL;

	// Release Raspberry SPI
	if ( isSPI() )
		bcm2835_spi_end();

		// Release Raspberry I2C
	if ( isI2C() )
		bcm2835_i2c_end();

	// Release Raspberry I/O control
  bcm2835_close();
}

	
void Adafruit_SSD1306::begin( void ) 
{
  uint8_t multiplex;
  uint8_t chargepump;
  uint8_t compins;
  uint8_t contrast;
  uint8_t precharge;
	
	constructor(ssd1306_lcdwidth, ssd1306_lcdheight);

  // Setup reset pin direction (used by both SPI and I2C)  
  bcm2835_gpio_fsel(rst, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(rst, HIGH);
	
  // VDD (3.3V) goes high at start, lets just chill for a ms
  usleep(1000);
  
	// bring reset low
  bcm2835_gpio_write(rst, LOW);
	
  // wait 10ms
  usleep(10000);
  
	// bring out of reset
  bcm2835_gpio_write(rst, HIGH);
	
	// depends on OLED type configuration
	if (ssd1306_lcdheight == 32)
	{
		multiplex = 0x1F;
		compins 	= 0x02;
		contrast	= 0x8F;
	}
	else
	{
		multiplex = 0x3F;
		compins 	= 0x12;
		contrast	= (vcc_type==SSD_External_Vcc?0x9F:0xCF);
	}
	
	if (vcc_type == SSD_External_Vcc)
	{
		chargepump = 0x10; 
		precharge  = 0x22;
	}
	else
	{
		chargepump = 0x14; 
		precharge  = 0xF1;
	}
	
	ssd1306_command(SSD_Display_Off);                    // 0xAE
	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV, 0x80);      // 0xD5 + the suggested ratio 0x80
	ssd1306_command(SSD1306_SETMULTIPLEX, multiplex); 
	ssd1306_command(SSD1306_SETDISPLAYOFFSET, 0x00);        // 0xD3 + no offset
	ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	ssd1306_command(SSD1306_CHARGEPUMP, chargepump); 
	ssd1306_command(SSD1306_MEMORYMODE, 0x00);              // 0x20 0x0 act like ks0108
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
	ssd1306_command(SSD1306_COMSCANDEC);
	ssd1306_command(SSD1306_SETCOMPINS, compins);  // 0xDA
	ssd1306_command(SSD_Set_ContrastLevel, contrast);
	ssd1306_command(SSD1306_SETPRECHARGE, precharge); // 0xd9
	ssd1306_command(SSD1306_SETVCOMDETECT, 0x40);  // 0xDB
	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);    // 0xA4
	ssd1306_command(SSD1306_Normal_Display);         // 0xA6

	// Reset to default value in case of 
	// no reset pin available on OLED
	ssd1306_command( 0x21, 0, 127 ); 
	ssd1306_command( 0x22, 0,   7 ); 
	stopscroll();
	
	// Empty uninitialized buffer
	clearDisplay();
	ssd1306_command(SSD_Display_On);							//--turn on oled panel
}


void Adafruit_SSD1306::invertDisplay(uint8_t i) 
{
  if (i) 
    ssd1306_command(SSD_Inverse_Display);
  else 
    ssd1306_command(SSD1306_Normal_Display);
}

void Adafruit_SSD1306::ssd1306_command(uint8_t c) 
{ 
	// Is SPI
  if (isSPI())
  {
		// Setup D/C line to low to switch to command mode
	  bcm2835_gpio_write(dc, LOW);

		// Write Data on SPI
    fastSPIwrite(c);
  }
  // so I2C
  else
  {
		char buff[2] ;
		
		// Clear D/C to switch to command mode
		buff[0] = SSD_Command_Mode ; 
		buff[1] = c;
		
		// Write Data on I2C
		fastI2Cwrite(buff, sizeof(buff))	;
  }
}

void Adafruit_SSD1306::ssd1306_command(uint8_t c0, uint8_t c1) 
{ 
	char buff[3] ;
	buff[1] = c0;
	buff[2] = c1;

	// Is SPI
  if (isSPI())
  {
		// Setup D/C line to low to switch to command mode
	  bcm2835_gpio_write(dc, LOW);

		// Write Data
    fastSPIwrite(&buff[1], 2);
  }
  // I2C
  else
  {
		// Clear D/C to switch to command mode
		buff[0] = SSD_Command_Mode ;

		// Write Data on I2C
		fastI2Cwrite(buff, 3)	;
  }
}

void Adafruit_SSD1306::ssd1306_command(uint8_t c0, uint8_t c1, uint8_t c2) 
{ 
	char buff[4] ;
		
	buff[1] = c0;
	buff[2] = c1;
	buff[3] = c2;

	// Is SPI
  if (isSPI())
  {
		// Setup D/C line to low to switch to command mode
	  bcm2835_gpio_write(dc, LOW);

		// Write Data
    fastSPIwrite(&buff[1], 3);
  }
  // I2C
  else
  {
		// Clear D/C to switch to command mode
		buff[0] = SSD_Command_Mode; 

		// Write Data on I2C
		fastI2Cwrite(buff, sizeof(buff))	;
  }
}


// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop)
{
	ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(0XFF);
	ssd1306_command(SSD_Activate_Scroll);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1306::startscrollleft(uint8_t start, uint8_t stop)
{
	ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(0XFF);
	ssd1306_command(SSD_Activate_Scroll);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop)
{
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	ssd1306_command(0X00);
	ssd1306_command(ssd1306_lcdheight);
	ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD_Activate_Scroll);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Adafruit_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop)
{
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	ssd1306_command(0X00);
	ssd1306_command(ssd1306_lcdheight);
	ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD_Activate_Scroll);
}

void Adafruit_SSD1306::stopscroll(void)
{
	ssd1306_command(SSD_Deactivate_Scroll);
}

void Adafruit_SSD1306::ssd1306_data(uint8_t c) 
{
	// SPI
  if ( isSPI())
  {
    // SPI
		// Setup D/C line to high to switch to data mode
	  bcm2835_gpio_write(dc, HIGH);

		// write value
    fastSPIwrite(c);
  }
  // I2C
  else
  {
		char buff[2] ;
		
		// Setup D/C to switch to data mode
		buff[0] = SSD_Data_Mode; 
		buff[1] = c;

		// Write on i2c
		fastI2Cwrite(	buff, sizeof(buff))	;
  }
}

void Adafruit_SSD1306::display(void) 
{
  ssd1306_command(SSD1306_SETLOWCOLUMN  | 0x0); // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0); // hi col = 0
  ssd1306_command(SSD1306_SETSTARTLINE  | 0x0); // line #0

	uint16_t i=0 ;
	
	// pointer to OLED data buffer
	uint8_t * p = poledbuff;

	// SPI
  if ( isSPI())
  {
		// Setup D/C line to high to switch to data mode
	  bcm2835_gpio_write(dc, HIGH);

		// Send all data to OLED
    for ( i=0; i<(ssd1306_lcdwidth*ssd1306_lcdheight/8); i++) 
		{
      fastSPIwrite(*p++);
    }

    // i wonder why we have to do this (check datasheet)
    if (ssd1306_lcdheight == 32) 
		{
      for (uint16_t i=0; i<(ssd1306_lcdwidth*ssd1306_lcdheight/8); i++) 
			{
        fastSPIwrite(0);
      }
    }
		
  }
  // I2C
  else
  {
		char buff[17] ;
		uint8_t x ;

		// Setup D/C to switch to data mode
		buff[0] = SSD_Data_Mode; 

		// loop trough all OLED buffer and 
    // send a bunch of 16 data byte in one xmission
    for ( i=0; i<(ssd1306_lcdwidth*ssd1306_lcdheight/8); i+=16 ) 
		{
      for (x=1; x<=16; x++) 
				buff[x] = *p++;

			fastI2Cwrite(	buff,  17);
    }
  }
}

// clear everything (in the buffer)
void Adafruit_SSD1306::clearDisplay(void) 
{
  memset(poledbuff, 0, (ssd1306_lcdwidth*ssd1306_lcdheight/8));
}

