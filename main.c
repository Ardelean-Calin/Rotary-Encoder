/*
 * File:   main.c
 * Author: calin
 *
 * Created on December 22, 2015, 8:18 PM
 */


#include <xc.h>
// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

#define _XTAL_FREQ 4000000 

int PWM_value = 0;

void setupGPIO(){
    // GP0 and GP1 digital inputs
    TRISIObits.TRISIO0 = 1;
    TRISIObits.TRISIO1 = 1;
    ANSELbits.ANS0 = 0;
    ANSELbits.ANS1 = 0;
    CMCON0bits.CM = 0b111;
    
    // GP2 is PWM so digital OUTPUT
    ANSELbits.ANS2 = 0;
    TRISIObits.TRISIO2 = 0;
    
}


void setPWM(){
    CCP1CONbits.DC1B = PWM_value & 0x03; /* Two LSB's */
    CCPR1L = (PWM_value >> 2) & 0xff; /* Eight MSB's */
}


void setupPWM(){
    TRISIObits.TRISIO2 = 1; // disable CCP
    PR2 = 0x3F;
    CCP1CONbits.CCP1M = 0b1100; // PWM active high
    
    setPWM();
    
    PIR1bits.TMR2IF = 0;
    T2CONbits.T2CKPS = 0; // prescaler = 1
    T2CONbits.TMR2ON = 1; // enable timer 2. Start counting    
    
    while(!TMR2IF){ // Datasheet says to wait until TMR2IF is set
        continue;
    }
    TRISIObits.TRISIO2 = 0; // enable CCP1
}


void main(void) {
    
    setupGPIO();
    setupPWM();
    
    unsigned char prevA = 1;
    unsigned char A = 1;
    unsigned char B = 1;
   
    while(1){
        // Read A and B
        A = GPIObits.GP0;
        B = GPIObits.GP1;
        // Check the value of B at the falling edge of A
        if((!A) && prevA){
            if(B==1){
                if(PWM_value > 0) //CCW
                    PWM_value--;
            }
            else{
                if(PWM_value < 255) // CW
                    PWM_value++;
            }
        }
        prevA = A;
        setPWM();
    }
    
    return;
}
