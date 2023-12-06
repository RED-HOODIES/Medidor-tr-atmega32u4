/*
 * File:   main.c (PERSONALIZACIÓN CARACTER)
 * 
 * Autores: Oscar David Poblador Parra      20211005116
 *          Juan David Bello Rodriguez      20211005028 
 *          Manuel Alejandro Guio Cardona   20211005061
 * Intritución: Universidad Distrital Francisco José de Caldas
 */

#include <xc.h>
#include "Setup.h"


void SetupOscilador(void)
{
    CLKPR = (1 << CLKPCE);
    // Configura el prescaler a 0 para funcionar a la máxima velocidad
    CLKPR = 0;
return;}

void Setupports(void)
{   
    DDRB = 0b00001111; //Configura pines PB0,PB1, PB2 y PB3 como salida  // o para encender 0b00001000;
    PORTB |= (1 << PORTB4); //Habilita el pull up del PD5
    PORTB |= (1 << PORTB5); //Habilita el pull up del PD5
return;}

void Setupint(void)
{
    
    return;}

void SetupADC (void)
{
     
    ADMUX |=  (1<<REFS0);		//Seleccionar el voltaje de referencia
	ADMUX &=~ (1<<REFS1);
	ADMUX &=~ (1<<ADLAR);		//Ajustar el resultado

	ADCSRA |= (1<<ADPS0);		// divisor = 128  16000/128 = 125 KHz
	ADCSRA |= (1<<ADPS1);
	ADCSRA |= (1<<ADPS2);
	ADCSRA |= (1<<ADEN);		// Encendemos en ADC
    
    
    return;}


void Setup(void)
{   
    SetupOscilador();
    Setupports();
    Setupint();
    SetupADC();
    
return;}


