/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32|64 size display using SPI or I2C to communicate
4 or 5 pins are required to interface

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

#include "ArduiPi_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#include <getopt.h>

#define PRG_NAME        "ssd1306_demo"
#define PRG_VERSION     "1.0"
#define DEF_WIDTH				128 /* default OLED width  */
#define DEF_HEIGHT			32  /* default OLED height */

// Instantiate the display
Adafruit_SSD1306 display;


enum bus_e 		{  MODE_SPI, MODE_I2C };

// Config Option
struct s_opts
{
	int mode;
	int width;
	int height;
	int verbose;
} ;

// default options values
s_opts opts = {
	MODE_SPI,		// SPI by default
	DEF_WIDTH,	// 128 pixels width
	DEF_HEIGHT,	// 32 pixels height
  false			// Not verbose
};

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static unsigned char logo16_glcd_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random() % display.width();
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
    
    printf("x: %d", icons[f][XPOS]);
    printf("y: %d", icons[f][YPOS]);
    printf("dy: %d\n", icons[f][DELTAY]);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    usleep(200000);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
	icons[f][XPOS] = random() % display.width();
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.print("\n");
  }    
  display.display();
}

void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  usleep(250000);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  usleep(250000);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  usleep(250000);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  usleep(250000);
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.print("scroll");
  display.display();
 
  display.startscrollright(0x00, 0x0F);
  sleep(2);
  display.stopscroll();
  sleep(1);
  display.startscrollleft(0x00, 0x0F);
  sleep(2);
  display.stopscroll();
  sleep(1);    
  display.startscrolldiagright(0x00, 0x07);
  sleep(2);
  display.startscrolldiagleft(0x00, 0x07);
  sleep(2);
  display.stopscroll();
}


/* ======================================================================
Function: usage
Purpose : display usage
Input 	: program name
Output	: -
Comments: 
====================================================================== */
void usage( char * name)
{
	printf("%s\n", name );
	printf("Usage is: %s --i2c|spi --witdh --height [options]\n", name);
	printf("  --<i>2c    :  OLED is SPI\n");
	printf("  --<s>pi    :  OLED is I2C\n");
	printf("  --<width>  :  OLED width\n");
	printf("  --heigh<t> :  OLED height\n");
	printf("Options are:\n");
	printf("  --<v>erbose  : speak more to user\n");
	printf("  --<h>elp\n");
	printf("<?> indicates the equivalent short option.\n");
	printf("Short options are prefixed by \"-\" instead of by \"--\".\n");
	printf("Example :\n");
	printf( "%s -s -w 128 -t 32 use a SPI 128x32 OLED\n\n", name);
	printf( "%s -i -w 128 -t 64 use a I2C 128x64 OLED\n", name);
}


/* ======================================================================
Function: parse_args
Purpose : parse argument passed to the program
Input 	: -
Output	: -
Comments: 
====================================================================== */
void parse_args(int argc, char *argv[])
{
	static struct option longOptions[] =
	{
		{"i2c"		, no_argument,	  	0, 'i'},
		{"spi"		, no_argument,	  	0, 's'},
		{"verbose", no_argument,	  	0, 'v'},
		{"width"	, required_argument,0, 'w'},
		{"height"	, required_argument,0, 't'},
		{"help"		, no_argument, 			0, 'h'},
		{0, 0, 0, 0}
	};

	int optionIndex = 0;
	int c;

	while (1) 
	{
		/* no default error messages printed. */
		opterr = 0;

    c = getopt_long(argc, argv, "isvhw:t:", longOptions, &optionIndex);

		if (c < 0)
			break;

		switch (c) 
		{
			case 'v': opts.verbose = true	;	break;
			case 'i': opts.mode = MODE_I2C	;	break;
			case 's': opts.mode = MODE_SPI ;	break;

			case 'w':
				opts.width = (int) atoi(optarg);
				
				if (opts.width != DEF_WIDTH )
				{
						fprintf(stderr, "--width '%d' ignored.\n", opts.width);
						fprintf(stderr, "--width must be 128 for now\n");
						opts.width = DEF_WIDTH;
				}
			break;

			case 't':
				opts.height = (int) atoi(optarg);
				
				if (opts.height != 32 && opts.height != 63  )
				{
						fprintf(stderr, "--height '%d' ignored.\n", opts.height);
						fprintf(stderr, "--height must be 32 or 64 for now\n");
						opts.height = DEF_HEIGHT;
				}
			break;

			case 'h':
				usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
			default:
				fprintf(stderr, "Unrecognized option.\n");
				fprintf(stderr, "Run with '--help'.\n");
				exit(EXIT_FAILURE);
		}
	} /* while */

	if (opts.verbose)
	{
		printf("%s v%s\n", PRG_NAME, PRG_VERSION);
		printf("-- OLED params -- \n");
		printf("mode is    : %s\n", opts.mode == MODE_SPI ? "SPI" : "I2C");
		printf("width is   : %d\n", opts.width);
		printf("height is  : %d\n", opts.height);
		printf("-- Other Stuff -- \n");
		printf("verbose is : %s\n", opts.verbose? "yes" : "no");
		printf("\n");
	}	
}


