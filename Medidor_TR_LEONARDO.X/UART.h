/*
 * File:   main.c (PERSONALIZACIÓN CARACTER)
 * 
 * Autores: Oscar David Poblador Parra      20211005116
 *          Juan David Bello Rodriguez      20211005028 
 *          Manuel Alejandro Guio Cardona   20211005061
 * Institución: Universidad Distrital Francisco José de Caldas
 */

#include <stdio.h>
#include <string.h>
#include <xc.h>
#include <avr/io.h>

#define SALTO (USART_transmit('\r'))


void UART_init();							//función para iniciar el USART AVR asíncrono, 8 bits, 9600 baudios,
unsigned char UART_read();					//función para la recepción de caracteres
void UART_write(unsigned char);				//función para la transmisión de caracteres
int USART_printCHAR(char character, FILE *stream);

static FILE USART_0_stream = FDEV_SETUP_STREAM(USART_printCHAR, NULL, _FDEV_SETUP_WRITE);

int USART_printCHAR(char character, FILE *stream)
{
	UART_write(character);	
	return 0;

}


void UART_init()
{
    
	UCSR1A = (0<<TXC1)|(0<<U2X1)|(0<<MPCM1);  //banderas
	UCSR1B = (1<<RXCIE1)|(0<<TXCIE1)|(0<<UDRIE1)|(1<<RXEN1)|(1<<TXEN1)|(0<<UCSZ12)|(0<<TXB81);// 
	UCSR1C = (0<<UMSEL11)|(0<<UMSEL10)|(0<<UPM11)|(0<<UPM10)|(0<<USBS1)|(1<<UCSZ11)|(1<<UCSZ10)|(0<<UCPOL1);
   // UCSR1D = (0<<CTSEN) || (0<<RTSEN);
    
    UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	
	stdout = &USART_0_stream;
	//UBRR1 = 51;							//NO DUPLICA VELOCIDAD 9600B A 160000
}

unsigned char UART_read(){
	/*if(UCSR1A&(1<<7)){			//si el bit7 del registro UCSR0A se ha puesto a 1
		return UDR1;			//devuelve el dato almacenado en el registro UDR0
	}
	else
	return 0;*/
    /* Wait for data to be received */
    while ( !(UCSR1A & (1<<RXC1)) )
    ;
    /* Get and return received data from buffer */
    return UDR1;
}

void UART_write(unsigned char caracter){
	while(!(UCSR1A&(1<<5)));    // mientras el registro UDR0 esté lleno espera
	UDR1 = caracter;            //cuando el el registro UDR0 está vacio se envia el caracter
}

void UART_write_txt(char* cadena){			//cadena de caracteres de tipo char
	while(*cadena !=0x00){				//mientras el último valor de la cadena sea diferente a el caracter nulo
		UART_write(*cadena);			//transmite los caracteres de cadena
		cadena++;						//incrementa la ubicación de los caracteres en cadena
										//para enviar el siguiente caracter de cadena
	}
}

void USART_transmit(uint8_t data) {
    // Transmitir un byte a través de la UART
    while (!(UCSR1A & (1 << UDRE1))); // Esperar a que el registro de datos esté vacío
    UDR1 = data; // Transmitir el byte
}



