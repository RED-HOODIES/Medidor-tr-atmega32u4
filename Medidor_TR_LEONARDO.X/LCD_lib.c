/*
 * File:   main.c (PERSONALIZACI�N CARACTER)
 * 
 * Autores: Oscar David Poblador Parra      20211005116
 *          Juan David Bello Rodriguez      20211005028 
 *          Manuel Alejandro Guio Cardona   20211005061
 * Intrituci�n: Universidad Distrital Francisco Jos� de Caldas
 */

#include <xc.h>
#include "LCD_16x2.h"

static uint8_t _lcd_params;
static char _lcd_buffer_[_LCD_nCOL_+1];

void _write_nibble(uint8_t nibble);
void _send_byte(uint8_t value, uint8_t mode);

void _send_byte(uint8_t value, uint8_t mode){
	
	 switch(mode){
		 case 0: _LCD_PORT &= ~(1<<_LCD_RS); break;
		 case 1: _LCD_PORT |=  (1<<_LCD_RS); break;
	 }
	_write_nibble(value>>4);
	_write_nibble(value&0x0F);
}



void _write_nibble(uint8_t nibble){
	
	if( (nibble & 1) == 0) _LCD_PORT &=~(1<<_LCD_D4); else _LCD_PORT |= (1<<_LCD_D4);
	if( (nibble & 2) == 0) _LCD_PORT &=~(1<<_LCD_D5); else _LCD_PORT |= (1<<_LCD_D5);
	if( (nibble & 4) == 0) _LCD_PORT &=~(1<<_LCD_D6); else _LCD_PORT |= (1<<_LCD_D6);
	if( (nibble & 8) == 0) _LCD_PORT &=~(1<<_LCD_D7); else _LCD_PORT |= (1<<_LCD_D7);
	
	_LCD_PORT &= ~(1<<_LCD_EN);
	_LCD_PORT |=  (1<<_LCD_EN);
	_LCD_PORT &= ~(1<<_LCD_EN);
	
	_delay_ms(0.3);
	
}

void lcd_write(uint8_t letra){
	 _send_byte(letra,_DATA_);
}
void lcd_command(uint8_t cmd){
	 _send_byte(cmd,_COMMAND_);
}


void LCD_init(void){
	 
	 _LCD_DDR |= (1<<_LCD_RS
				 |1<<_LCD_EN	 
				 |1<<_LCD_D4
				 |1<<_LCD_D5
				 |1<<_LCD_D6
				 |1<<_LCD_D7);
	 _LCD_PORT &= ~(1<<_LCD_RS
				   |1<<_LCD_EN
				   |1<<_LCD_D4
				   |1<<_LCD_D5
				   |1<<_LCD_D6
				   |1<<_LCD_D7);
				   
	_delay_ms(15);
	_write_nibble(0x03);
	_delay_ms(4.1);
	_write_nibble(0x03);
	_delay_us(100);
	_write_nibble(0x03);
	_write_nibble(0x02);
	
	
	lcd_command( _LCD_FUNTIONSET | _LCD_4BITMODE | _LCD_2LINE |_LCD_5x8DOTS );
	_delay_us(37);
			
	_lcd_params = _LCD_DISPLAY_ON | _LCD_CURSOR_OFF | _LCD_BLINK_OFF;
	lcd_command(_LCD_DISPLAYCONTROL | _lcd_params);
	_delay_us(37);
	
	lcd_command(_LCD_CLEARDISPLAY);
	_delay_ms(2);
	
}

void  lcd_puts(char *str){
	while(*str){
		lcd_write(*str);
		str++;
	}	
}

void LCD_set_cursor(uint8_t row, uint8_t col){
	static uint8_t local_mem[4]={0x00,0x40,0x14,0x54};
	lcd_command(_LCD_SET_DDRAM_ADDR | (local_mem[row]+(col)) );
}

void LCD_printf(char *str, ...){
	
	va_list args;
	va_start(args,str);
	vsnprintf(_lcd_buffer_, _LCD_nCOL_+1, str, args);
	va_end(args);
	lcd_puts(_lcd_buffer_);
	
}

void LCD_clear(void){
	lcd_command(_LCD_CLEARDISPLAY);
	_delay_ms(2);
}
void lcd_return_home(void){
	lcd_command(_LCD_RETURNHOME);
	_delay_ms(2);
}

void lcd_on(void){
	_lcd_params |=  _LCD_DISPLAY_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}
void lcd_off(void){
	_lcd_params &=  ~_LCD_DISPLAY_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}

void lcd_enable_blink(void){
	_lcd_params |= _LCD_BLINK_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}
void lcd_disable_blink(void){
	_lcd_params &= ~_LCD_BLINK_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}

void lcd_enable_cursor(void){
	_lcd_params |= _LCD_CURSOR_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}
void lcd_disable_cursor(void){
	_lcd_params &= ~_LCD_CURSOR_ON;
	lcd_command(_LCD_DISPLAYCONTROL |_lcd_params);
	_delay_us(37);
}

void LCD_scroll_left(void){
	 lcd_command(_LCD_CURSORDISPLAYSHIFT | _LCD_DISPLAY_SHIFT | _LCD_MOVELEFT);
	 _delay_us(37);
}
void LCD_scroll_right(void){
	 lcd_command(_LCD_CURSORDISPLAYSHIFT | _LCD_DISPLAY_SHIFT | _LCD_MOVERIGHT);
	 _delay_us(37);
}

void lcd_custom_char(uint8_t mem, uint8_t *charmap){
	lcd_command(_LCD_SET_CGRAM_ADDR |  ((mem&0x07)<<3) );
	for(uint8_t i=0; i<=7; i++){
		lcd_write(charmap[i]);
	}
	lcd_command(_LCD_SET_DDRAM_ADDR);
	_delay_us(37);
}

void LCD_desplazar(char *text) {
    LCD_clear();
    int textLength = 0;
    while (text[textLength] != '\0') {
        textLength++;
    }
    
    for (int i = 0; i <= textLength; i++) {
        // Borra la pantalla antes de escribir el texto
        
        // Calcula la posici�n de inicio para el desplazamiento
        int startPosition = i;
        
        // Verifica si el texto a�n cabe en la pantalla
        if (startPosition + 16 > textLength) {
            startPosition = textLength - 16;
        }
        // Imprime la parte del texto que se ver� en la pantalla
        LCD_set_cursor(0, 0);
        LCD_printf(&text[startPosition]);
        
        // Espera un tiempo antes de desplazar
        _delay_ms(400);
    }
}