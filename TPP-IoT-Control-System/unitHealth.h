#pragma once

#include "healthTypes.h"
#include <string>

struct unitNode {
	int id;
	std::string name;
	int* sensorIds;
	int sensorCount;
};

UnitHealthReport collectUnitHealth(const unitNode& unit, int y1, int m1, int d1, int y2, int m2, int d2);

void initUnitRegistry();

void freeUnitRegistry();

UnitHealthReport collectUnitHealth(const unitNode& unit, int y1, int m1, int d1, int y2, int m2, int d2);

static const char* healthStateStr(HealthState s);

void printUnitHealthReport(
	const unitNode& unit,
	const UnitHealthReport& report,
	int y1, int m1, int d1,
	int y2, int m2, int d2
);

void unitHealth(int unitId, int y1, int m1, int d1, int y2, int m2, int d2);
