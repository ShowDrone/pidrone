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
#include <RTIMULib.h>
#include "Sonar.h"
#include "Led.h"
#include <mosquitto.h>
#include "gps.h"
#include "Mqtt.h"

#define MAIN_DEBUG 1  // 루프 부분 디버그 메시지 출력 정의 1이면 출력, 0이면 미출력

using namespace std;
void  INThandler(int sig);

int main(int argc, char **argv)
{
	uint64_t rateTimer;
	uint64_t rateTimer_s;
	uint64_t sonarTimer;
	uint64_t tok, tokold;
	uint64_t toktime;
	int tokcnt = 0;
	uint64_t displayTimer;
	uint64_t now;
	RTIMU_DATA imuData;

	uint32_t ControlPeriod = 10000;   //10ms
	uint32_t ControlPeriod_s = 1000000; //1000ms
	uint32_t DisplayPeriod = 100000;  //100ms
	uint16_t fd;

	// SONAR ID
	uint16_t sonarid0 = 0x71;
	uint16_t sonarid1 = 0x72;
	uint16_t sonarid2 = 0x73;
	uint16_t sonarid3 = 0x74;
	char *imuresult;
	bool sonarflag = true;

	//interrupt for exit
	signal(SIGINT, INThandler);

	//init Sonar
	SONAR sonar0 = SONAR(sonarid0);
	//~ SONAR sonar1=SONAR(sonarid2); 
	//~ SONAR sonar2=SONAR(sonarid3);
	//~ SONAR sonar3=SONAR(sonarid4);

	sonar0.distance = 0;
	//~ sonar1.distance = 0;
	//~ sonar2.distance = 0;
	//~ sonar3.distance = 0;

	// wiringPi setup 실패시 프로그램 종료
	if (wiringPiSetup() == -1) {
		return 1;
	}

	//init imu
	RTIMUSettings *settings = new RTIMUSettings("RTIMULib");
	RTIMU *imu = RTIMU::createIMU(settings);

	if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
		printf("No IMU found\n");
		exit(1);
	}

	// imu는 RTIMU 클래스 속성을 가진 변수
	imu->IMUInit();       // 초기화
	imu->setSlerpPower(0.02); // 리딩 속도 0.02초
	imu->setGyroEnable(true); // 자이로 ON
	imu->setAccelEnable(true);  // 가속도 ON
	imu->setCompassEnable(true);// 지자계 ON

								//init led
	ledinit();

	//init GPS
	gps_init();
	loc_t gps;
	gps.latitude = 0;
	gps.longitude = 0;
	gps.altitude = 0;
	gps.speed = 0.0;
	gps.satellites = 0;

	//init mqtt
	mq_init();

	//init pwm
	pwid = pca9685Setup(pin_base, 0x40, pwmfreq); // ID 값 반환

	if (pwid < 0) {
		if (MAIN_DEBUG == 1)
			printf("error init setup\n");  // PCA 초기화 실패시 에러 발생
	}
	pca9685PWMReset(pwid);  // pwid로 pcb핀 리셋 

							//SET throttle
	setThrottle(bl0, 0);
	setThrottle(bl1, 0);

	//set angle
	setAngle(dc0, 0);
	setAngle(dc1, 0);
	setAngle(dc2, 0);
	setAngle(dc3, 0);
	setAngle(dcgm, 0);
	setAngle(dcgh, 0);
	setAngle(dcgp, 0);
	//setAngle(bz,255);
	delay(2000);      // 프로그램 안정화.

					  //init controller
	setLim(pitch, dc.min, dc.max);  // pitch OFFSET
	setLim(roll, dc.min, dc.max);   // roll  OFFSET
	setLim(yaw, -1, 1);       // yaw   OFFSET

							  // 차후 변수로 변경
	setGain(pitch, 8, 4, 4);        // pitch Gain set
	setGain(roll, 8, 4, 4);         // roll  Gain set
	setGain(yaw, 0.0008, 0.0004, 0.0004); // yaw   Gain set

										  //~ printf("new kp %f, ki %f, kd %f\r\n",pitch.kp,pitch.ki,pitch.kd);

	float ref = 0;
	int cnt = 0;


	// 아래부턴 LED 관련된 부분 
	// put all the characters of the scrolling text in a contiguous block
	setBuffer(led.setValue, displayBuffer, length);

	for (int j = 0; j < 8; j++) {
		printf("%u\r", j, 0, displayBuffer[j][0]);
	}

	//~ rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();
	rateTimer = displayTimer = micros();
	rateTimer_s = rateTimer;
	// LED SET 끝


	//mqtt start
	mq_start();


	// 무한 루프
	while (1) {
		//imu
		usleep(imu->IMUGetPollInterval() * 1000);
		tokold = micros();

		// imu 데이터 reading
		while (imu->IMURead());
		imuData = imu->getIMUData();

		// 각도를 문자열로 받아서, 분리하고 실수로 바꿔서 대입
		imuresult = (char *)RTMath::displayDegrees("", imuData.fusionPose);
		imuresult = strtok(imuresult, ":");
		imuresult = strtok(NULL, ",");
		roll.y = atof(imuresult);   // roll
		imuresult = strtok(NULL, ":");

		imuresult = strtok(NULL, ",");
		pitch.y = atof(imuresult);    // pitch

		imuresult = strtok(NULL, ":");
		imuresult = strtok(NULL, ":");
		yaw.y = atof(imuresult);      // yaw

		if (cnt == 10) {
			roll.r = 0;
			pitch.r = 0;
			yaw.r = yaw.y;
			roll.yo = roll.y;
			pitch.yo = pitch.y;
			yaw.yo = yaw.y;
		}

		// getInput 함수는 PID 함수로 보임 Rpi.h 164번째줄 참조 getinput(PID& pid, float rnew, float ynew)
		// setValue에 4를 대입할 경우엔 x, y, z all ON
		if (rpi.setValue == 1 || rpi.setValue == 4) {
			getInput(pitch, pitch.r, pitch.y);
			printf("pitch=%f,u=%f,up=%f,ui=%f,ud=%f,r=%f\r\n", pitch.y, pitch.u, pitch.up, pitch.ui, pitch.ud, pitch.r);
			dc.setValue = pitch.u;
			dc3.setValue = -pitch.u;
		}

		if (rpi.setValue == 2 || rpi.setValue == 4) {
			getInput(roll, roll.r, roll.y);
			printf("roll=%f,u=%f,up=%f,ui=%f,ud=%f,r=%f\r\n", roll.y, roll.u, roll.up, roll.ui, roll.ud, roll.r);
			dc2.setValue = roll.u;
			dc4.setValue = -roll.u;
		}

		if (rpi.setValue == 3 || rpi.setValue == 4) {
			getInput(yaw, yaw.r, yaw.y);
			printf("yaw=%f,u=%f,up=%f,ui=%f,ud=%f,r=%f\r\n", yaw.y, yaw.u, yaw.up, yaw.ui, yaw.ud, yaw.r);
			yawgain = yaw.u;
		}

		// Throttle
		// void setThrottle(BL& s,int input)
		setThrottle(bl0, bl0.setValue*(1 - yawgain));   // 프롭의 차로 yaw조절 (상단)
		setThrottle(bl1, bl1.setValue*(1 + yawgain)); // 프롭의 차로 yaw조절 (하단)

													  //set angle  void(DC& dc,float input)
		setAngle(dc0, dc0.setValue);
		setAngle(dc1, dc1.setValue);
		setAngle(dc2, dc2.setValue);
		setAngle(dc3, dc3.setValue);
		setAngle(cm0, cm0.setValue);
		setAngle(cm1, cm1.setValue);

		// 짐벌
		setAngle(dcgm, dcgm.setValue);
		setAngle(dcgh, dcgh.setValue);
		setAngle(dcgp, dcgp.setValue);


		//sonar 
		if (!sonarflag) {
			sonar0.RequestData(); // 데이터 요청
			sonarTimer = micros(); // 시간 저장
			sonarflag = true;
		}


		//led scroll
		//~ ledScroll(displayBuffer,length,letter,y);
		//~ printf("text is %s\r\n",led.setValue);
		ledDraw(displayBuffer2, led.setValue);

		//~ now =RTMath::currentUSecsSinceEpoch();      
		now = micros();
		// slow timer


		// GPS 센서 속도(1000ms)에 맞춰서 Reading
		if ((now - rateTimer_s) >= ControlPeriod_s) {
			//get gps Data
			gps_location(&gps);
			lBeep();
			rateTimer_s = now;
		}


		// 디스플레이 속도(100ms)에 맞춰서 Reading
		if ((now - displayTimer) > DisplayPeriod) {
			//~ printf("in main %lf, %lf \r\n",gps.latitude, gps.longitude);    
			//~ printf("Recievd sonar value : %u cm\n",sonar.distance);
			//~ 
			//~ printf("Recievd: %s\r\n",RTMath::displayDegrees("",imuData.fusionPose));

			if (MAIN_DEBUG == 1)
				sprintf(mqbuf, "%i,%i,%i\r", sonar0.id, sonar0.distance, sonar_status);
			mq_send("pidrone/SONAR", mqbuf);
			if (MAIN_DEBUG == 1)
				sprintf(mqbuf, "%lf,%lf,%lf,%i,%lf,%i\r", gps.latitude, gps.longitude, gps.altitude, gps.satellites, gps.speed*1.8, gps_status);
			mq_send("pidrone/GPS", mqbuf);
			if (MAIN_DEBUG == 1)
				sprintf(mqbuf, "%s,%i\r", RTMath::displayDegrees("", imuData.fusionPose), imu_status);
			mq_send("pidrone/IMU", mqbuf);

			toktime = toktime / tokcnt;
			tokcnt = 0;
			//~ toktime=0;

			if (MAIN_DEBUG == 1)
				sprintf(mqbuf, "measured time is %llds.\r\n", toktime);
			mq_send("pidrone/PI", mqbuf);

			fflush(stdout);
			displayTimer = now;
		}

		tok = micros();
		toktime = toktime + tok - tokold;
		tokcnt++;

		// 10ms마다 한번씩 동작 (초음파)
		while ((now - rateTimer) < ControlPeriod) {
			//~ usleep(40000); //5ms for under 60cm 20ms for under 100cm 40ms for 200cm
			// 초음파 속도에 맞춰 Reading
			if (((now - sonarTimer) >= 60000) && sonarflag) {
				sonar0.distance = sonar0.GetValues();
				sonarflag = false;
			}
			//~ now=RTMath::currentUSecsSinceEpoch();
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
	setAngle(cm1, 0);
	setAngle(cm2, 0);
	setAngle(dcgm, 0);
	setAngle(dcgh, 0);
	setAngle(dcgp, 0);

	pca9685PWMReset(pwid);

	setAngle(bz, 255);
	setThrottle(bl0, 0);
	setThrottle(bl1, 0);

	signal(sig, SIG_IGN);

	close(file);
	exit(0);
}




