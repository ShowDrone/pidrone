#ifndef RPI_H
#define RPI_H

#include <pca9685.h>
#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	OK,
	ERROR
} STATUS;


typedef struct Rpi {
	int setValue;
	STATUS stat;
} RPI;

typedef struct Dc {
	int id;
	float setValue;
	float min;
	float max;
	float tmin;
	float tmax;
	STATUS stat;
} DC;

typedef struct Bl {
	int id;
	int setValue;
	int min;
	int max;
	float tmin;
	float tmax;
	STATUS stat;
} BL;

typedef struct Led {
	char *setValue;
	STATUS stat;
} LED;



typedef struct Pid {
	float kp;
	float ki;
	float kd;
	float wi;
	float wd;
	float T;
	float up;
	float ui;
	float ud;
	float u;
	float umin;
	float umax;
	float r;
	float y;
	float yo;
} PID;

typedef struct Plant {
	float T;
	float a;
	float b;
	float u;
	float y;
	float yold;
} PLANT;


STATUS gps_status, imu_status, sonar_status;
LED led;
RPI rpi = { 0,OK };

DC dcgm = { 1,-90,-90,90,0.5,2.5,OK };  //  mode of gymbal
DC dcgp = { 2,0,-90,90,0.5,2.5,OK };    //  gymbal pitch
DC dcgh = { 3,0,-90,90,0.5,2.5,OK };    //  gymbal heading

DC dc0 = { 4,0,-90,90,0.5,2.5,OK };
DC dc1 = { 5,0,-90,90,0.5,2.5,OK };
DC dc2 = { 6,0,-90,90,0.5,2.5,OK };
DC dc3 = { 7,0,-90,90,0.5,2.5,OK };
DC cm0 = { 10,0,-90,90,1,2,OK };
DC cm1 = { 11,0,-90,90,1,2,OK };
DC bz = { 15,0,0,255,0,50,OK };

BL bl0 = { 8,0,5000,0,2,0.7,OK };
BL bl1 = { 9,0,5000,0,2,0.7,OK };
PID pitch = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PID roll = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PID yaw = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PLANT plant = { 0.1,10,100,0,0,0 };

int pwid;
int pin_base = 300; // ID에 더하는 값
int max_pwm = 4096;
int pwmfreq = 50; // PWM 출력 속도
int startCount = 0;
int endCount = 6;
float yawgain = 0.0;
int bloffset = 0;
float jgainr = 15;
float jgainp = 15;
float jgainy = 5;
float jgainb = 2000;
float jgaingp = 90;
float jgaingh = 90;
char *user = "pi";      // Raspberry Pi ID
char *pw = "vkdlemfhs";   // Raspberry Pi Password


void setGain(PID& pid, float pnew, float inew, float dnew) {

	//~ s.kp=s.mbar*s.kv/s.T;
	//~ s.ki=s.mbar*s.k/s.T;
	//~ s.kd=s.mbar/s.T;
	pid.kp = pnew;
	pid.ki = inew;
	pid.kd = dnew;

}

void setLim(PID& pid, float uminNew, float umaxNew) {
	pid.umax = umaxNew;
	pid.umin = uminNew;
}

void getInput(PID& pid, float rNew, float yNew) {
	float eold = pid.r - (pid.y - pid.yo);
	float e = rNew - (yNew - pid.yo);

	pid.r = rNew;
	pid.y = yNew;
	pid.up = pid.kp*e;
	pid.ui = pid.ui / (1 + pid.wi*pid.T) + pid.ki*pid.wi*pid.T*e / (1 + pid.wi*pid.T);
	pid.ud = pid.ud / (1 + pid.wd*pid.T) + pid.kd*pid.wd*(e - eold) / (1 + pid.wd*pid.T);
	pid.u = pid.up + pid.ui + pid.ud;

	// out of range
	if (pid.u > pid.umax) {
		pid.u = pid.umax;
	}
	else if (pid.u < pid.umin) {
		pid.u = pid.umin;
	}
}

int calcTicks(float ms, int hertz) {
	float cycle = 1000.0f / hertz;
	return (int)(max_pwm*ms / cycle + 0.5f);
}
float map(float input, float min, float max, float pmin, float pmax) {
	float target = input - min;
	return pmin + (pmax - pmin) / (max - min)*target;
}

void setThrottle(BL& bl, int input) {
	float mils = map((float)input, bl.min, bl.max, bl.tmin, bl.tmax);
	int tick = calcTicks(mils, pwmfreq);// 50Hz
	int id = bl.id + pin_base; // id 0-15, 16 all pin
	pwmWrite(id, tick);
	//~ printf("id=%i,tick=%i\r\n",id,tick);
}

void setAngle(DC& dc, float input) {
	float mils = map(input, dc.min, dc.max, dc.tmin, dc.tmax);
	int tick = calcTicks(mils, pwmfreq);// 50Hz
	int id = dc.id + pin_base; // id 0-15, 16 all pin
	pwmWrite(id, tick);
	//~ printf("id=%i,tick=%i\r\n",id,tick); 
}

void lBeep() {
	if (endCount > 0) {
		if (startCount < endCount) {
			if (startCount % 2 == 0)
				setAngle(bz, 0);    // buz on                                                                                                                                                                                        
			else
				setAngle(bz, 255); // buz off
			startCount++;
		}
		else {
			startCount = 0;
			endCount = 0;
		}
	}
}

#endif
