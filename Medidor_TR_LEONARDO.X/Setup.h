/*
 * File:   main.c (PERSONALIZACIÓN CARACTER)
 * 
 * Autores: Oscar David Poblador Parra      20211005116
 *          Juan David Bello Rodriguez      20211005028 
 *          Manuel Alejandro Guio Cardona   20211005061
 * Intritución: Universidad Distrital Francisco José de Caldas
 */

#include <xc.h>
#include <avr/io.h>

#define F_CPU 16000000UL
#define BAUD 9600

//Para NPN VCC=9V, PNP VCC=-9V, Vemisor=0;
#define RC 1000
#define RB 100000
#define BOTON (PINB&(1 << PINB4))
#define BOTON2 (PINB&(1 << PINB5))

void SetupOscilador(void);
void Setupports(void);
void Setupint(void);
void SetupADC(void);
void Setup(void);


#ifndef SETUP_H
#define	SETUP_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* SETUP_H */



