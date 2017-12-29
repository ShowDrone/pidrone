#ifndef MQTT_H
#define MQTT_H

#include <mosquitto.h>
#include "Rpi.h"
#include <string.h>
#include <cstdlib>
#include "Led.h"

#define MQTT_DEBUG 1  // MQTT 부분 디버그 메시지 출력 정의 1이면 출력, 0이면 미출력

struct mosquitto *mosq = NULL;
char *mqbuf = (char *)malloc(70 * sizeof(char));
int mq_send(const char *topic, const char *msg);

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {

	bool match = 0;
	char *msg;
	char *s1;
	char *s2;
	int getid;
	float getValue;

	mosquitto_topic_matches_sub("pidrone/CMD/PI", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		if (MQTT_DEBUG == 1)
			printf("pi command= %i\r\n", atoi(msg));

		rpi.setValue = atoi(msg);

		if (rpi.setValue == 0) {
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
			setThrottle(bl0, 0);
			setThrottle(bl1, 0);
			setAngle(bz, 255);
		}
	}

	mosquitto_topic_matches_sub("pidrone/CMD/DC", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		s1 = strtok(msg, ",");
		s2 = strtok(NULL, "\r");
		getid = atoi(s1);
		getValue = atof(s2);
		//~ printf("getid= %i getValue=%.1f\r\n",getid,getValue);
		//~ printf("getid= %i getValue=%.1f\r\n",dc.id,getValue); 
		//~ if(getid==dc.id)

		if (getid == 1) {
			dcgm.setValue = getValue;
			if(MQTT_DEBUG == 1)
				printf("gymbal mode id= %i setValue=%.1f\r\n", dcgm.id, dcgm.setValue);
		}
		else if (getid == 2) {
			dcgp.setValue = getValue;
			if(MQTT_DEBUG == 1)
				printf("gymbal pitch id= %i setValue=%.1f\r\n", dcgp.id, dcgp.setValue);
		}
		else if (getid == 3) {
			dcgh.setValue = getValue;
			if(MQTT_DEBUG == 1)
				printf("gymbal head id= %i setValue=%.1f\r\n", dcgh.id, dcgh.setValue);
		}
		else if (getid == 4) {
			dc0.setValue = getValue;
			if(MQTT_DEBUG == 1)
				printf("dc0 id= %i setValue=%.1f\r\n", dc0.id, dc0.setValue);
		}
		else if (getid == 5) {
			dc1.setValue = getValue;
			if (MQTT_DEBUG == 1)
				printf("dc1 id= %i setValue=%.1f\r\n", dc1.id, dc1.setValue);
		}
		else if (getid == 6) {
			dc2.setValue = getValue;
			if (MQTT_DEBUG == 1)
				printf("dc2 id= %i setValue=%.1f\r\n", dc2.id, dc2.setValue);
		}
		else if (getid == 7) {
			dc3.setValue = getValue;
			if (MQTT_DEBUG == 1)
				printf("dc3 id= %i setValue=%.1f\r\n", dc3.id, dc3.setValue);
		}
		else if (getid == 10) {
			cm0.setValue = getValue;
			if (MQTT_DEBUG == 1)
				printf("cm0 id= %i setValue=%.1f\r\n", cm0.id, cm0.setValue);
		}
		else if (getid == 11) {
			cm1.setValue = getValue;
			if (MQTT_DEBUG == 1)
				printf("cm1 id= %i setValue=%.1f\r\n", cm1.id, cm1.setValue);
		}
	}

	mosquitto_topic_matches_sub("pidrone/CMD/BL", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		s1 = strtok(msg, ",");
		s2 = strtok(NULL, "\r");
		getid = atoi(s1);
		getValue = atof(s2);
		if (getid == 8) {
			bl0.setValue = (int)getValue;
			if (MQTT_DEBUG == 1)
				printf("bldc0 id %i command=%i\r\n", bl0.id, bl0.setValue);
		}
		if (getid == 9) {
			bl1.setValue = (int)getValue;
			if (MQTT_DEBUG == 1)
				printf("bldc1 %i command=%i\r\n", bl1.id, bl1.setValue);
		}
	}

	mosquitto_topic_matches_sub("pidrone/CMD/MV", message->topic, &match);
	if (match) {
		rpi.setValue = 4;
		msg = (char *)message->payload;
		s1 = strtok(msg, ",");
		roll.r = atof(s1)*jgainr;
		s1 = strtok(NULL, ",");
		pitch.r = atof(s1)*jgainp;
		s1 = strtok(NULL, ",");
		yaw.r = yaw.r + jgainy*atof(s1);
		if (yaw.r>180.0) {
			yaw.r = 180.0;
		}
		if (yaw.r<-180.0) {
			yaw.r = -180.0;
		}
		s1 = strtok(NULL, ",");
		bl0.setValue = (int)(atof(s1)*jgainb);
		bl1.setValue = bl0.setValue;
		s1 = strtok(NULL, ",");
		dcgp.setValue = (atof(s1)*jgaingp);
		s1 = strtok(NULL, "\r");

		dcgh.setValue = (atof(s1)*jgaingh);

		printf("input : roll %f, pitch %f, yaw %f, bl %i, gp %f, gh %f\r\n", roll.r, pitch.r, yaw.r, bl0.setValue, dcgp.setValue, dcgh.setValue);
	}

	mosquitto_topic_matches_sub("pidrone/CMD/FG/get", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		s1 = strtok(msg, "");
		if (atoi(s1) == 0) {
			// 요청에 의한 출력이라 디버그에 의한 출력 무시처리를 하지 않음.
			printf("dc0 max=%f,min=%f,tmax=%f,tmin=%f\r\n", dc0.max, dc0.min, dc0.tmax, dc0.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", dc0.id, dc0.max, dc0.min, dc0.tmax, dc0.tmin);
			mq_send("pidrone/FG", mqbuf);
			printf("dc1 max=%f,min=%f,tmax=%f,tmin=%f\r\n", dc1.max, dc1.min, dc1.tmax, dc1.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", dc1.id, dc1.max, dc1.min, dc1.tmax, dc1.tmin);
			mq_send("pidrone/FG", mqbuf);
			printf("dc2 max=%f,min=%f,tmax=%f,tmin=%f\r\n", dc2.max, dc2.min, dc2.tmax, dc2.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", dc2.id, dc2.max, dc2.min, dc2.tmax, dc2.tmin);
			mq_send("pidrone/FG", mqbuf);
			printf("dc3 max=%f,min=%f,tmax=%f,tmin=%f\r\n", dc3.max, dc3.min, dc3.tmax, dc3.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", dc3.id, dc3.max, dc3.min, dc3.tmax, dc3.tmin);
			mq_send("pidrone/FG", mqbuf);

			printf("bl0 max=%i,min=%i,tmax=%f,tmin=%f\r\n", bl0.max, bl0.min, bl0.tmax, bl0.tmin);
			sprintf(mqbuf, "%u,%i,%i,%f,%f\r\n", bl0.id, bl0.max, bl0.min, bl0.tmax, bl0.tmin);
			mq_send("pidrone/FG", mqbuf);
			printf("bl1 max=%i,min=%i,tmax=%f,tmin=%f\r\n", bl1.max, bl1.min, bl1.tmax, bl1.tmin);
			sprintf(mqbuf, "%u,%i,%i,%f,%f\r\n", bl1.id, bl1.max, bl1.min, bl1.tmax, bl1.tmin);
			mq_send("pidrone/FG", mqbuf);

			printf("cm0 max=%f,min=%f,tmax=%f,tmin=%f\r\n", cm0.max, cm0.min, cm0.tmax, cm0.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", cm0.id, cm0.max, cm0.min, cm0.tmax, cm0.tmin);
			mq_send("pidrone/FG", mqbuf);
			printf("cm1 max=%f,min=%f,tmax=%f,tmin=%f\r\n", cm1.max, cm1.min, cm1.tmax, cm1.tmin);
			sprintf(mqbuf, "%u,%f,%f,%f,%f\r\n", cm1.id, cm1.max, cm1.min, cm1.tmax, cm1.tmin);
			mq_send("pidrone/FG", mqbuf);
		}
	}

	mosquitto_topic_matches_sub("pidrone/CMD/FG/set", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		s1 = strtok(msg, ",");
		getid = atoi(s1);
		if (getid == 4) {
			s1 = strtok(NULL, ",");
			dc0.max = atof(s1);
			s1 = strtok(NULL, ",");
			dc0.min = atof(s1);
			s1 = strtok(NULL, ",");
			dc0.tmax = atof(s1);
			s1 = strtok(NULL, "");
			dc0.tmin = atof(s1);
		}

		if (getid == 5) {
			s1 = strtok(NULL, ",");
			dc1.max = atof(s1);
			s1 = strtok(NULL, ",");
			dc1.min = atof(s1);
			s1 = strtok(NULL, ",");
			dc1.tmax = atof(s1);
			s1 = strtok(NULL, "");
			dc1.tmin = atof(s1);
		}

		if (getid == 6) {
			s1 = strtok(NULL, ",");
			dc2.max = atof(s1);
			s1 = strtok(NULL, ",");
			dc2.min = atof(s1);
			s1 = strtok(NULL, ",");
			dc2.tmax = atof(s1);
			s1 = strtok(NULL, "");
			dc2.tmin = atof(s1);
		}

		if (getid == 7) {
			s1 = strtok(NULL, ",");
			dc3.max = atof(s1);
			s1 = strtok(NULL, ",");
			dc3.min = atof(s1);
			s1 = strtok(NULL, ",");
			dc3.tmax = atof(s1);
			s1 = strtok(NULL, "");
			dc3.tmin = atof(s1);
		}

		if (getid == 8) {
			s1 = strtok(NULL, ",");
			bl0.max = atoi(s1);
			s1 = strtok(NULL, ",");
			bl0.min = atoi(s1);
			s1 = strtok(NULL, ",");
			bl0.tmax = atof(s1);
			s1 = strtok(NULL, "");
			bl0.tmin = atof(s1);
		}

		if (getid == 9) {
			s1 = strtok(NULL, ",");
			bl1.max = atoi(s1);
			s1 = strtok(NULL, ",");
			bl1.min = atoi(s1);
			s1 = strtok(NULL, ",");
			bl1.tmax = atof(s1);
			s1 = strtok(NULL, "");
			bl1.tmin = atof(s1);
		}
		if (getid == 10) {
			s1 = strtok(NULL, ",");
			cm0.max = atof(s1);
			s1 = strtok(NULL, ",");
			cm0.min = atof(s1);
			s1 = strtok(NULL, ",");
			cm0.tmax = atof(s1);
			s1 = strtok(NULL, "");
			cm0.tmin = atof(s1);
		}
		if (getid == 11) {
			s1 = strtok(NULL, ",");
			cm1.max = atof(s1);
			s1 = strtok(NULL, ",");
			cm1.min = atof(s1);
			s1 = strtok(NULL, ",");
			cm1.tmax = atof(s1);
			s1 = strtok(NULL, "");
			cm1.tmin = atof(s1);
		}
	}

	mosquitto_topic_matches_sub("pidrone/CMD/LED", message->topic, &match);
	if (match) {
		msg = (char *)message->payload;
		led.setValue = strdup(msg);
		printf("led command=%s\r\n", msg);
		//~ led.setValue=msg;

		//~ memcpy(&led.setValue[0],&msg[0],1);
		//setBuffer(led.setValue,displayBuffer,length);
		letter = 0;
		y = 0;
	}

}

void mq_init() {
	mosquitto_lib_init();

	mosq = mosquitto_new(NULL, true, NULL);
	mosquitto_username_pw_set(mosq, user, pw);
	mosquitto_message_callback_set(mosq, message_callback);
	if (mosquitto_connect(mosq, "58.224.86.126", 1883, 60)) {
		printf("mqtt connect error\r\n");;
	}
	mosquitto_subscribe(mosq, NULL, "pidrone/CMD/#", 0);
}

void mq_start() {
	int stat = mosquitto_loop_start(mosq);
	while (stat) {
		printf("connection error!\r\n");
		usleep(20000);
		mosquitto_reconnect(mosq);
	}
}

int mq_send(const char *topic, const char *msg) {
	return mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 0, 0);
}

void mq_close() {
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}
#endif
