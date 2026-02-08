#pragma once

#include "stats.h"
#include "types.h"
#include "storage.h"
#include "sensor.h"
#include "date.h"
#include "utilities.h"

struct correlationAccumulator {
	double* valuesA;
	double* valuesB;
	int count;
	int capacity;
};

struct correlationResult {
	double r;
	int points;
	double avgA;
	double avgB;
	double stdA;
	double stdB;
	int lagHours;
};

enum CorrelationType {
	CORR_STRONG_POSITIVE,
	CORR_MODERATE_POSITIVE,
	CORR_WEAK,
	CORR_MODERATE_NEGATIVE,
	CORR_STRONG_NEGATIVE
};

void analyzeSensorCorrelation(
	int sensorA,
	int sensorB,
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
);

void analyzeAllSensorsCorrelation(
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
);

void analyzeSensorCorrelation(
	int sensorA,
	int sensorB,
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
);
