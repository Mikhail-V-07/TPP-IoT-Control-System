#pragma once
#include "sensor.h"
#include "utilities.h"
#include "stats.h"
#include "storage.h"

enum TrendType {
	TREND_UP,
	TREND_DOWN,
	TREND_STABLE,
	TREND_OSCILLATING_NORMAL,
	TREND_OSCILLATING_DANGEROUS
};

struct trendResult {
	TrendType type;
	double slope;
	double avgValue;
	double range;
	int daysPeriod;
	double stdDev;
	double lastValue;
};

struct trendAccumulator {
	double* values;
	int count;
	int capacity;
};

void initTrendAccumulator(trendAccumulator& t);

void addTrendValue(trendAccumulator& t, double value);

void freeTrendAccumulator(trendAccumulator& t);

double computeSlope(const double* values, int count);

double computeRange(const double* values, int count);

bool isMostMonotonic(const double* values, int count);

void printFinalTrendReport(int sensorId, const double* values, int count);

trendResult detectTrend(const double* values, int count, double normalMin, double normalMax);

trendResult computeLinearRegression(const double* values, int count, double normalMin, double normalMax);

void analyzeSensorTrend(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);

trendResult getTrendData(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);
