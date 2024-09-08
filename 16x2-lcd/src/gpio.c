/*
 ============================================================================
 Name        : gpio.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Driver for BBB GPIOs.
 ============================================================================
 */

#include "gpio.h"

#define SYSFS_GPIO_PATH	"/sys/class/gpio"


/**
  * @brief  Create GPIO node and configure the direction.
  * @param  gpio: GPIO number
  * @param  dir: direction; 0=out, 1=in
  * @retval returns the value file descriptor if success, else -1
  */
int gpio_init(char *gpio, int dir)
{
	int fd, ret;
	char temp[100];

	/* create the GPIO file if it does not exist */
	strcpy(temp, SYSFS_GPIO_PATH "/gpio");
	strcat(temp, gpio);
	if(access(temp, F_OK)){
		fd = open(SYSFS_GPIO_PATH "/export", O_WRONLY);
		if(fd <= 0){
			perror("Cannot open export file\n");
			return -1;
		}

		ret = write(fd, gpio, strlen(gpio));
		if(ret != strlen(gpio)){
			perror("Cannot write export file\n");
			return -1;
		}
		close(fd);
	}

	/* configure the direction */
	strcpy(temp, SYSFS_GPIO_PATH "/gpio");
	strcat(temp, gpio);
	strcat(temp, "/direction");
	fd = open(temp, O_WRONLY);
	if(fd <= 0){
		perror("Cannot open direction file\n");
		return -1;
	}
	if(dir == DIR_OUT){
		write(fd, "out", strlen("out"));
	}
	else{
		write(fd, "in", strlen("in"));
	}
	close(fd);

	/* get the value file descriptor */
	strcpy(temp, SYSFS_GPIO_PATH "/gpio");
	strcat(temp, gpio);
	strcat(temp, "/value");
	fd = open(temp, O_RDWR);
	if(fd <= 0){
		perror("Cannot open value file\n");
		return -1;
	}

	return fd;
}

/**
  * @brief  Write GPIO.
  * @param  fd: file descriptor for the GPIO
  * @param  val: value, 0/1
  * @retval None
  */
void write_gpio(int fd, int val)
{
	if(val)
		write(fd, "1", sizeof("1"));
	else
		write(fd, "0", sizeof("0"));
}

/**
  * @brief  Read GPIO.
  * @param  fd: file descriptor for the GPIO
  * @retval value, 0/1
  */
int read_gpio(int fd)
{
	char val;

	lseek(fd, 0, SEEK_SET);
	read(fd, &val, sizeof(val));
	if(val == '0')
		return 0;
	else
		return 1;
}
