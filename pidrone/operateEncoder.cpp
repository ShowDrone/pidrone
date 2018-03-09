
#include <wiringPi.h>
#include <time.h>
#include <stdio.h>

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


void dcEncoderA() {
	dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? 1 : -1;
	printf("PinA : %d, encoder :%d\n", dc0.encoderA, dc0.encoderPos);
}


void dcEncoderB() {
	dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? -1 : 1;
	printf("PinB : %d, encoder : %d\n", dc0.encoderB, dc0.encoderPos);
}

void doMotor(bool dir, int val) {
	digitalWrite(dc0.dirId, dir);
	pwmWrite(dc0.pwmId)

}

float ratio = 360./420.;
DC dc0 = { 4,5,17,18,0,-90,90,0.5,2.5,OK }; // 4,5 are pca pin,  bcm17,18 are physical pin 11,12 

float ratio = 360. / 270. / 64.;

int main() {

	pinMode(dc0.encoderA, INPUT);
	pinMode(dc0.encoderB, INPUT);

	dc0.encoderPos = 0;


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
	
	while (1) {
		float motorDeg = float(dc0.encoderPos) * ratio;
	}

}
