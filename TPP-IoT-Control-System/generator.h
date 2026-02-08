#pragma once
#include "types.h"
#include "storage.h"
#include "sensor.h"
#include "date.h"

void generateHistory(int daysCount, int startYear, int startMonth, int startDay);

double getRandom(double min, double max);

std::string typeToString(DayType t);




