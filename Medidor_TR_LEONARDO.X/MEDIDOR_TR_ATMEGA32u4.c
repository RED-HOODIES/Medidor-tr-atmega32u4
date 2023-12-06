/*
 * File:   main.c (PERSONALIZACIÓN CARACTER)
 * 
 * Autores: Oscar David Poblador Parra      20211005116
 *          Juan David Bello Rodriguez      20211005028 
 *          Manuel Alejandro Guio Cardona   20211005061
 * Intritución: Universidad Distrital Francisco José de Caldas
 */

#include <stdio.h>
#include "Setup.h"
#include "Config.h"
#include <util/delay.h>
#include <util/setbaud.h>
#include "LCD_16x2.h"

#define F_CPU 16000000UL

char str[10];
uint16_t dato_ADC;

int S=1; //
int Vbase=0,Vcolector=0,Vce=0; //Vbase en ADC0 y Vcolector en ADC1
double VCE_mostrar=0;
int Vbase_aux=0,Vcolector_aux=0;
int VRbase=0, VRcolector=0;
int SALIDA=0; //1 si es PNP, 2 si es NPN y 3 si está dañado
double hfe=0,VAF=0; //Beta y voltaje early
double VRBASE=0,VRCOLECTOR=0,VBASE=0,VCOLECTOR=0; //Voltajes convertidos
double VCE1=0,VCE2=0,IC1=0,IC2=0;
double ro=0;

char disbf[20];
char imprimir[40];

int DEBOUNCER1() {
    if (BOTON) { // Verificar si el botón está presionado (lógica negada debido al resistor pull-up)
        _delay_ms(10); // Pequeño retardo para evitar rebotes
        if (BOTON) { // Verificar nuevamente después del retardo
            return 1; // Botón presionado
        }
    }
    return 0; // Botón no presionado
}

int DEBOUNCER2() {
    if (BOTON2) { // Verificar si el botón está presionado (lógica negada debido al resistor pull-up)
        _delay_ms(10); // Pequeño retardo para evitar rebotes
        if (BOTON2) { // Verificar nuevamente después del retardo
            return 1; // Botón presionado
        }
    }
    return 0; // Botón no presionado
}

uint16_t ADC_read(uint8_t canal){
	canal&=0b00001111;				//Limitar la entrada a 5
	ADMUX = (ADMUX & 0xF0)|canal;  //Limpiar los últimos 4 bits de ADMUX, OR con ch
	ADCSRA|=(1<<ADSC);				//Inicia la conversión
	while((ADCSRA)&(1<<ADSC));		//Hasta que se complete la coversión
	return(ADC);					//devuelve el valor del adc
}

int VCE(int VCOLECTOR){
    int Vce=0;
    
    if(SALIDA==1){
        Vce=VCOLECTOR;
    }
    if(SALIDA==2){
        Vce=1023-VCOLECTOR;
    }
    if(SALIDA==3 || SALIDA==0){
        Vce=0;
    }
    return Vce;
}

void PNP_NPN(){
    //Base PB0, Colector PB1, Emisor PB2
   SALIDA=0;
    PORTB = 0b00000001; //PB0
    _delay_ms(10);
    dato_ADC=ADC_read(0);
    Vbase=dato_ADC;
    dato_ADC=ADC_read(1);
    Vcolector=dato_ADC;
    _delay_ms(10); //Retardo para cambiar fuente
    
    if(Vbase<1000 && Vbase>30){
        SALIDA=1; //NPN
    }
    Vbase_aux=Vbase;
    Vcolector_aux=Vcolector;
    PORTB=0b00000110; //Enciende solo PB1 (Fuente para probar si es PNP)
    _delay_ms(10); //Retardo para cambiar fuente
    dato_ADC=ADC_read(0); //Con referencia en 5V
    Vbase=dato_ADC;
    dato_ADC=ADC_read(1); //Con referencia en 5V
    Vcolector=dato_ADC;
    if(Vbase<1000 && Vbase>30){
        SALIDA=2; //PNP
    }
  /*  LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%d",Vbase);
    LCD_printf(imprimir);
   //_delay_ms(1500);*/
   
    
}

