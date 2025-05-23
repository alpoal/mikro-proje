#include <msp430.h>
#include "lcd_i2c.h"

char buffer[16];
unsigned int adc_val = 0;
unsigned int person_counter = 0;

#define IR1_CHANNEL INCH_0  // P1.0 ADC kanalı

unsigned int readADC(unsigned char channel) {
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + ADC10ON;
    ADC10CTL1 = channel;
    ADC10CTL0 |= ENC + ADC10SC;
    while (ADC10CTL1 & ADC10BUSY);
    return ADC10MEM;
}

void delay_ms(unsigned int ms) {
    while (ms--) __delay_cycles(1000);
}

void intToStr(unsigned int val, char *str)  {
    int i = 0;
    char temp[5];

    do {
        temp[i++] = (val % 10) + '0';
        val /= 10;
    } while (val && i < 5);

    while (i--) *str++ = temp[i];
    *str = '\0';
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    ADC10AE0 |= BIT0;                                             

    I2C_Init();
    LCD_Init();
   
    int obstacle = 0; 
    while (1) {
        char str_int[6];

        adc_val = readADC(IR1_CHANNEL);
        intToStr(adc_val, str_int); 
        
        if (adc_val > 400 && obstacle == 0) { 
            person_counter++; 
            obstacle = 1;
        }

        if (adc_val < 400 && obstacle == 1) {
            obstacle = 0;
        }
       
        LCD_SendCmd(0x01);  // Temizle
        LCD_SendString(str_int);
        LCD_SendCmd(0xC0);  // Alt satir

        intToStr(person_counter, str_int);
        LCD_SendString(str_int);

        delay_ms(50);
    }
}
