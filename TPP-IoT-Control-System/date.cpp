#include "date.h"
#include <cmath>
using namespace std;

bool isLeap(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int year, int month) {

	if (month < 1 || month > 12) {
		return 0;
	}

	static int mdays[] = { 31,28,31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (month == 2 && isLeap(year)) {
		return 29;
	}
	return mdays[month - 1];
}

long dateToAbsDays(int y, int m, int d) {
	long totalDays = d;

	for (int i = 1; i < y; i++) {
		if (isLeap(i)) {
			totalDays += 366;
		}
		else {
			totalDays += 365;
		}
	}

	for (int i = 1; i < m; i++) {
		totalDays += daysInMonth(y, i);
	}
	return totalDays;
}

void absDaysToDate(long absDays, int& y, int& m, int& d) {
	y = 1;
	while (true) {
		int daysInYear;

		if (isLeap(y)) {
			daysInYear = 366;
		}
		else {
			daysInYear = 365;
		}
		if (absDays > daysInYear) {
			absDays -= daysInYear;
			y++;
		}
		else {
			break;
		}
	}

	m = 1;
	while (true) {
		int dim = daysInMonth(y, m);

		if (absDays > dim) {
			absDays -= dim;
			m++;
		}
		else {
			break;
		}
	}

	d = int(absDays);
}

long getDifference(int y1, int m1, int d1, int y2, int m2, int d2) {
	long dayCount1 = dateToAbsDays(y1, m1, d1);
	long dayCount2 = dateToAbsDays(y2, m2, d2);

	return abs(dayCount1 - dayCount2);
}

void nextDay(Date& d) {
	d.day++;
	if (d.day > daysInMonth(d.year, d.month)) {
		d.day = 1;
		d.month++;
		if (d.month > 12) {
			d.month = 1;
			d.year++;
		}
	}
}

int getWeekday(int y, int m, int d) {
	long totalDays = dateToAbsDays(y, m, d);

	return (totalDays -1) % 7;
}

int getWeekdayFromAbs(int totalDays) {
	return (totalDays - 1) % 7;
}

std::string weekdayToString(int wd) {
	static const std::string names[] = {
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
	};
	if (wd >= 0 && wd < 7) return names[wd];
	return "Unknown";
}

std::string monthToString(int month) {
	static const std::string names[] = {
		"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
	};
	if (month >= 1 && month < 13) return names[month - 1];
	return "Unknown";
}

int getMonthFromAbs(long absDays) {
	int y, m, d;
	absDaysToDate(absDays, y, m, d);

	return m;
}