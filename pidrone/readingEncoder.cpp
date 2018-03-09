
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
	float ratio = 360./420.; // ����, ȸ��������/���ӱ����/1ȸ���޽� �ѹ����� 64�޽� ���ӱ⿡���� 270������,1������ 17280 �޽�,0.020�������
	STATUS stat;
} DC;

DC dc0 = { 4,5,17,18,0,-90,90,0.5,2.5,OK }; // 4,5 are pca pin,  bcm17,18 are physical pin 11,12 


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
		delay(500);
	}

}


void dcEncoderA() {
	dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? 1 : -1;
	printf("PinA : %d, encoder :%d\n", dc0.encoderA, dc0.encoderPos);
}


void dcEncoderB() {
	dc0.encoderPos += (digitalRead(dc0.encoderA) == digitalRead(dc0.encoderB)) ? -1 : 1;
	printf("PinB : %d, encoder : %d\n", dc0.encoderB, dc0.encoderPos);
}