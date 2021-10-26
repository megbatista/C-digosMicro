#define _XTAL_FREQ 20000000  

#include <xc.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include "lcd_kit.h"

#pragma config FOSC = INTOSC_EC 		// Oscilador interno
#pragma config WDT = OFF            		// Watchdog Timer desligado
#pragma config MCLRE = OFF      		// Master Clear desabilitado
#pragma config PBADEN = OFF         		// Desliga o conversor AD

long contagem = 0;
char linha1[16];
char linha2[16];

int luminosidade;

void setupTmr2() {
    TMR2 = 0x00;
    PR2 = 249;       		// Período PWM = 250 us * pré-escala
}
void setupInt(void) {
    GIE = 1;       	 	// Habilita interrupção global
    PEIE = 1;       		// Habilita interrupção de periféricos
    ADIE = 1;       		// Habilita interrupção do ADC   
 }
void setupADC(void) {
    TRISA = 0b00000001;         		// Habilita pino A0 como entrada
    ADCON2bits.ADCS = 0b100;    	// Clock do AD: Fosc/4
    ADCON2bits.ACQT = 0b010;    	// Tempo de aquisição: 4 Tad
    ADCON2bits.ADFM = 0b0;      	// Formato: à esquerda
    ADCON1bits.VCFG = 0b00;     	// Tensões de referência: Vss e Vdd  
    ADCON0bits.CHS = 0b0000;    	// Seleciona o canal AN0
    ADCON0bits.ADON = 1;        	// Liga o AD
}
void setupPWM (void) {
    setupTmr2();                    		// Configura timer 2  
    CCP2CONbits.CCP2M = 0b1100;   	// Seleciona Modo PWM 
    T2CONbits.T2CKPS = 0b01;        	// Pré-escala = x4
    TMR2IF = 0;                     		// Limpa flag do TMR2
    TMR2ON = 1;                     		// Dispara o timer
    TRISCbits.TRISC1 = 0;           	// Habilita RC1 como saída  
}
void interrupt interrupcao() {   
    if (ADIF) {     
        CCPR2L = ADRESH;                 // igual aos 8 MSB do ADC
        luminosidade = (ADRESH*100)/255; 
        ADIF = 0;                               	// Desmarca flag da interrupção ADC
        ADCON0bits.GO = 1;                // Dispara ADC

         instr_wr(0xC7);
         sprintf(linha2, "%i %c      ", luminosidade, 0x25);
         escreve_texto(linha2);
     }    
}
void main(void){
    OSCCON = 0b01100000;		// Oscilador interno a 4 MHz
    TRISD = 0b00000000;		// Define porta D inteira como saída
    TRISE = 0;

    setupADC();                     		// Configuração do ADC
    setupInt();                     		// Configuração da Interrupção
    setupPWM();			// Configuração do PWM
    inicializa();                   		// Inicializa LCD
    sprintf(linha1, "  Luminosidade  ");
    escreve_texto(linha1);

    ADCON0bits.GO = 1;              	// Dispara ADC

while(1) {
}
return;
}