#pragma once

#include "types.h"
#include <iostream>

bool isLeap(int year);

int daysInMonth(int year, int month);

long dateToAbsDays(int y, int m, int d);

void absDaysToDate(long absDays, int& y, int& m, int& d);

long getDifference(int y1, int m1, int d1, int y2, int m2, int d2);

void nextDay(Date& d);

struct Day {
	int dayIndex;
	DayType type;
	sensorReading* readings;
};

int getWeekday(int y, int m, int d);

int getWeekdayFromAbs(int totalDays);

std::string weekdayToString(int wd);

int getMonthFromAbs(long absDays);

std::string monthToString(int month);