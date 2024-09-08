/*
 ============================================================================
 Name        : gpio.h
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Driver for BBB GPIOs.
 ============================================================================
 */


#ifndef GPIO_H_
#define GPIO_H_

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#define DIR_OUT			0
#define DIR_IN			1


int gpio_init(char *gpio, int dir);
void write_gpio(int fd, int val);
int read_gpio(int fd);

#endif /* GPIO_H_ */
