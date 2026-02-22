#ifndef __LCD_H
#define __LCD_H

#include "stdint.h"
#include "stm32f1xx_hal.h"


#define LCD_COLS 16
#define LCD_ROWS 2

#define LCD_BACKLIGHT 0x08
#define LCD_EN 0x04
#define LCD_RW 0x02
#define LCD_RS 0x01


typedef struct {
    I2C_HandleTypeDef *hi2c;     // I2C handler for communication
    uint8_t address;            // I2C address of the LCD
} I2C_LCD_HandleTypeDef;

void lcd_init(I2C_LCD_HandleTypeDef *lcd);
void lcd_send_cmd(I2C_LCD_HandleTypeDef *lcd, char cmd);
void lcd_send_data(I2C_LCD_HandleTypeDef *lcd, char data);
void lcd_send_char(I2C_LCD_HandleTypeDef *lcd, char ch);
void lcd_send_string(I2C_LCD_HandleTypeDef *lcd, char *str);

/**
 * @brief Moves the cursor to a specific position on the LCD.
 * @param lcd: Pointer to the LCD handle
 * @param col: Column number (0-15)
 * @param row: Row number (0 or 1)
 */
void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, int col, int row);

/**
 * @brief Clears the LCD display.
 * @param lcd: Pointer to the LCD handle
 */
void lcd_clear(I2C_LCD_HandleTypeDef *lcd);


#endif