/*
* main.cxx
*
* Copyright 2017  <pi@raspberrypi>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA 02110-1301, USA.
*
*
* Commented by Seunghyun Lee
* Code modification by Seunghyun Lee
*
*/

#include "Rpi.h"
#include "Sonar.h"
#include "Led.h"
#include <mosquitto.h>
#include "gps.h"
#include "Mqtt.h"
#include "Calibration.h"

#define MAIN_DEBUG 1  // Debug msg output
#define CALI 0		  // execute Calibration
using namespace std;
void  INThandler(int sig);

int main(int argc, char **argv)
{
	uint64_t rateTimer;
	uint64_t rateTimer_s;
	uint64_t sonarTimer;
	uint64_t tok, tokold;
	uint64_t toktime;
	int tokcnt = 1;
	uint64_t displayTimer;
	uint64_t now;

	uint32_t ControlPeriod = 10000;		//10ms
	uint32_t ControlPeriod_s = 1000000; //1000ms
	uint32_t DisplayPeriod = 100000;	//100ms
	uint16_t fd;
	float ref = 0;
	int cnt = 0;
	bool sonarFlag = true;

	// interrupt for exit
	signal(SIGINT, INThandler);

	// init Encoder Pin
	pinMode(dc0.encoderA, INPUT);
	pinMode(dc0.encoderB, INPUT);
	pinMode(dc1.encoderA, INPUT);
	pinMode(dc1.encoderB, INPUT);
	pinMode(dc2.encoderA, INPUT);
	pinMode(dc2.encoderB, INPUT);
	pinMode(dc3.encoderA, INPUT);
	pinMode(dc3.encoderB, INPUT);

	// Init Sonar ID
	uint16_t sonarid0 = 0x71;
	uint16_t sonarid1 = 0x72;
	SONAR sonar0 = SONAR(sonarid0);
	SONAR sonar1 = SONAR(sonarid1);
	sonar0.distance = 0;
	sonar1.distance = 0;


	if (MAIN_DEBUG == 1) {
		printf("\nPROGRAM START\n");
	}

	
	// Setup wiringPi
	if (wiringPiSetup() < 0) {
		fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
		return 1;
	}

	if (wiringPiISR(dc0.encoderA, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc0.encoderA ISR error occured\n", stderr);
		return 1;
	}

	if (wiringPiISR(dc0.encoderB, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc0.encoderB ISR error occured\n", stderr);
		return 1;
	}

	if (wiringPiISR(dc1.encoderA, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc1.encoderA ISR error occured\n", stderr);
		return 1;
	}

	if (wiringPiISR(dc1.encoderB, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc1.encoderB ISR error occured\n", stderr);
		return 1;
	}
	if (wiringPiISR(dc2.encoderA, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc2.encoderA ISR error occured\n", stderr);
		return 1;
	}

	if (wiringPiISR(dc2.encoderB, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc2.encoderB ISR error occured\n", stderr);
		return 1;
	}
	
	if (wiringPiISR(dc3.encoderA, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc3.encoderA ISR error occured\n", stderr);
		return 1;
	}

	if (wiringPiISR(dc3.encoderB, INT_EDGE_BOTH, dc0EncoderA) < 0) {
		fputs("dc3.encoderB ISR error occured\n", stderr);
		return 1;
	}


	// Init LED
	if (MAIN_DEBUG == 1) {
		printf("Init Led Start\n");
	}

	ledinit();
	setLed();
	setBuffer(led.setValue, displayBuffer, length);

	for (int j = 0; j < 8; j++) {
		printf("%u\r", j, 0, displayBuffer[j][0]);
	}

	rateTimer = displayTimer = micros();
	rateTimer_s = rateTimer;

	if(MAIN_DEBUG == 1) {
		printf("Init Led Complete\n");
	}

	// Init GPS
	gps_init();
	loc_t gps;
	gps.latitude = 0;
	gps.longitude = 0;
	gps.altitude = 0;
	gps.speed = 0.0;
	gps.satellites = 0;

	if(MAIN_DEBUG == 1) {
		printf("Init Gps Complete\n");
	}
	

	
	// Init MQTT

	if (MAIN_DEBUG == 1) {
		printf("Init Start\n");
	}

	mq_init();
	mq_start();

	if(MAIN_DEBUG == 1) {
		printf("Init Mqtt Complete\n");
	}

	
	// Init PCA9685
	if (MAIN_DEBUG == 1) {
		printf("Init Pca9685 Start\n");
	}

	pwid = pca9685Setup(pin_base, 0x40, pwmfreq); // return ID value
	if (pwid < 0) {
		if (MAIN_DEBUG == 1)
			printf("Error init setup\n");  // if pca9685Setup fails
	}
	pca9685PWMReset(pwid);  // Reset pcb9685 pin to pwid


	if(MAIN_DEBUG == 1) {
		printf("Init Pca9685 Complete\n");
	}

	if(CALI == 1) {
		setBldcCalibration(0); // 1 = output debug msg, 0 = no output
		delay(1000);
	}


	FILE *fp;
	fp = fopen("/home/pi/Code/pidrone/src/pidgain.txt", "a");
	if (fp == NULL) {
		fprintf(stderr,"Can't open pidgain.txt file\n");
		// add basci pidgain code
	}
	while (!feof(fp)) {
		fscanf(fp, "%f%f%f", &kp, &ki, &kd);
	}


	if (MAIN_DEBUG == 1) {
		printf("Set Dc&Bldc start\n");
	}

	// set BLDC Motor
	setThrottle(bl0, 0);
	setThrottle(bl1, 0);

	
	// set DC Motor
	setAngle(dc0, 0);
	setAngle(dc1, 0);
	setAngle(dc2, 0);
	setAngle(dc3, 0);
	setAngle(dcgm, 0);
	setAngle(dcgh, 0);
	setAngle(dcgp, 0);
	//~ setAngle(bz,255);
	delay(2000);     


	if(MAIN_DEBUG == 1) {
		printf("Set Dc&Bldc Complete\n");
	}


	if (MAIN_DEBUG == 1) {
		printf("Enter the Loop\n");
	}


	// ���� ����
	while (1) {
		tokold = micros();

		if (rpi.setValue == 1 || rpi.setValue == 4) {
			
			// Add control data read

			//~ dc0.setValue = ;
			//~ dc2.setValue = ;

			if (MAIN_DEBUG == 1) {
				printf("dc0.setValue=\f, dc2,setValue=\f\r\n", dc0.setValue, dc2.setValue);
			}
			
		}

		if (rpi.setValue == 2 || rpi.setValue == 4) {
	
			// Add control data read

			//~ dc1.setValue = ;
			//~ dc3.setValue = ;

			if (MAIN_DEBUG == 1) {
				printf("dc1.setValue=\f, dc3,setValue=\f\r\n", dc1.setValue, dc3.setValue);
			}
		}

		if (rpi.setValue == 3 || rpi.setValue == 4) {

			// Add control data read

			//~ yawgain = ;

			if (MAIN_DEBUG == 1) {
				printf("yawgain=\f\r\n", yawgain);
			}
		}

		// Edit value value

		// BLDC Motor
		setThrottle(bl0, bl0.setValue*(1 - yawgain)); // Yaw adjustment by prop's difference (Top)
		setThrottle(bl1, bl1.setValue*(1 + yawgain)); // Yaw adjustment by prop's difference (Bottom)


		// DC Motor
		setAngle(dc0, dc0.setValue);
		setAngle(dc1, dc1.setValue);
		setAngle(dc2, dc2.setValue);
		setAngle(dc3, dc3.setValue);
		setAngle(cm0, cm0.setValue);
		setAngle(cm1, cm1.setValue);

		// Gymbol Motor
		setAngle(dcgm, dcgm.setValue);
		setAngle(dcgh, dcgh.setValue);
		setAngle(dcgp, dcgp.setValue);


		// Sonar
		if (!sonarFlag) {
			sonar0.RequestData();  // Request Data
			sonarTimer = micros(); // Save Time
			sonarFlag = true;
		}
		

		// Led Scroll
		//~ ledScroll(displayBuffer,length,letter,y);
		ledDraw(displayBuffer2, led.setValue);
		now = micros();


		// Read to gps sensor speed (1000ms)
		if ((now - rateTimer_s) >= ControlPeriod_s) {
			//gps_location(&gps);
			lBeep();
			rateTimer_s = now;
		}



		// Data dsiplay (100ms)
		if ((now - displayTimer) > DisplayPeriod) {

			//~ printf("in main %lf, %lf \r\n",gps.latitude, gps.longitude);    
			
			//~ printf("Recievd sonar value : %u cm\n",sonar0.distance);
			//~ printf("Recievd: %s\r\n",RTMath::displayDegrees("",imuData.fusionPose));

			//sprintf(mqbuf, "%i,%i,%i\r", sonar0.id, sonar0.distance, sonar_status);
			mq_send("pidrone/SONAR", mqbuf);
			//sprintf(mqbuf, "%lf,%lf,%lf,%i,%lf,%i\r", gps.latitude, gps.longitude, gps.altitude, gps.satellites, gps.speed*1.8, gps_status);
			mq_send("pidrone/GPS", mqbuf);


			toktime = toktime / tokcnt;
			tokcnt = 0;

			printf(mqbuf, "measured time is %llds.\r\n", toktime);
			mq_send("pidrone/PI", mqbuf);

			fflush(stdout);
			displayTimer = now;
		}

		tok = micros();
		toktime = toktime + tok - tokold;
		tokcnt++;

		// Operate every 10ms
		while ((now - rateTimer) < ControlPeriod) {
			//~ usleep(40000); //5ms for under 60cm 20ms for under 100cm 40ms for 200cm
			// Read to sonar sensor speed
			if (((now - sonarTimer) >= 60000) && sonarFlag) {
				sonar0.distance = sonar0.GetValues();
				sonarFlag = false;
			}
			now = micros();
		}
		rateTimer = now;
		cnt++;
	}
	return 0;
}

void  INThandler(int sig) {
	// Closing file and turning off Matrix
	unsigned short int clear[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	displayImage(clear, res, daddress, file);
	printf("Closing file and turning off \r\n");
	daddress = 0x20;
	for (daddress = 0xef; daddress >= 0xe0; daddress--) {
		res = i2c_smbus_write_byte(file, daddress);
	}
	gps_off();

	setAngle(dc0, 0);
	setAngle(dc1, 0);
	setAngle(dc2, 0);
	setAngle(dc3, 0);
	setAngle(cm0, 0);
	setAngle(cm1, 0);
	setAngle(dcgm, 0);
	setAngle(dcgh, 0);
	setAngle(dcgp, 0);

	pca9685PWMReset(pwid);

	setAngle(bz, 255);
	setThrottle(bl0, 0);
	setThrottle(bl1, 0);

	signal(sig, SIG_IGN);

	fclose(fp);

	exit(0);
}