#ifndef RPI_H
#define RPI_H

#include <pca9685.h>
#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	OK,
	ERROR
} STATUS;


typedef struct Rpi {
	int setValue;
	STATUS stat;
} RPI;

typedef struct Dc {
	const int pwmId;
	const int dirId;
	const int encoderA;
	const int encoderB;
	float setValue;
	float min;
	float max;
	float tmin;
	float tmax;
	float encoderPos;
	STATUS stat;
} DC;


typedef struct Bl {
	const int id;
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
	float p;
	float i;
	float d;
	float err;
	float err_prev;
	float de;
	float dt;
	float control;
	float time_prev;
	float targerDeg = 0;
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
LED led = { 0,OK };
RPI rpi = { 0,OK };

DC dcgm = { 1,-90,-90,90,0.5,2.5,OK };  //  mode of gymbal
DC dcgp = { 2,0,-90,90,0.5,2.5,OK };    //  gymbal pitch
DC dcgh = { 3,0,-90,90,0.5,2.5,OK };    //  gymbal heading

DC dc0 = { 4,5,17,18,0,-90,90,0.5,2.5,OK }; // 4,5 are pca pin,  17,18 are physical pin 11,12
DC dc1 = { 6,7,27,22,0,-90,90,0.5,2.5,OK }; // 6,7 are pca pin,  17,18 are physical pin 13,15
DC dc2 = { 8,9,23,24,0,-90,90,0.5,2.5,OK }; // 8,9 are pca pin,  23,24 are physical pin 16,18
DC dc3 = { 10,11,25,4,0,-90,90,0.5,2.5,OK };// 10,11 are pca pin,  25,4 are physical pin 22,7
DC cm0 = { 12,0,0,-90,90,1,2,OK };
DC cm1 = { 13,0,0,-90,90,1,2,OK };
DC bz = { 14,0,0,0,255,0,50,OK };


// 핀 부족
BL bl0 = { 8,0,0,5000,0.7,2,OK };
BL bl1 = { 9,0,0,5000,0.7,2,OK };

PID pitch = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PID roll = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PID yaw = { 8,4,4,6,628,0.01,0,0,0,0,1,2,0,0,0 };
PLANT plant = { 0.1,10,100,0,0,0 };


float ratio = 360. / 270. / 64.; // 예시, 회전각도값/감속기비율/1회전펄스 한바퀴에 64펄스 감속기에의해 270번바퀴,1바퀴에 17280 펄스,0.020도제어가능
int pwid;
int pin_base = 300; // Value to add to ID
int max_pwm = 4096;
int pwmfreq = 50;	// PWM Ouput Speed
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
char *pw = "vkdlemfhs"; // Raspberry Pi Password

void pidControl(PID& pid, Dc& dc, float targetDeg) {
	float motorDeg = dc.encoderPos * dc.ratio;

	pid.err = targetDeg - motorDeg;
	pid.de = pid.err - pid.err_prev;
	pid.dt = micros() - pid.time_prev;

	pid.p = pid.err * pid.kp;
	pid.i = pid.i + (pid.err*pid.ki) * pid.dt;
	pid.d = pid.kd * pid.de / pid.dt;
	pid.control = pid.p + pid.i + pid.d;

	pid.err_prev = pid.err;
	pid.time_prev = micros();
}

/*
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
}*/

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

// 이름 setServo로 변경 요망
// 라즈베리파이에서 확인요망
void setAngle(DC& dc, float input) {	// 기존값 0.5~2.5
	float mils = map(input, dc.min, dc.max, dc.tmin, dc.tmax);
	// 각도 -> 펄스길이 변환
	int tick = calcTicks(mils, pwmfreq);// 50Hz
	int id = dc.pwmid + pin_base; // id 0-15, 16 all pin
	pwmWrite(id, tick); // 0~4095 
	//~ printf("id=%i,tick=%i\r\n",id,tick);
}

// Interrupt func
void dc0EncoderA() { dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? 1 : -1; }
void dc0EncoderB() { dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? -1 : 1; }
void dc1EncoderA() { dc1.encoderPos += (digitalRead(dc1.encoderA) == digitalRead(dc1.encoderB)) ? 1 : -1; }
void dc1EncoderB() { dc1.encoderPos += (digitalRead(dc1.encoderA) == digitalRead(dc1.encoderB)) ? -1 : 1; }
void dc2EncoderA() { dc2.encoderPos += (digitalRead(dc2.encoderA) == digitalRead(dc2.encoderB)) ? 1 : -1; }
void dc2EncoderB() { dc2.encoderPos += (digitalRead(dc2.encoderA) == digitalRead(dc2.encoderB)) ? -1 : 1; }
void dc3EncoderA() { dc3.encoderPos += (digitalRead(dc3.encoderA) == digitalRead(dc3.encoderB)) ? 1 : -1; }
void dc3EncoderB() { dc3.encoderPos += (digitalRead(dc3.encoderA) == digitalRead(dc3.encoderB)) ? -1 : 1; }


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
