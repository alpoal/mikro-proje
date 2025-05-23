#include <msp430.h>
#include "lcd_i2c.h"

#define LCD_ADDR 0x27  // PCF8574 için yaygın adres (A0-A2 GND)

void I2C_Init(void)
{
    P1SEL |= BIT6 + BIT7;     // P1.6 = SCL, P1.7 = SDA
    P1SEL2 |= BIT6 + BIT7;

    UCB0CTL1 |= UCSWRST;      // USCI B0 reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // I2C master, sync
    UCB0CTL1 = UCSSEL_2 + UCSWRST;        // SMCLK
    UCB0BR0 = 12;              // 1MHz / 12 ≈ 100kHz
    UCB0BR1 = 0;
    UCB0I2CSA = LCD_ADDR;
    UCB0CTL1 &= ~UCSWRST;      // USCI reset kaldır
}

void I2C_Write(unsigned char data)
{
    while (UCB0CTL1 & UCTXSTP);         // Önceki stop bit bekleniyor
    UCB0CTL1 |= UCTR + UCTXSTT;         // Yazma + START
    while (!(IFG2 & UCB0TXIFG));        // TX hazır
    UCB0TXBUF = data;                   // Veri gönder

    while (!(IFG2 & UCB0TXIFG));
    UCB0CTL1 |= UCTXSTP;                // STOP
    while (UCB0CTL1 & UCTXSTP);
}

void LCD_EnablePulse(unsigned char data)
{
    I2C_Write(data | 0x04); // EN=1
    __delay_cycles(2000);
    I2C_Write(data & ~0x04); // EN=0
}

void LCD_WriteNibble(unsigned char nibble, unsigned char rs)
{
    unsigned char data = (nibble & 0xF0) | (rs ? 0x01 : 0x00) | 0x08; // BL=1
    LCD_EnablePulse(data);
}

void LCD_SendCmd(unsigned char cmd)
{
    LCD_WriteNibble(cmd & 0xF0, 0);             // Üst nibble
    LCD_WriteNibble((cmd << 4) & 0xF0, 0);       // Alt nibble
    __delay_cycles(7000);
}

void LCD_SendData(unsigned char data)
{
    LCD_WriteNibble(data & 0xF0, 1);
    LCD_WriteNibble((data << 4) & 0xF0, 1);
    __delay_cycles(7000);
}

void LCD_SendString(char *str)
{
    while (*str)
        LCD_SendData(*str++);
}

void LCD_Init(void)
{
    __delay_cycles(100000); // Başlangıç bekleme

    LCD_WriteNibble(0x30, 0);
    __delay_cycles(40000);
    LCD_WriteNibble(0x30, 0);
    __delay_cycles(8000);
    LCD_WriteNibble(0x30, 0);
    __delay_cycles(5000);
    LCD_WriteNibble(0x20, 0); // 4-bit mod

    LCD_SendCmd(0x28); // 4-bit, 2 satır, 5x8
    LCD_SendCmd(0x0C); // Ekran açık, imleç kapalı
    LCD_SendCmd(0x06); // Sağa yaz
    LCD_SendCmd(0x01); // Temizle
    __delay_cycles(5000);
}
