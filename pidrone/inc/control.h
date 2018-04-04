
#ifndef CONTROL_H
#define CONTROL_H

#include <Rpi.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <math.h>

extern void setI2C (byte _id0, byte _id1);
extern void transmite_SlaveA();
extern void transmite_SlaveB();
extern void request_SlaveA();
extern void request_SlaveB()

typedef struct En {
  float setValue;
  float deg;
	float encoderPos;
	float encoderPosDecimal;
	STATUS stat;
} EN;

EN en0 = {0, 0, 0, OK};
EN en1 = {0, 0, 0, OK};

typedef struct Azimuth {
  byte low;
  byte high;
  byte decimal;
  float final;
} AZIMUTH;

AZIMUTH myAzimuth = {0, 0, 0, 0};
AZIMUTH targetAzimuth = {0, 0, 0, 0;

typedef struct Sm {
  byte pos;
	STATUS stat;
} SM;

SM sm0 = {0, OK};
SM sm1 = {0, OK};

typedef struct Dc {
	float setValue;
	float min;
	float max;
	float tmin;
	float tmax;
	STATUS stat;
} DC;

// pca9685, 아두이노? 처리문제 확인 일단 제거.
DC dcgm = { 0,-90,90,0.5,2.5,OK };  //  mode of gymbal
DC dcgp = { 0,-90,90,0.5,2.5,OK };    //  gymbal pitch
DC dcgh = { 0,-90,90,0.5,2.5,OK };    //  gymbal heading


void setAngle(DC& dc, float input) {	// ������ 0.5~2.5
	float mils = map(input, dc.min, dc.max, dc.tmin, dc.tmax);
	int tick = calcTicks(mils, pwmfreq);// 50Hz
	int id = dc.pwmid + pin_base; // id 0-15, 16 all pin
	pwmWrite(id, tick); // 0~4095 
	//~ printf("id=%i,tick=%i\r\n",id,tick);
}


extern float bldcSpeed;
extern bool servoMode;
extern bool autoPilotMode;
extern byte yawLow;
extern byte yawHigh;
extern byte yawDecimal;
extern byte enTargetPosA;
extern byte enTargetPosA;
extern int slaveA_id;
extern int slaveB_id;