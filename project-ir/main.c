#include <msp430.h>
#include "lcd_i2c.h"

char buffer[16];
unsigned int adc_val = 0;
unsigned int person_counter = 0;

#define IR1_CHANNEL INCH_0  // P1.0 ADC kanalı

unsigned int readSensorA0(void)
{
    ADC10CTL1 = INCH_0;                  // Input channel A0
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10ENC + ADC10SC; // Start conversion
    while (ADC10CTL1 & ADC10BUSY);       // Wait until done
    return ADC10MEM;                     // Return result
}

unsigned int readSensorA1(void)
{
    ADC10CTL1 = INCH_1;                  // Input channel A1
    ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10ENC + ADC10SC; // Start conversion
    while (ADC10CTL1 & ADC10BUSY);       // Wait until done
    return ADC10MEM;                     // Return result
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
   
    ADC10AE0 |= BIT0 + BIT1;                                   

    I2C_Init();
    LCD_Init();
   
    int obstacle = 0; 
    while (1) {
        char str_int[6];

        unsigned int val0 = readSensorA0();
        unsigned int val1 = readSensorA1();

        intToStr(adc_val, str_int); 
        
        if (val0 > 400 && obstacle == 0) { 
            person_counter++; 
            obstacle = 1;
        }

        if (val1 < 400 && obstacle == 1) {
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