void Estado(){
    SALIDA=2;
    /*
    if(Vbase_aux==Vbase && Vbase==0){
        SALIDA=3; //Verifica corto entre base y emisor como emisor es 0V
    }
    if(Vcolector_aux==Vcolector && Vcolector==0){
        SALIDA=3; //Verifica corto entre colector y emisor como emisor es 0V
    }
    if(Vcolector==Vbase && Vcolector_aux==Vbase_aux){
        SALIDA=3; //Verifica corto entre la base y el colector
    }
    */
    if(SALIDA==1){
        //NPN
        PORTB=0b00000011; //Enciende solo PB0 y PB1(Fuente para probar si es NPN)
        _delay_ms(10); //Retardo para cambiar fuente
    }
    
    if(SALIDA==2){
        //PNP
        PORTB=0b00000100; //Enciende solo PB2 (Fuente para probar si es NPN)
        _delay_ms(10); //Retardo para cambiar fuente
    }
    
    if(SALIDA==3){
        PORTB=0b00001000;
        _delay_ms(10); //Retardo para cambiar fuente
    }
    
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","ESTADO TR");
    LCD_printf(imprimir);
    LCD_set_cursor(1, 0);
    sprintf(imprimir,"%s","CALCULADO");
    LCD_printf(imprimir);
    _delay_ms(1000);
}

void CuadrarPotenciometros(){

    
    //En la anterior función dejó polarizado el transistor según su tipo, si está dañado se apaga el led
    
    //Ponga el potenciometro de la base del transistor en la menor corriente para la cual Vc este entre 0.2-0.4V y oprima el botón
    //Si cambiando solo el potenciometro de la base no llega al rango de 0.2-0.4V, entonces cambie tambien el potenciometro del colector hasta obtener el rango
    //Se necesita la minima corriente de base con la maxima corriente de colector
    dato_ADC=ADC_read(1); //Con referencia en 5V
    Vcolector=dato_ADC;
    Vce=VCE(Vcolector);
    
    //printf("Cuadre Potenciometros y presione el BOTON"); //PRUEBA IC2
    //SALTO;
    
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","Cuadre potenciometros y presione B1"); //MIRAR
    LCD_desplazar(imprimir);
    
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","Presione B1");
    LCD_printf(imprimir);
    
    
    while(DEBOUNCER1()){ //Espera mientras el boton es 1
        LCD_clear();
        LCD_set_cursor(0, 0);
        sprintf(imprimir,"%s","Presione B1");
        LCD_printf(imprimir);
        
        dato_ADC=ADC_read(1); //Con referencia en 5V
        Vcolector=dato_ADC;
        Vce=VCE(Vcolector);
        VCE_mostrar=(((5*((double)Vce)) / 1023));
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","VCE: ");
        LCD_printf(imprimir);
        sprintf(disbf, "%.3f", VCE_mostrar);
        LCD_set_cursor(1, 4);
        LCD_printf(disbf);
        _delay_ms(200);
    }
    
    dato_ADC=ADC_read(1); //Con referencia en 5V
    Vcolector=dato_ADC;
    Vce=VCE(Vcolector);
    if(Vce>42 && Vce<82){
        //printf("El Vce está dentro del rango\n"); //PRUEBA IC2
        //SALTO;
        LCD_clear();
        LCD_set_cursor(0, 0);
        sprintf(imprimir,"%s","El Vce esta");
        LCD_printf(imprimir);
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","dentro del rango");
        LCD_printf(imprimir);
        PORTB |= (0b00001000);
        _delay_ms(2000);
    }
    else{
        while(Vce<42 || Vce>82){
            //printf("El Vce está fuera del rango, cambie potenciometros\n"); //PRUEBA IC2
            //SALTO;
            
            LCD_clear();
            LCD_set_cursor(0, 0);
            sprintf(imprimir,"%s","Vce fuera de rango,cambie potenciometros ");
            LCD_desplazar(imprimir);
            /*LCD_printf(imprimir);
            for(int i=0;i<=22;i+=1){   
            LCD_scroll_left();
            _delay_ms(400);
            }*/
            
            LCD_clear();
            LCD_set_cursor(0, 0);
            sprintf(imprimir,"%s", "presione B1");
            LCD_printf(imprimir);
            
            while(DEBOUNCER1()){
                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","Presione B1");
                LCD_printf(imprimir);

                dato_ADC=ADC_read(1); //Con referencia en 5V
                Vcolector=dato_ADC;
                Vce=VCE(Vcolector);
                VCE_mostrar=(((5*((double)Vce)) / 1023));
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","VCE: ");
                LCD_printf(imprimir);
                sprintf(disbf, "%.3f", VCE_mostrar);
                LCD_set_cursor(1, 4);
                LCD_printf(disbf);
                _delay_ms(200);
            }
        }
        LCD_clear();
        LCD_set_cursor(0, 0);
        sprintf(imprimir,"%s","El Vce esta");
        LCD_printf(imprimir);
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","dentro del rango");
        LCD_printf(imprimir);
        PORTB |= (0b00001000);
        _delay_ms(2000);
    }
}

