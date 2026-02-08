#pragma once

enum DayType {
	NORMAL,
	WARNING,
	EMERGENCY
};

enum FactorType {
	boilerLoad,
	turbineLoad,
	electricalLoad,
	coolingEfficiency,
	combustionQuality,
	fuelLevel,
	factorCount
};

struct sensorReading {
	double value;
	int status;
};

struct Date {
	int year;
	int month;
	int day;
};