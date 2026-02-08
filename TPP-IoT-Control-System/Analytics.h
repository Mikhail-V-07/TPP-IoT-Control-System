#pragma once

#include "types.h"
#include "storage.h"
#include "sensor.h"
#include "date.h"
#include "stats.h"

void basicStatistics(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);

struct basicStats {
	double min;
	double max;
	double average;
	double median;
	double standardDeviation;
	double timeNormal;
	double timeWarning;
	double timeEmergency;
	int count;
};

typedef bool (*ReadingFilter)(const BinaryReading& r, void* ctx);

struct HourFilterCtx {
	int hour;
};

struct WeekdayFilterCtx {
	int weekday;
	int startAbsDays;
};

struct MonthFilterCtx {
	int startAbsDays;
	int month;
};

struct SeasonFilterCtx {
	int startAbsDays;
	int months[12];
	int count;
};



void analysisByHour(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int hour);

bool filterByHour(const BinaryReading& r, void* ctx);

bool filterByWeekday(const BinaryReading& r, void* ctx);

bool filterByMonth(const BinaryReading& r, void* ctx);

bool filterBySeason(const BinaryReading& r, void* ctx);

void runAnalytics(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, ReadingFilter filter, void* filterCtx, statsAccumulator& s);

void basicStatistics(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2);

void analysisByHour(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int hour);

void analysisByWeekday(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int weekday);

void analysisByMonth(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int month);

void analysisBySeason(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int seasonType);