void Beta(){
    if(SALIDA==1 || SALIDA==2){
        
        for(int i=0;i<10;i++){
            LCD_clear();
            dato_ADC=ADC_read(0);
            Vbase=dato_ADC;
            dato_ADC=ADC_read(1);
            Vcolector=dato_ADC;
            dato_ADC=ADC_read(2);
            VRbase=dato_ADC;
            dato_ADC=ADC_read(3);
            VRcolector=dato_ADC;
            VBASE=((5*((double)Vbase))/1023);
            VCOLECTOR=((5*((double)Vcolector))/1023);
            VRBASE=((5*((double)VRbase))/1023);
            VRCOLECTOR=((5*((double)VRcolector))/1023);
            if(SALIDA==1){
                hfe=hfe+((RB/RC)*((VRCOLECTOR-VCOLECTOR)/(VRBASE-VBASE)));
            }
            if(SALIDA==2){
                hfe=hfe+((RB/RC)*((VCOLECTOR-VRCOLECTOR)/(VBASE-VRBASE)));
            }
        }
        hfe=hfe/10;
    }
    if(SALIDA==3){
        hfe=0; //No se calcula
    }
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","BETA DEL TR: ");
    LCD_printf(imprimir);
    LCD_set_cursor(1, 0);
    sprintf(imprimir,"%.3f",hfe);
    LCD_printf(imprimir);
    _delay_ms(2500);
}

void V_EARLY(){
    //Se necesitan 2 mediciones del VCE y IC VCE=VC,IC=((Vcc-VRc)/RC),VRc=((5*VRcolector)/1023);
    PORTB &= ~(0b00001000);
    
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","Suba el Vce y presione B1");
    LCD_desplazar(imprimir);
    /*LCD_printf(imprimir);
    for(int i=0;i<=10;i+=1){   
    LCD_scroll_left();
    _delay_ms(400);
    }*/
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","Presione B1");
    LCD_printf(imprimir);
    
    
    dato_ADC=ADC_read(1);
    Vce=VCE(dato_ADC);
    VCE1=((5*((double)Vce))/1023);
    
    
    LCD_set_cursor(1, 0);
    sprintf(imprimir,"%.3f",VCE1);
    LCD_printf(imprimir);
    
    
    while( DEBOUNCER1() ){
        while( (VCE1<1) || (VCE1>2)){
            dato_ADC=ADC_read(1);
            Vce=VCE(dato_ADC);
            VCE1=((5*((double)Vce))/1023);
            sprintf(disbf, "%.3f", VCE1);
            LCD_set_cursor(1, 0);
            sprintf(imprimir,"%s","VCE1: ");
            LCD_printf(imprimir);
            LCD_set_cursor(1, 6);
            LCD_printf(disbf);
            _delay_ms(200);
        }
        dato_ADC=ADC_read(1);
        Vce=VCE(dato_ADC);
        VCE1=((5*((double)Vce))/1023);
        sprintf(disbf, "%.3f", VCE1);
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","VCE1: ");
        LCD_printf(imprimir);
        LCD_set_cursor(1, 6);
        LCD_printf(disbf);
        _delay_ms(200);
    }
    
    PORTB |= 0b00001000;
    _delay_ms(1000); //espera un segundo para no equivocarse
    
    dato_ADC=ADC_read(1);
    Vcolector=dato_ADC;
    VCOLECTOR=((5*(double)Vcolector)/1023);
    
    dato_ADC=ADC_read(3);
    VRcolector=dato_ADC;
    VRCOLECTOR=((5*(double)VRcolector)/1023);
    
    if(SALIDA==1){
        IC1=(VRCOLECTOR-VCOLECTOR)/(RC);
    }
    if(SALIDA==2){
        IC1=(VCOLECTOR-VRCOLECTOR)/(RC);
    }
    
    PORTB &= ~(0b00001000);
    //printf("Suba el Vce y presione el BOTON");
    //SALTO;
    LCD_clear();
    LCD_set_cursor(0, 0);
    //sprintf(imprimir,"%s","Cuadre potenciometros y presione B1");
    sprintf(imprimir,"%s","Suba el Vce y presione B1");
    LCD_desplazar(imprimir);
    /*LCD_printf(imprimir);
    for(int i=0;i<=10;i++){   
    LCD_scroll_left();
    _delay_ms(400);
    }*/
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","presione B1");
    LCD_printf(imprimir);
    
    dato_ADC=ADC_read(1);
    Vce=VCE(dato_ADC);
    VCE2=((5*((double)Vce))/1023);
    while( DEBOUNCER1() ){
        while( VCE2<(VCE1+0.1) ){
            dato_ADC=ADC_read(1);
            Vce=VCE(dato_ADC);
            VCE2=((5*((double)Vce))/1023);
            sprintf(disbf, "%.3f", VCE2);
            LCD_set_cursor(1, 0);
            sprintf(imprimir,"%s","VCE2: ");
            LCD_printf(imprimir);
            LCD_set_cursor(1, 6);
            LCD_printf(disbf);
            _delay_ms(200);
        }
        dato_ADC=ADC_read(1);
        Vce=VCE(dato_ADC);
        VCE2=((5*((double)Vce))/1023);
        sprintf(disbf, "%.3f", VCE2);
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","VCE2: ");
        LCD_printf(imprimir);
        LCD_set_cursor(1, 6);
        LCD_printf(disbf);
        _delay_ms(200);
    }
    
    PORTB |= 0b00001000;
    _delay_ms(500); //espera un segundo para no equivocarse
    dato_ADC=ADC_read(1);
    Vcolector=dato_ADC;
    VCOLECTOR=((5*(double)Vcolector)/1023);
    
    dato_ADC=ADC_read(3);
    VRcolector=dato_ADC;
    VRCOLECTOR=((5*(double)VRcolector)/1023);
    
    if(SALIDA==1){
        IC2=(VRCOLECTOR-VCOLECTOR)/(RC);
    }
    if(SALIDA==2){
        IC2=(VCOLECTOR-VRCOLECTOR)/(RC);
    }
    
    if(VCE2>VCE1){
        ro=((VCE2-VCE1)/(IC2-IC1));
    }
    else{
        ro=((VCE1-VCE2)/(IC1-IC2));
    }
    VAF=(IC1*ro)-VCE1;
    LCD_clear();
    LCD_set_cursor(0, 0);
    sprintf(imprimir,"%s","VAF calculado");
    LCD_printf(imprimir);
    
    //printf("VAF calculado");
    //SALTO;
    LCD_set_cursor(1, 0);
    sprintf(imprimir,"%.3f",VAF);
    LCD_printf(imprimir);
    _delay_ms(1500);
}

