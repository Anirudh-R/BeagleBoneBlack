/*
 ============================================================================
 Name        : hd44780.h
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Driver for HD44780 LCD controller in 4-bit mode and only write
 	 	 	   functionality.
 ============================================================================
 */

#ifndef HD44780_H_
#define HD44780_H_

#include "gpio.h"
#include <stdint.h>


int lcd_init(char *rs_gpio, char *e_gpio, char *d4_gpio, char *d5_gpio, char *d6_gpio, char *d7_gpio);
void lcd_print_string(uint8_t line, char *str);

#endif /* HD44780_H_ */
