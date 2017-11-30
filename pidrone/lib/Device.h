#ifndef DEVICE_H
#define DEVICE_H


#include <exception>
#include <iostream>
#include <stdint.h>
#define HANDLE void *
#define DWORD unsigned long
#define WORD unsigned short
#define BYTE unsigned char





class Device
{

public: 
	uint16_t id;
	HANDLE handle;
	//Device(uint16_t _id);
	void SetHandle(HANDLE _handle);
	virtual bool RequestData()=0;
	virtual ~Device();

};




class IMU : public Device
{
	
public:
	IMU(DWORD _id);
	virtual bool RequestData();
	virtual ~IMU();
	void GetValues(const BYTE* data1, const BYTE* data2, float& roll, float& pitch, float& yaw);
};



class DeviceException: public std::exception
{
public:
	virtual const char* what() const throw();
};


#endif 
