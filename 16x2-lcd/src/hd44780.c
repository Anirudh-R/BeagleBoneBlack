/*
 ============================================================================
 Name        : hd44780.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Driver for HD44780 LCD controller in 4-bit mode and only write
 	 	 	   functionality.
 ============================================================================
 */

#include "hd44780.h"

#define LCD_CMD_CLEAR_DISPLAY  	0x01
#define LCD_CMD_RETURN_HOME 	0x02
#define LCD_CMD_ENTRY_MODESET   0X04
#define LCD_CMD_ONOFF_CONTROL  	0x08
#define LCD_CMD_FUNCTION_SET  	0x20
#define LCD_CMD_SET_DDRAM_ADDR	0x80


void lcd_print_char(char c);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_send_cmd(uint8_t cmd);
void lcd_pulse_en(void);

int fd_RS, fd_E, fd_D4, fd_D5, fd_D6, fd_D7;

/**
  * @brief  Initialize LCD using the given GPIOs for the interface.
  * @param  rs_gpio: RS
  * @param  e_gpio: E
  * @param  d4_gpio: D4
  * @param  d5_gpio: D5
  * @param  d6_gpio: D6
  * @param  d7_gpio: D7
  * @retval returns 0 if success, else -1
  */
int lcd_init(char *rs_gpio, char *e_gpio, char *d4_gpio, char *d5_gpio, char *d6_gpio, char *d7_gpio)
{
	uint8_t cmd;

	fd_RS = gpio_init(rs_gpio, DIR_OUT);
	fd_E = gpio_init(e_gpio, DIR_OUT);
	fd_D4 = gpio_init(d4_gpio, DIR_OUT);
	fd_D5 = gpio_init(d5_gpio, DIR_OUT);
	fd_D6 = gpio_init(d6_gpio, DIR_OUT);
	fd_D7 = gpio_init(d7_gpio, DIR_OUT);

	if(fd_RS + fd_E + fd_D4 + fd_D5 + fd_D6 + fd_D7 < 0){
		return -1;
	}

	cmd = LCD_CMD_FUNCTION_SET | (0 << 4) | (1 << 3) | (0 << 2);	/* 4-bit mode, 2 lines, 5x8 font */
	lcd_send_cmd(cmd);
	usleep(2000);

	cmd = LCD_CMD_ENTRY_MODESET | (1 << 1) | (0 << 0);				/* increment cursor, no display shift */
	lcd_send_cmd(cmd);
	usleep(2000);

	cmd = LCD_CMD_ONOFF_CONTROL | (1 << 2) | (0 << 1) | (0 << 0);	/* display on, cursor off, cursor blink off */
	lcd_send_cmd(cmd);
	usleep(2000);

	lcd_send_cmd(LCD_CMD_CLEAR_DISPLAY);
	usleep(2000);

	return 0;
}

/**
  * @brief  Print a string on the given line.
  * @param  line: between 1 and 2
  * @param  str: string of max length 16
  * @retval None
  */
void lcd_print_string(uint8_t line, char *str)
{
	lcd_set_cursor(line, 0);

	while(*str != '\0'){
		lcd_print_char(*str);
		str++;
	}
}

/**
  * @brief  Print a character.
  * @param  c: character to print
  * @retval None
  */
void lcd_print_char(char c)
{
	write_gpio(fd_RS, 1);

	write_gpio(fd_D7, (c >> 7) & 0x01);
	write_gpio(fd_D6, (c >> 6) & 0x01);
	write_gpio(fd_D5, (c >> 5) & 0x01);
	write_gpio(fd_D4, (c >> 4) & 0x01);
	lcd_pulse_en();

	write_gpio(fd_D7, (c >> 3) & 0x01);
	write_gpio(fd_D6, (c >> 2) & 0x01);
	write_gpio(fd_D5, (c >> 1) & 0x01);
	write_gpio(fd_D4, (c >> 0) & 0x01);
	lcd_pulse_en();

	usleep(50);
}

/**
  * @brief  Set cursor position.
  * @param  row: between 1 and 2
  * @param  col: between 1 and 16
  * @retval None
  */
void lcd_set_cursor(uint8_t row, uint8_t col)
{
	uint8_t cmd;

	if(row < 1)
		row = 1;
	else if(row > 2)
		row = 2;

	if(col < 1)
		col = 1;
	else if(col > 16)
		col = 16;

	cmd = LCD_CMD_SET_DDRAM_ADDR | (row - 1)*0x40 | (col - 1);
	lcd_send_cmd(cmd);
	usleep(50);
}

/**
  * @brief  Send a command to the LCD.
  * @param  cmd: command byte
  * @retval None
  */
void lcd_send_cmd(uint8_t cmd)
{
	write_gpio(fd_RS, 0);

	write_gpio(fd_D7, (cmd >> 7) & 0x01);
	write_gpio(fd_D6, (cmd >> 6) & 0x01);
	write_gpio(fd_D5, (cmd >> 5) & 0x01);
	write_gpio(fd_D4, (cmd >> 4) & 0x01);
	lcd_pulse_en();

	write_gpio(fd_D7, (cmd >> 3) & 0x01);
	write_gpio(fd_D6, (cmd >> 2) & 0x01);
	write_gpio(fd_D5, (cmd >> 1) & 0x01);
	write_gpio(fd_D4, (cmd >> 0) & 0x01);
	lcd_pulse_en();
}

/**
  * @brief  Pulse the enable signal to latch data.
  * @param  None
  * @retval None
  */
void lcd_pulse_en(void)
{
	write_gpio(fd_E, 1);
	usleep(50);
	write_gpio(fd_E, 0);
}
