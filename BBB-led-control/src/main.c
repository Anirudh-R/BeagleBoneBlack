/*
 ============================================================================
 Name        : main.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Controlling the BBB on-board LEDs via command line
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define SYSFS_LED_PATH	"/sys/class/leds"
#define TEMP_BUF_SIZE	100						/* size of temporary buffer */


/**
  * @brief  Writes trigger value for the given LED.
  * @param  led_no: LED number, 0-3
  * @param  trigger: trigger string
  * @retval returns 0 if success, else -1
  */
int write_trigger_value(int led_no, char* trigger)
{
	int fd, ret;
	char tempbuf[TEMP_BUF_SIZE];

	snprintf(tempbuf, TEMP_BUF_SIZE, SYSFS_LED_PATH "/beaglebone:green:usr%d/trigger", led_no);

	fd = open(tempbuf, O_WRONLY);
	if(fd <= 0){
		perror("Cannot open trigger file\n");
		return -1;
	}

	ret = write(fd, trigger, strlen(trigger));
	if(ret != strlen(trigger)){
		perror("Cannot write trigger file\n");
		return -1;
	}

	return 0;
}

/**
  * @brief  Writes brightness value for the given LED.
  * @param  led_no: LED number, 0-3
  * @param  brightness: brightness string, 0/1
  * @retval returns 0 if success, else -1
  */
int write_brightness_value(int led_no, char* brightness)
{
	int fd, ret;
	char tempbuf[TEMP_BUF_SIZE];

	snprintf(tempbuf, TEMP_BUF_SIZE, SYSFS_LED_PATH "/beaglebone:green:usr%d/brightness", led_no);

	fd = open(tempbuf, O_WRONLY);
	if(fd <= 0){
		perror("Cannot open brightness file\n");
		return -1;
	}

	ret = write(fd, brightness, strlen(brightness));
	if(ret != strlen(brightness)){
		perror("Cannot write brightness file\n");
		return -1;
	}

	return 0;
}

/**
  * @brief  Processes trigger values given by the user.
  * @param  led_no: LED number, 0-3
  * @param  trigger: trigger string
  * @retval None
  */
void process_trigger(int led_no, char* trigger)
{
	if(!strcmp(trigger, "timer") || !strcmp(trigger, "heartbeat") ||
	   !strcmp(trigger, "none") || !strcmp(trigger, "oneshot") ||
	   !strcmp(trigger, "default-on")){
		write_trigger_value(led_no, trigger);
	}
	else{
		printf("Invalid trigger value, valid ones are heartbeat, timer, none, oneshot, default-on\n");
	}
}

/**
  * @brief  Processes brightness values given by the user.
  * @param  led_no: LED number, 0-3
  * @param  brightness: brightness string
  * @retval None
  */
void process_brightness(int led_no, char* brightness)
{
	if(!strcmp(brightness, "0") || !strcmp(brightness, "1")){
		write_brightness_value(led_no, brightness);
	}
	else{
		printf("Invalid brightness value, valid ones are 0 or 1\n");
	}
}

int main(int argc, char *argv[]) 
{
	if(argc != 4){
		printf("Usage: %s <led> <control-option> <value>\n", argv[0]);
		printf("Valid led values: 0-3\n");
		printf("Valid control-options: trigger, brightness\n");
		printf("Valid trigger values: heartbeat, timer, none, oneshot, default-on\n");
		printf ("Valid brightness values: 0, 1\n");
	}
	else{
		int led = atoi(argv[1]);

		if(led >= 0 && led <= 3){
			if(!strcmp(argv[2], "trigger")){
				process_trigger(led, argv[3]);
			}
			else if(!strcmp(argv[2], "brightness")){
				process_brightness(led, argv[3]);
			}
			else{
				printf("Invalid control-option, valid ones are trigger, brightness\n");
			}
		}
		else{
			printf("Invalid led value, valid ones are 0-3\n");
		}
	}

	return 0;
}
