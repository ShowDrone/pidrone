#ifndef RPI_H
#define RPI_H

#include <pca9685.h>
#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Rpi {
	int setValue;
} RPI;

typedef struct Led {
	char *setValue;
} LED;

RPI rpi = { 0 };
LED led;

// �� ����

int pwid;
int startCount = 0;
int endCount = 6;
char *user = "pi";      // Raspberry Pi ID
char *pw = "vkdlemfhs"; // Raspberry Pi Password

void lBeep() {
	if (endCount > 0) {
		if (startCount < endCount) {
			if (startCount % 2 == 0) {
				//setAngle(bz, 0);    // buz on                                                                                                                                                                                        
			}
			else {
				//setAngle(bz, 255); // buz off
			}
			startCount++;
		}
		else {
			startCount = 0;
			endCount = 0;
		}
	}
}

#endif
