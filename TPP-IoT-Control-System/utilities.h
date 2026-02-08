#pragma once
#include <string>
#include "date.h"
#include "sensor.h"
#include<iomanip>

int getSafeInt(std::string message, int minVal = -2147483647, int maxVal = 2147483647);

char getSafeChar(std::string message, std::string validChars);

void getDateRange(int startYear, int startMonth, int startDay, int daysCount, int& y1, int& m1, int& d1, int& y2, int& m2, int& d2);

void printMenuHeader(std::string title);

double computeAverage(const double* values, int count);

double computeStdDev(const double* values, int count, double avg);

void displaySensorList();

std::string centerText(std::string text, int width);


std::string getFactorName(int factor);

void printExitCat();