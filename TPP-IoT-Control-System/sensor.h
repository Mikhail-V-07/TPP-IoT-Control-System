#pragma once
#include "types.h"
#include <string>

struct sensorTemplate {
	std::string name;
	std::string unit;
	double normalMin;
	double normalMax;
	double alarmMin;
	double alarmMax;
	FactorType relatedFactor;
};

extern int sensorCount;
extern sensorTemplate* sensors;

void initSensors();




void nextDay(Date& d);


//Analytics

