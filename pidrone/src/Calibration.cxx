// Calibration.cxx

#include "Calibration.h"
#include <stdio.h>

#define CALIBRATION_DEBUG 1

void setBldcCalibration() {

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

}

void setDcCalibration() {
}
