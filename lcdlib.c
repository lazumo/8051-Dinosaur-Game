#include <8051.h>
#include "lcdlib.h"
__data __at (0x20) unsigned char lcd_ready;  /* @@@ change to a different location if needed. It just needs a bit, no need to be a char. */
#define DB7 P1_7
#define DB6 P1_6
#define DB5 P1_5
#define DB4 P1_4
#define DB  P1
#define RS P1_3
#define E P1_2

void delay(unsigned char n) ;
#define DELAY_AMOUNT 40
unsigned char LCD_ready(void) {
    return lcd_ready;
}
void LCD_Init(void) {
	LCD_functionSet();
	LCD_entryModeSet(0, 0); /* increment and no shift */
	LCD_displayOnOffControl(1, 1, 1); /* display on, cursor on and blinking on */
	lcd_ready = 1;
}
void LCD_IRWrite(char c) {
	lcd_ready = 0;
	DB = (c & 0xf0); // high nibble, keep RS low
	E = 1;  // pulse E
	E = 0;
	DB = (c << 4); // low nibble, keep RS low
	E = 1;
	E = 0;
	delay(DELAY_AMOUNT);
	lcd_ready = 1;
}
void LCD_functionSet(void) {
	lcd_ready = 0;
	// The high nibble for the function set is actually sent twice
	// because this is how 4-bit mode works for the HD44780 controller.
	DB = 0x20;  // DB<7:4> = 0010, <RS,E,x,x>=0
	E = 1;
	E = 0;
	delay(DELAY_AMOUNT);
	E = 1;
	E = 0;
	delay(DELAY_AMOUNT); // added, to ensure sufficient delay
	DB7 = 1; // 2-line model
	// DB6 defaults to 0 = 5x7, DB5, DB4 are don't-cares
	E = 1;
	E = 0;
	delay(DELAY_AMOUNT);
	lcd_ready = 1;
}
// --------------------------------------------------------------------
void LCD_write_char(char c) {
	lcd_ready = 0;
	DB = (c & 0xf0) | 0x08; //; keep the RS
	RS = 1;
	E = 1;
	E = 0;
	DB = (c << 4) | 0x08; // keep the RS
	E = 1;
	E = 0;
	delay(DELAY_AMOUNT);
	lcd_ready = 1;
}
void LCD_write_string(char* str) {
	while (*str++) {
		LCD_write_char(*str);
	 }
}
void delay(unsigned char n) {
	{ __asm
	dhere:
		djnz dpl, dhere
	__endasm;
	}
}

void LCD_set_symbol1() {
// or it could be defined as a macro.
// in any case, it takes the following calls:

    LCD_setCgRamAddress(0x08); // code is the character generation memory 

              // (CG RAM address for the bitmap. 
              //  write '\1' for the dinosaur, '\2' for the cactus.

	//{0x07, 0x05, 0x06, 0x07, 0x14, 0x17, 0x0E, 0x0A};
    LCD_write_char(0x07);//high
    LCD_write_char(0x05);
    LCD_write_char(0x06);
    LCD_write_char(0x07);
    LCD_write_char(0x14);
    LCD_write_char(0x17);
    LCD_write_char(0x0E);
    LCD_write_char(0x0A);//low
   // you need to write all 8 bytes for each bitmap.
   // of course, you could do it in a loop or do pointer arithmetic
}

//0x04, 0x05, 0x15, 0x15, 0x16, 0x0C, 0x04, 0x04
void LCD_set_symbol2() {
// or it could be defined as a macro.
// in any case, it takes the following calls:

    LCD_setCgRamAddress(0x10); // code is the character generation memory 

              // (CG RAM address for the bitmap. 
              //  write '\1' for the dinosaur, '\2' for the cactus.

	//{0x07, 0x05, 0x06, 0x07, 0x14, 0x17, 0x0E, 0x0A};
    LCD_write_char(0x04);//high
    LCD_write_char(0x05);
    LCD_write_char(0x15);
    LCD_write_char(0x15);
    LCD_write_char(0x16);
    LCD_write_char(0x0C);
    LCD_write_char(0x04);
    LCD_write_char(0x04);//low
   // you need to write all 8 bytes for each bitmap.
   // of course, you could do it in a loop or do pointer arithmetic
}
