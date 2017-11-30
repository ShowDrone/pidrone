#ifndef IMU_H
#define IMU_H

#include <exception>
#include <stdint.h>
#include <iostream>
#include <RTIMULib.h>
class IMU
{
public:
	uint16_t id;
    RTIMUSettings *settings;
    RTIMU *imu;
    RTIMU_DATA imuData;
    float roll,pitch,yaw;    
	IMU(uint16_t _id);
	virtual bool RequestData();
	virtual ~IMU();
    void GetValues((char *)& data);    
     
   
};

#endif