/* ======================================================================
Function: main
Purpose : Main entry Point
Input 	: -
Output	: -
Comments: 
====================================================================== */
int main(int argc, char **argv)
{
	// Get OLED type
	parse_args(argc, argv);

	// SPI
	if (opts.mode == MODE_SPI )
	{
		// SPI LCD 128x32 size, change parameters to fit to your LCD
		if ( !display.init(OLED_SPI_DC,OLED_SPI_RESET,OLED_SPI_CS, opts.width, opts.height) )
			exit(EXIT_FAILURE);
			
		// SPI start : by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
		display.begin(SSD1306_SWITCHCAPVCC);
	}

	// I2C
	if (opts.mode == MODE_I2C )
	{
		// SPI LCD 128x32 size, change parameters to fit to your LCD
		if ( !display.init(OLED_I2C_RESET,opts.width, opts.height) )
			exit(EXIT_FAILURE);
			
		// by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
		if (opts.height == 32 )
			// initialize with the I2C addr 0x3C (for the 128x32)
			display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
		else
			// initialize with the I2C addr 0x3D (for the 128x64)
			display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  
	}

	
  // init done
  display.display(); // show splashscreen
  sleep(2);
  display.clearDisplay();   // clears the screen and buffer

	// text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("Hello, world!\n");
  display.setTextColor(BLACK, WHITE); // 'inverted' text
  display.printf("%f\n", 3.141592);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.printf("0x%8X\n", 0xDEADBEEF);
  display.display();
  sleep(5); 

  // draw a single pixel
  display.drawPixel(10, 10, WHITE);
  display.display();
  sleep(2);
  display.clearDisplay();
	
  // draw many lines
  testdrawline();
  display.display();
  sleep(2);
  display.clearDisplay();

  // draw rectangles
  testdrawrect();
  display.display();
  sleep(2);
  display.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display.display();
  sleep(2);
  display.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  display.display();
  sleep(2);
  display.clearDisplay();

  // draw a white circle, 10 pixel radius
  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);
  display.display();
  sleep(2);
  display.clearDisplay();

  testdrawroundrect();
  sleep(2);
  display.clearDisplay();

  testfillroundrect();
  sleep(2);
  display.clearDisplay();

  testdrawtriangle();
  sleep(2);
  display.clearDisplay();
   
  testfilltriangle();
  sleep(2);
  display.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display.display();
  sleep(2);
  display.clearDisplay();

  // draw scrolling text
  testscrolltext();
  sleep(2);
  display.clearDisplay();


  // miniature bitmap display
  display.clearDisplay();
  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display.display();

  // invert the display
  display.invertDisplay(true);
  sleep(1); 
  display.invertDisplay(false);
  sleep(1); 

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);

	// Free PI GPIO ports
	display.close();

}


