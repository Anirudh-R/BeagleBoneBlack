/*
 ============================================================================
 Name        : main.c
 Author      : Anirudh
 Version     :
 Copyright   :
 Description : Displaying time and CPU frequency on a 16x2 LCD. The LCD is
 	 	 	   powered using SYS_5V pin of BBB. Since the RW pin of LCD is
 	 	 	   grounded, it is safe to connect to the 3.3V GPIOs of BBB.
 ============================================================================
 */

#include <stdlib.h>
#include <time.h>
#include "hd44780.h"

/* LCD pins */
#define RS_GPIO			"66"
#define E_GPIO			"67"
#define D4_GPIO			"69"
#define D5_GPIO			"68"
#define D6_GPIO			"45"
#define D7_GPIO			"44"

#define CPU_FREQ_FILE	"/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_cur_freq"


int main(int argc, char *argv[])
{
	time_t t;
	struct tm lt;
	char tbuf[17];
	char fbuf[17];
	int freq, fd, ret;

	ret = lcd_init(RS_GPIO, E_GPIO, D4_GPIO, D5_GPIO, D6_GPIO, D7_GPIO);
	if(ret < 0){
		printf("Error initializing LCD\n");
		return -1;
	}

	fd = open(CPU_FREQ_FILE, O_RDONLY);
	if(fd <= 0){
		perror("Cannot open CPU freq file\n");
		return -1;
	}

    while(1){
    	t = time(NULL);
    	lt = *localtime(&t);
    	sprintf(tbuf, "    %02d:%02d:%02d    ", lt.tm_hour, lt.tm_min, lt.tm_sec);

    	lseek(fd, 0, SEEK_SET);
    	read(fd, fbuf, sizeof(fbuf));
    	freq = atoi(fbuf) / 1000;		/* convert to MHz */
    	sprintf(fbuf, "CPU = %d MHz     ", freq);

    	lcd_print_string(1, tbuf);
    	lcd_print_string(2, fbuf);

    	usleep(1000 * 1000);
    }

	return 0;
}
