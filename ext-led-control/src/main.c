/*
 ============================================================================
 Name        : main.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Controlling the blink rate of an external LED via a push-button.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define SYSFS_GPIO_PATH	"/sys/class/gpio"
#define LED_GPIO		"46"
#define BTN_GPIO		"47"
#define DELAY_MIN		250
#define DELAY_MAX		1000
#define DELAY_STEP		300


int main(int argc, char *argv[])
{
	int delay_value_ms = DELAY_MAX, ledstate = 0;
	char btnval = '1';
	int fdled, fdbtn, ret;

	/* create the GPIO files if they don't exist */
	if(access(SYSFS_GPIO_PATH "/gpio" LED_GPIO, F_OK)){
		fdled = open(SYSFS_GPIO_PATH "/export", O_WRONLY);
		if(fdled <= 0){
			perror("Cannot open export file\n");
			return -1;
		}

		ret = write(fdled, LED_GPIO, strlen(LED_GPIO));
		if(ret != strlen(LED_GPIO)){
			perror("Cannot write export file\n");
			return -1;
		}
		close(fdled);
	}

	if(access(SYSFS_GPIO_PATH "/gpio" BTN_GPIO, F_OK)){
		fdbtn = open(SYSFS_GPIO_PATH "/export", O_WRONLY);
		if(fdbtn <= 0){
			perror("Cannot open export file\n");
			return -1;
		}

		ret = write(fdbtn, BTN_GPIO, strlen(BTN_GPIO));
		if(ret != strlen(BTN_GPIO)){
			perror("Cannot write export file\n");
			return -1;
		}
		close(fdbtn);
	}

	/* open the GPIO files and configure the direction */
	fdled = open(SYSFS_GPIO_PATH "/gpio" LED_GPIO "/direction", O_WRONLY);
	if(fdled <= 0){
		perror("Cannot open LED GPIO file\n");
		return -1;
	}
	write(fdled, "out", strlen("out"));
	close(fdled);

	fdled = open(SYSFS_GPIO_PATH "/gpio" LED_GPIO "/value", O_WRONLY);
	if(fdled <= 0){
		perror("Cannot open LED GPIO file\n");
		return -1;
	}

	fdbtn = open(SYSFS_GPIO_PATH "/gpio" BTN_GPIO "/direction", O_WRONLY);
	if(fdbtn <= 0){
		perror("Cannot open Button GPIO file\n");
		return -1;
	}
	write(fdbtn, "in", strlen("in"));
	close(fdbtn);

	fdbtn = open(SYSFS_GPIO_PATH "/gpio" BTN_GPIO "/value", O_RDONLY);
	if(fdbtn <= 0){
		perror("Cannot open Button GPIO file\n");
		return -1;
	}

	while(1){
		lseek(fdbtn, 0, SEEK_SET);
		read(fdbtn, &btnval, sizeof(btnval));
		if(btnval == '0'){
			delay_value_ms -= DELAY_STEP;
			if(delay_value_ms < DELAY_MIN){
				delay_value_ms = DELAY_MAX;
			}
		}

		ledstate = 1 - ledstate;
		if(ledstate)
			write(fdled, "1", sizeof("1"));
		else
			write(fdled, "0", sizeof("0"));

		usleep(delay_value_ms * 1000);
	}

	return 0;
}
