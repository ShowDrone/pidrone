
#include <control.h>

int slaveA;
int slaveB;
float bldcSpeed;
bool servoMode;
bool autoPilotMode;
byte yawLow;
byte yawHigh;
byte yawDecimal;
byte enTargetPosA;
byte enTargetPosA;


void setI2C (byte _id0, byte _id1) {
    slaveA_id = wiringPiI2CSetup(_id0);
    slaveB_id = wiringPiI2CSetup(_id1);
    int valueA = wiringPiI2CWrite(slaveA_id, 1);
    int valueB = wiringPiI2CWrite(slaveA_id, 1);
    slaveA_state = (valueA) ? OK : 0;
    slaveB_state = (valueA) ? OK : 0;
}

void transmite_SlaveA() {
    wiringPiI2CWrite(slaveA_id, enTargetPosA);
    wiringPiI2CWrite(slaveA_id, sm0.targetPos);
    wiringPiI2CWrite(slaveA_id, sm1.targetPos);
    wiringPiI2CWrite(slaveA_id, servoMode);
}

void transmite_SlaveB() {
    myAzimuth.decimal = (myAzimuth.final - floor(myAzimuth.final))*100;
    myAzimuth.low = myAzimuth.final / 255;
    myAzimuth.high = myAzimuth.final % 255;
    targetAzimuth.decimal = (targetAzimuth.final - floor(targetAzimuth.final))*100;
    targetAzimuth.low = targetAzimuth.final / 255;
    targetAzimuth.high = targetAzimuth.final % 255;
    yawdecimal = (yaw - floor(yaw))*100 ;
    yawLow = yaw / 255;
    yawHigh = yaw % 255;
    wiringPiI2CWrite(slaveB_id, enTargetPosB);
    wiringPiI2CWrite(slaveB_id, bldcSpeed);
    wiringPiI2CWrite(slaveB_id, yawLow);ㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅇㅁㄴㅇㄴㅇㅇ
    wiringPiI2CWrite(slaveB_id, yawHigh);
    wiringPiI2CWrite(slaveB_id, yawdecimal);
    wiringPiI2CWrite(slaveB_id, myAzimuth.low);
    wiringPiI2CWrite(slaveB_id, myAzimuth.high);
    wiringPiI2CWrite(slaveB_id, myAzimuth.decimal);
    wiringPiI2CWrite(slaveB_id, targetAzimuth.low);
    wiringPiI2CWrite(slaveB_id, targetAzimuth.high);
    wiringPiI2CWrite(slaveB_id, targetAzimuth.decimal);
    wiringPiI2CWrite(slaveB_id, autoPilotMode);
}

void request_SlaveA() {
    em0.encoderPos = wiringPiI2CRead(slaveA_id);
    em0.encoderPosDecimal = wiringPiI2CRead(slaveA_id);
    em1.encoderPos = wiringPiI2CRead(slaveA_id);
    em1.encoderPosDecimal = wiringPiI2CRead(slaveA_id);
    servoMode = wiringPiI2CRead(slaveA_id);
    em0.encoderPos += em0.encoderPosDecimal*100;
    em1.encoderPos += em1.encoderPosDecimal*100;
}

void request_SlaveB() {
    em2.encoderPos = wiringPiI2CRead(slaveB_id);
    em3.encoderPos = wiringPiI2CRead(slaveB_id);
}
