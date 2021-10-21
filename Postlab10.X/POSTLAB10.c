/*
  Archivo:  POSTLAB10.c
  Autor:    Alejandro Ramirez Morales
  Creado:   20/oct/21
  Establecer comunicación serie con la hiperterminal. Para controlar 
  el valor desplegado en los PORTA o PORTB. 
 */

// PIC16F887 Configuration Bit Settings

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#include <xc.h>
#include <stdint.h>
#include <string.h>
#define _XTAL_FREQ 1000000

/*
 +----------------------------------------------------------------------------+
 |                                VARIABLES                                   |
 +----------------------------------------------------------------------------+
 */
char data = 0;
char Pregunta [] = "QUE PUERTO DESEA CAMIBAR? 1:PORTA, 2:PORTB";
char Opcion [] = "Escriba el valor a ingresar al puerto:";
char Error1 [] = "Error#1: Ingrese 1 o 2 para seleccionar un puerto.";
char Error2 [] = "Error#2: Ingrese digitos de 0 a 9.";
int i;
int bandera = 0;
int bandera2 = 0;

uint8_t u0 = 0;
uint8_t u1 = 0;
uint8_t u2 = 0;

uint8_t portEs  = 0;

int estado = 0;

/*
 +----------------------------------------------------------------------------+
 |                          PROTOTIPOS DE FUNCIONES                           |
 +----------------------------------------------------------------------------+
 */
void setup(void);
uint8_t asciiAnum (uint8_t valor);

/*
 +----------------------------------------------------------------------------+
 |                               INTERRUPCIONES                               |
 +----------------------------------------------------------------------------+
 */
void __interrupt() isr(void){
    if(PIR1bits.RCIF)
    {
        if (bandera == 1)
        {
            portEs = RCREG;
            
            if (asciiAnum(portEs) == 1){
                bandera = 3;
            }
            else if (asciiAnum(portEs) == 2){
                bandera = 3;
            }
            else{
                bandera = 2;
            }
        }
        
        if (portEs == 0b110001 & bandera == 5)
        {
          bandera2 = bandera2++;
          switch(estado)
          {
              case 0:
                  u0 = asciiAnum(RCREG);  
                  if(u0>= 0 & u0 <= 9){
                      bandera = 5;
                  }
                  else{
                      bandera = 6;
                  }
                  break;
              case 1:
                  u1 = asciiAnum(RCREG);  
                  if(u1 >= 0 & u1 <= 9){
                      bandera = 5;
                  }
                  else{
                      bandera = 6;
                  }
                  break;
              case 2: 
                  u2 = asciiAnum(RCREG);  
                  if(u2 >= 0 & u2 <= 9){
                      bandera = 5;
                      PORTA = (int)100*u0 + (int)10*u1  + u2;
                  }
                  else{
                      bandera = 6;
                  }
                  break;
                  
              default:
                 estado = 0;
                 break;
          }
          estado = estado++;
          
        }
        else if (portEs == 0b110010 & bandera == 5)
        {
            bandera2 = bandera2++;
            switch(estado)
            {
                case 0:
                    u0 = asciiAnum(RCREG);  
                    if(u0 >= 0 & u0 <= 9){
                      bandera = 5;
                    }
                    else{
                        bandera = 6;
                    }
                    break;
                case 1:
                    u1 = asciiAnum(RCREG);  
                    if(u1 >= 0 & u1 <= 9){
                      bandera = 5;
                    }
                    else{
                        bandera = 6;
                    }
                    break;
                case 2: 
                    u2 = asciiAnum(RCREG);   
                    if(u2 >= 0 & u2 <= 9){
                      bandera = 5;
                      PORTB = (int)100*u0 + (int)10*u1  + u2;
                    }
                    else{
                      bandera = 6;
                    }
                    break;
                default:
                   estado = 0;
                   break;
            }
            estado = estado++;
        }
        else if (bandera == 5)
        {
            bandera = 2;
        }
        
    }
}

/*
 +----------------------------------------------------------------------------+
 |                                   LOOP                                     |
 +----------------------------------------------------------------------------+
 */
void main(void) {
    setup();
    
    while (1){
        __delay_ms(100);
                
        if (bandera == 0){
            if(PIR1bits.TXIF)
            {
                for (i = 0; i<= strlen(Pregunta); i++)
                {
                    data = Pregunta[i];
                    TXREG = data;
                    
                    if (i == strlen(Pregunta))
                    {
                        bandera = 1;
                    }
                }
                TXREG = 13;
            }
        }
        else if (bandera == 2){
            if(PIR1bits.TXIF)
            {
                for (i = 0; i<= strlen(Error1); i++)
                {
                    data = Error1[i];
                    TXREG = data;
                    
                    if (i == strlen(Error1))
                    {
                        bandera = 0;
                        TXREG = 13;
                    }
                }
                TXREG = 13;
            }
        }
        
        else if (bandera == 3)
        {
            if(PIR1bits.TXIF)
            {   
                //TXREG = 13;
                 for (i = 0; i<= strlen(Opcion); i++)
                {
                    data = Opcion[i];
                    TXREG = data;
                    
                    if (i == strlen(Opcion))
                    {
                        bandera = 5;
                    }
                }
                TXREG = 13;
            }
        }
        else if (bandera == 6){
            if(PIR1bits.TXIF)
            {  
                for (i = 0; i<= strlen(Error2); i++)
                {
                    data = Error2[i];
                    TXREG = data;
                    
                    if (i == strlen(Error2))
                    {
                        bandera = 0;
                        TXREG = 13;
                    }
                }
                TXREG = 13;
            }
        }
        
        if (bandera2 == 3)
        {
            bandera2=0;
            bandera=0;
            estado = 0;
            if(PIR1bits.TXIF)
            {
                TXREG = 13;
            }
        }
    }
}

/*
 +----------------------------------------------------------------------------+
 |                                  SETUP                                     |
 +----------------------------------------------------------------------------+
 */
void setup(void) {
    // Configuracion de puertos
    ANSEL = 0;
    ANSELH = 0;
    
    TRISA = 0;
    TRISB = 0;
    PORTA = 0;
    PORTB = 0;
    
    // Reloj
    OSCCONbits.IRCF = 0b100;
    OSCCONbits.SCS  = 1;
    
    // 9600 baudios
    // Configuracion de TX y RX
    TXSTAbits.SYNC = 0;  // Asincronico
    TXSTAbits.BRGH = 1;  // High speed 
    BAUDCTLbits.BRG16 = 1; // Generador de 16 bits
    
    // SPBRGH:SPBRG = 25
    SPBRG  = 25;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1; // Serial port enable RX/DT y TX/CK
    RCSTAbits.RX9  = 0; // Recibir 8 bits
    RCSTAbits.CREN = 1; // Activar recepcion
    
    TXSTAbits.TXEN = 1; // Activa trasmi
    TXSTAbits.TX9      = 0;
    
    // Configuracion de interrupciones
    PIR1bits.RCIF   = 0;
    PIE1bits.RCIE   = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;
    
}


uint8_t asciiAnum (uint8_t valor){
    switch(valor){
        case 48:
            return 0; 
            break;
        case 49:
            return 1;
            break;
        case 50:
            return 2;
            break;
        case 51:
            return 3;
            break;
        case 52:
            return 4;
            break;
        case 53:
            return 5;
            break;
        case 54:
            return 6;
            break;
        case 55:
            return 7;
            break;
        case 56:
            return 8;
            break;
        case 57:
            return 9;
            break;
        default:
            break;  
    }    
}