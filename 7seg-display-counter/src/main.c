/*
 ============================================================================
 Name        : main.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : 7-segment display up/down counter. Direction controlled by a button.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define SYSFS_GPIO_PATH	"/sys/class/gpio"
#define SEG_A_GPIO		"66"
#define SEG_B_GPIO		"67"
#define SEG_C_GPIO		"69"
#define SEG_D_GPIO		"68"
#define SEG_E_GPIO		"45"
#define SEG_F_GPIO		"44"
#define SEG_G_GPIO		"26"
#define BTN_GPIO		"47"
#define DIR_OUT			0
#define DIR_IN			1


void write_gpio(int fd, int val);

int fd_a, fd_b, fd_c, fd_d, fd_e, fd_f, fd_g;

/**
  * @brief  Output a single digit on a 7-segment display.
  * @param  digit: number to display
  * @retval None
  */
void display(int digit)
{
	switch(digit)
	{
	case 0: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 1);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 0);
			break;

	case 1: write_gpio(fd_a, 0);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 0);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 0);
			write_gpio(fd_g, 0);
			break;

	case 2: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 0);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 1);
			write_gpio(fd_f, 0);
			write_gpio(fd_g, 1);
			break;

	case 3: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 0);
			write_gpio(fd_g, 1);
			break;

	case 4: write_gpio(fd_a, 0);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 0);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 1);
			break;

	case 5: write_gpio(fd_a, 1);
			write_gpio(fd_b, 0);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 1);
			break;

	case 6: write_gpio(fd_a, 1);
			write_gpio(fd_b, 0);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 1);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 1);
			break;

	case 7: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 0);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 0);
			write_gpio(fd_g, 0);
			break;

	case 8: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 1);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 1);
			break;

	case 9: write_gpio(fd_a, 1);
			write_gpio(fd_b, 1);
			write_gpio(fd_c, 1);
			write_gpio(fd_d, 1);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 1);
			write_gpio(fd_g, 1);
			break;

	default: write_gpio(fd_a, 0);
			write_gpio(fd_b, 0);
			write_gpio(fd_c, 0);
			write_gpio(fd_d, 0);
			write_gpio(fd_e, 0);
			write_gpio(fd_f, 0);
			write_gpio(fd_g, 0);
			break;
	}
}

/**
  * @brief  Create GPIO node and configure the direction.
  * @param  gpio: GPIO number
  * @param  dir: direction; 0=out, 1=in
  * @retval returns the value file descriptor if success, else -1
  */
int gpio_init_dir(char *gpio, int dir)
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

int main(int argc, char *argv[])
{
	int fd_btn, btn_val, dir = 1, num = 0, i;

	fd_a = gpio_init_dir(SEG_A_GPIO, DIR_OUT);
	fd_b = gpio_init_dir(SEG_B_GPIO, DIR_OUT);
	fd_c = gpio_init_dir(SEG_C_GPIO, DIR_OUT);
	fd_d = gpio_init_dir(SEG_D_GPIO, DIR_OUT);
	fd_e = gpio_init_dir(SEG_E_GPIO, DIR_OUT);
	fd_f = gpio_init_dir(SEG_F_GPIO, DIR_OUT);
	fd_g = gpio_init_dir(SEG_G_GPIO, DIR_OUT);
	fd_btn = gpio_init_dir(BTN_GPIO, DIR_IN);

	while(1){
		display(num);

		for(i = 0; i < 1000; i++){
			btn_val = read_gpio(fd_btn);
			if(btn_val == 0)
				dir = 1 - dir;
			usleep(1000);
		}

		if(dir){
			num++;
			if(num > 9)
				num = 0;
		}
		else{
			num--;
			if(num < 0)
				num = 9;
		}
	}

	return 0;
}
