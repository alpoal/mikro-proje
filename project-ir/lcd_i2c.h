#ifndef LCD_I2C_H_
#define LCD_I2C_H_

void I2C_Init(void);
void LCD_Init(void);
void LCD_SendCmd(unsigned char cmd);
void LCD_SendData(unsigned char data);
void LCD_SendString(char *str);
void LCD_EnablePulse(unsigned char data);
void LCD_WriteNibble(unsigned char nibble, unsigned char rs);

#endif /* LCD_I2C_H_ */
