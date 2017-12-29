#ifndef SONAR_H
#define SONAR_H

#include <exception>
#include <stdint.h>
#include <iostream>
#include <wiringPi.h>
#include <wiringPiI2C.h>


class SONAR
{
public:
	uint16_t id;
	int distance;
	SONAR(uint16_t _id);
	virtual bool RequestData();
	virtual ~SONAR();
	uint16_t GetValues();
	void SetID(uint16_t newid);
};

#endif
