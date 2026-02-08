#pragma once
#include "types.h"
#include "storage.h"
#include "sensor.h"
#include "date.h"
#include "stats.h"
#include "utilities.h"

enum AnomalyType {THRESHOLD, SPIKE, UNSTABLE};

struct AnomalyEvent {
	int startDay;
	int startHour;
	int duration;
	AnomalyType type;
	double peakValue;
	double info;
};

struct AnomalyReport {
	AnomalyEvent* events;
	int count;
	int capacity;
};


void initReport(AnomalyReport& rep);

void addAnomaly(AnomalyReport& rep, const BinaryReading& r, int day, int hr, int duration, AnomalyType type, double val, double info);

void printAnomalyReport(const AnomalyReport& rep, int sensorId);

void freeReport(AnomalyReport& rep);

void detectAnomalies(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);

bool hasSensorAnomalies(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);

double getAnomalyRatio(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);
