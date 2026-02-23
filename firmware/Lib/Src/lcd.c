#include "lcd.h"

void lcd_send_cmd(I2C_LCD_HandleTypeDef *lcd, char cmd)
{
    uint8_t cmd_h = cmd & 0xF0;
    uint8_t cmd_l = (cmd << 4) & 0xF0;
    uint8_t cmd_t[4];

    cmd_t[0] = cmd_h | LCD_EN | LCD_BACKLIGHT;
    cmd_t[1] = cmd_h | LCD_BACKLIGHT;
    cmd_t[2] = cmd_l | LCD_EN | LCD_BACKLIGHT;
    cmd_t[3] = cmd_l | LCD_BACKLIGHT;

    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, cmd_t, 4, 100);
    HAL_Delay(20);
}

void lcd_send_data(I2C_LCD_HandleTypeDef *lcd, char data)
{
    uint8_t data_h = data & 0xF0;
    uint8_t data_l = (data << 4) & 0xF0;
    uint8_t data_t[4];

    data_t[0] = data_h | LCD_EN | LCD_RS | LCD_BACKLIGHT;
    data_t[1] = data_h | LCD_RS | LCD_BACKLIGHT;
    data_t[2] = data_l | LCD_EN | LCD_RS | LCD_BACKLIGHT;
    data_t[3] = data_l | LCD_RS | LCD_BACKLIGHT;

    HAL_I2C_Master_Transmit(lcd->hi2c, lcd->address, data_t, 4, 100);
    HAL_Delay(20);
}

void lcd_clear(I2C_LCD_HandleTypeDef *lcd)
{
    lcd_send_cmd(lcd, 0x01);    // Clear display command
    for (int i = 0; i < 16 * 2; i++)lcd_send_data(lcd, ' ');
    lcd_send_cmd(lcd, 0x02);    // Return home
}

void lcd_gotoxy(I2C_LCD_HandleTypeDef *lcd, int col, int row)
{
    if (col < 0 || col >= LCD_COLS || row < 0 || row >= LCD_ROWS) return; // Validate input

    uint8_t address;
    if (row == 0) address = 0x80 + col;
    else address = 0xC0 + col;

    lcd_send_cmd(lcd, address);
}

void lcd_init(I2C_LCD_HandleTypeDef *lcd)
{
    HAL_Delay(50);

    lcd_send_cmd(lcd, 0x33);
    HAL_Delay(5);
    lcd_send_cmd(lcd, 0x32);
    HAL_Delay(1);

    // 0x28 = 4-bit mode, 2 lines, 5x8 dots
    lcd_send_cmd(lcd, 0x28);
    HAL_Delay(1);

    // 0x0C = Display ON, Cursor OFF, Blink OFF
    lcd_send_cmd(lcd, 0x0C);
    HAL_Delay(1);

    lcd_send_cmd(lcd, 0x06);
    HAL_Delay(1);

    lcd_send_cmd(lcd, 0x01);
    HAL_Delay(5);
}

void lcd_send_string(I2C_LCD_HandleTypeDef *lcd, char *str)
{
    while (*str) lcd_send_data(lcd, *str++);
}

void lcd_send_char(I2C_LCD_HandleTypeDef *lcd, char ch)
{
    lcd_send_data(lcd, ch);
}