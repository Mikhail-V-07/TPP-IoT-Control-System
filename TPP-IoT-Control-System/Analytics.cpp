#include "Analytics.h"
#include "stats.h"
#include <fstream>
#include <iostream>

using namespace std;

bool filterByHour(const BinaryReading& r, void* ctx) {
	HourFilterCtx* hourCtx = (HourFilterCtx*)ctx;

	int recordHour = r.hour;
	int targetHour = hourCtx->hour;

	if (recordHour == targetHour) {
		return true;
	}
	else {
		return false;
	} 
}

bool filterByWeekday(const BinaryReading& r, void* ctx) {
	WeekdayFilterCtx* c = (WeekdayFilterCtx*)ctx;
	int absDays = c->startAbsDays + r.dayIndex;
	int targetWeekday = c->weekday;
	int currentWeekday = getWeekdayFromAbs(absDays);
	if (currentWeekday == targetWeekday) {
		return true;
	}
	else {
		return false;
	}
}

bool filterByMonth(const BinaryReading& r, void* ctx) {
	MonthFilterCtx* c = (MonthFilterCtx*)ctx;
	int absDays = c->startAbsDays + r.dayIndex;
	int targetMonth = c->month;
	int currentMonth = getMonthFromAbs(absDays);
	if (currentMonth == targetMonth) {
		return true;
	}
	else {
		return false;
	}
}

bool filterBySeason(const BinaryReading& r, void* ctx) {
	SeasonFilterCtx* c = (SeasonFilterCtx*)ctx;
	int absDays = c->startAbsDays + r.dayIndex;
	int currentMonth = getMonthFromAbs(absDays);
	for (int i = 0; i < c->count; i++) {
		if (currentMonth == c->months[i]) {
			return true;
		}
	}
	
	return false;
}

void runAnalytics(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, ReadingFilter filter, void* filterCtx, statsAccumulator& s) {
	ifstream bin("data.bin", ios::binary);

	int startYear, startMonth, startDay, daysCount;

	bin.read((char*)&startYear, sizeof(int));
	bin.read((char*)&startMonth, sizeof(int));
	bin.read((char*)&startDay, sizeof(int));
	bin.read((char*)&daysCount, sizeof(int));

	long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
	long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

	BinaryReading r;

	while (bin.read((char*)&r, sizeof(r))) {
		if (r.sensorId != sensorId) continue;
		if (r.dayIndex < startOffSet) continue;
		if (r.dayIndex > endOffSet) break;

		if (filter && !filter(r, filterCtx)) continue;

		addReading(s, r);
	}


	finalizeStats(s);
}


void basicStatistics(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {
	statsAccumulator s;
	initStats(s);

	runAnalytics(sensorId, y1, m1, d1, y2, m2, d2, nullptr, nullptr, s);

	printStats(s, sensorId);
	freeStats(s);
}

void analysisByHour(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int hour) {
	statsAccumulator s;
	initStats(s);

	HourFilterCtx ctx;
	ctx.hour = hour;

	runAnalytics(sensorId, y1, m1, d1, y2, m2, d2, filterByHour, &ctx, s);

	cout << "\nHour: " << hour << "\n";
	printStats(s, sensorId);
	freeStats(s);
}

void analysisByWeekday(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int weekday) {
	statsAccumulator s;
	initStats(s);

	WeekdayFilterCtx ctx;
	ctx.weekday = weekday;

	ifstream bin("data.bin", ios::binary);
	int sy, sm, sd;
	bin.read((char*)&sy, sizeof(int));
	bin.read((char*)&sm, sizeof(int));
	bin.read((char*)&sd, sizeof(int));
	bin.close();

	ctx.startAbsDays = dateToAbsDays(sy, sm, sd);

	runAnalytics(sensorId, y1, m1, d1, y2, m2, d2, filterByWeekday, &ctx, s);

	cout << "\nWeekday: " << weekdayToString(weekday) << "\n";

	printStats(s, sensorId);
	freeStats(s);
}

void analysisByMonth(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int month) {
	statsAccumulator s;
	initStats(s);

	MonthFilterCtx ctx;
	ctx.month = month;

	ifstream bin("data.bin", ios::binary);
	int sy, sm, sd;
	bin.read((char*)&sy, sizeof(int));
	bin.read((char*)&sm, sizeof(int));
	bin.read((char*)&sd, sizeof(int));
	bin.close();

	ctx.startAbsDays = dateToAbsDays(sy, sm, sd);

	runAnalytics(sensorId, y1, m1, d1, y2, m2, d2, filterByMonth, &ctx, s);

	cout << "\nMonth: " << monthToString(month) << "\n";

	printStats(s, sensorId);
	freeStats(s);
}

void analysisBySeason(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2, int seasonType) {
	statsAccumulator s;
	initStats(s);

	SeasonFilterCtx ctx;

	ifstream bin("data.bin", ios::binary);
	int sy, sm, sd;
	bin.read((char*)&sy, sizeof(int));
	bin.read((char*)&sm, sizeof(int));
	bin.read((char*)&sd, sizeof(int));
	bin.close();

	ctx.startAbsDays = dateToAbsDays(sy, sm, sd);
	if (seasonType == 0) {
		ctx.months[0] = 12; ctx.months[1] = 1; ctx.months[2] = 2;
		ctx.count = 3;
		cout << "\nSeason: Winter\n";
	}
	else if (seasonType == 1) {
		ctx.months[0] = 3; ctx.months[1] = 4; ctx.months[2] = 5;
		ctx.count = 3;
		cout << "\nSeason: Spring\n";
	}
	else if (seasonType == 2) {
		ctx.months[0] = 6; ctx.months[1] = 7; ctx.months[2] = 8;
		ctx.count = 3;
		cout << "\nSeason: Summer\n";
	}
	else {
		ctx.months[0] = 9; ctx.months[1] = 10; ctx.months[2] = 11;
		ctx.count = 3;
		cout << "\nSeason: Autumn\n";
	}

	runAnalytics(sensorId, y1, m1, d1, y2, m2, d2, filterBySeason, &ctx, s);

	printStats(s, sensorId);
	freeStats(s);
}