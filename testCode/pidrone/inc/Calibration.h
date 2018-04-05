// Calibration.h

#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_

#include "Rpi.h"
#include <wiringPi.h>
#include <stdio.h>

//extern void setAngle(DC& dc, float input);
//extern void setThrottle(BL& bl, float input);


void setBldcCalibration(bool CALIBRATION_DEBUG) {
/*
	if (CALIBRATION_DEBUG == 1) {
		printf("BLDC Calibration Start\n");
	}

	if (CALIBRATION_DEBUG == 1) {
		printf("BLDC Max Output\n");
	}
	setThrottle(bl0, 5000);
	setThrottle(bl1, 5000);

	if(CALIBRATION_DEBUG == 1) {
		printf("Wating 3sec\n");
	}

	delay(3000);

	if (CALIBRATION_DEBUG == 1) {
		printf("BLDC Min Ouput\n");
	}
	setThrottle(bl0, 0);
	setThrottle(bl1, 0);

	if (CALIBRATION_DEBUG == 1) {
		printf("BLDC Calibration Complete\b");
	}
	*/

}

void setDcCalibration() {
}


#endif