void caracteristicas(){
    LCD_clear();
    while(DEBOUNCER1()){
        LCD_clear();
        LCD_set_cursor(0, 0);
        sprintf(imprimir,"%s","B1 REINICIAR");
        LCD_printf(imprimir);
        LCD_set_cursor(1, 0);
        sprintf(imprimir,"%s","B2 RESULTADOS");
        LCD_printf(imprimir);
        
        if(DEBOUNCER2()==0){
            //printf("Caracteristicas de su transistor");
            //SALTO;
            if(SALIDA==3){
                //printf("Su transistor está dañado");
                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","TR DAÑADO");
                LCD_printf(imprimir);
            }
            if(SALIDA==1){
                //printf("TRANSISTOR NPN");
                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","TRANSISTOR NPN");
                LCD_printf(imprimir);
                _delay_ms(2000);
            }
            if(SALIDA==2){
                //printf("TRANSISTOR PNP");
                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","TRANSISTOR PNP");
                LCD_printf(imprimir);
                _delay_ms(2000);
            }
            if(SALIDA==1 || SALIDA==2){
                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","Beta: ");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 5);
                sprintf(imprimir, "%.3f", hfe);
                LCD_printf(imprimir);
                _delay_ms(1500);

                LCD_clear();
                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","VAF: ");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 4);
                sprintf(imprimir, "%.3f", VAF);
                LCD_printf(imprimir);
                sprintf(imprimir,"%s","V");
                LCD_printf(imprimir);
                _delay_ms(1500);

                LCD_clear();

                LCD_set_cursor(0, 0);
                sprintf(imprimir,"%s","CARACTERISTICAS");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 0);
                sprintf(imprimir,"%s","ro: ");
                LCD_printf(imprimir);
                LCD_set_cursor(1, 4);
                sprintf(imprimir, "%.1f", ro);
                LCD_printf(imprimir);
                sprintf(imprimir,"%s","ohms");
                LCD_printf(imprimir);
                _delay_ms(1500);
                LCD_clear();
            }
        }
    }
}

void inicializar_registros(){
    S=1;
    Vbase=0;
    Vcolector=0;
    Vce=0; 
    VCE_mostrar=0;
    Vbase_aux=0;
    Vcolector_aux=0;
    VRbase=0;
    VRcolector=0;
    SALIDA=1; 
    hfe=0;
    VAF=0; 
    VRBASE=0;
    VRCOLECTOR=0;
    VCE1=0;
    VCE2=0;
    IC1=0;
    IC2=0;
    ro=0;
};

int main(){
    LCD_init();
    Setup();
    inicializar_registros();
    while(1){
       //PNP_NPN();
        Estado();
        CuadrarPotenciometros();
        Beta();
        V_EARLY();
        caracteristicas();
    }
} 


