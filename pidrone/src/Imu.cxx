#include "Imu.h"
IMU::IMU(uint16_t _id)
{

    id=_id;
    *settings= RTIMUSettings("RTIMULib");
    *imu = RTIMU::createIMU(settings);
	
	if ((imu==NULL) || (imu->IMUType()==RTIMU_TYPE_NULL))
	{
		printf("No IMU found\n");
		exit(1);
	}
	imu->IMUInit();
	imu->setSlerpPower(0.02);
	imu->setGyroEnable(true);
	imu->setAccelEnable(true);
	imu->setCompassEnable(true);

}

bool IMU::RequestData()
{
    usleep(imu->IMUGetPollInterval()*1000);
    while( imu->IMURead())
    {
        imuData =imu ->getIMUData();
    }
}
void IMU::GetValue((char *)& data)
{
    data=RTMath::displayDegrees("",imuData.fusionPose);
}





IMU::~IMU()
{

}
