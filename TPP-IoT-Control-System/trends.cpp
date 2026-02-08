#include "trends.h"
#include "utilities.h"
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

static const int MIN_POINTS = 5;

static const double MIN_TREND_DELTA = 1.0;

static const double FLUCTUATION_THRESHOLD = 2.5;

static const double MIN_SLOPE = 0.15;

void initTrendAccumulator(trendAccumulator& t) {
	t.count = 0;
	t.capacity = 128;
	t.values = new double[t.capacity];
}

void addTrendValue(trendAccumulator& t, double value) {
	if (t.count >= t.capacity) {
		resizeArrays(t.values, t.capacity);
	}
	t.values[t.count++] = value;
}

void freeTrendAccumulator(trendAccumulator& t) {
	delete[] t.values;
}



double computeSlope(const double* values, int count) {
	return (values[count - 1] - values[0]) / (double)(count-1);
}

double computeRange(const double* values, int count) {
	double minV = values[0];
	double maxV = values[0];

	for (int i = 0; i < count; i++) {
		if (values[i] > maxV) maxV = values[i];
		if (values[i] < minV) minV = values[i];
	}
	return maxV - minV;
}

bool isMostMonotonic(const double* values, int count) {
	int ups = 0;
	int downs = 0;

	for (int i = 0; i < count -1; i++) {
		if (values[i] > values[i + 1]) downs++;
		if (values[i] < values[i + 1]) ups++;
	}

	int dominant = max(ups, downs);
	return dominant >= (int)(0.7 * (count - 1));
}

void printFinalTrendReport(int sensorId, const double* values, int count) {
	if (count < MIN_POINTS) {
		cout << "\n[!] INSUFFICIENT DATA FOR TREND ANALYSIS\n";
		return;
	}

	const auto& s = sensors[sensorId];

	trendResult simple = detectTrend(values, count, s.normalMin, s.normalMax);
	trendResult math = computeLinearRegression(values, count, s.normalMin, s.normalMax);

	string title = " FINAL TREND REPORT :: " + s.name + " ";
	int W = 90;

	cout << "\n" << string(W, '=') << "\n";
	cout << centerText(title, W) << "\n";
	cout << string(W, '=') << "\n\n";

	cout << " ANALYTICS DATA\n" << string(W, '-') << "\n";
	cout << left << setw(28) << " Average Value:" << math.avgValue << " " << s.unit << "\n";
	cout << left << setw(28) << " Regression Slope:" << math.slope << " per step\n";
	cout << left << setw(28) << " Monotonicity:" << (isMostMonotonic(values, count) ? "Yes" : "No/Noisy") << "\n\n";

	cout << " SYSTEM VERDICT\n" << string(W, '-') << "\n";

	string tname;
	switch (math.type) {
	case TREND_UP: tname = "UPWARD (Rising)"; break;
	case TREND_DOWN: tname = "DOWNWARD (Falling)"; break;
	case TREND_OSCILLATING_NORMAL: tname = "OSCILLATING (Normal)"; break;
	case TREND_OSCILLATING_DANGEROUS: tname = "OSCILLATING (Critical!)"; break;
	default: tname = "STABLE";
	}

	cout << " Result: " << tname << "\n";

	if (simple.type != math.type && math.type != TREND_STABLE) {
		cout << " [Note] Mathematical and heuristic models differ. Signal may be noisy.\n";
	}

	cout << string(W, '=') << "\n\n";
}




trendResult detectTrend(const double* values, int count, double normalMin, double normalMax) {
	trendResult result{ TREND_STABLE, 0.0, 0.0 };

	if (count < MIN_POINTS) {
		return result;
	}

	double avg = computeAverage(values, count);
	double slope = computeSlope(values, count);
	double range = computeRange(values, count);

	result.avgValue = avg;
	result.slope = slope;

	double safeOscRange = 0.25 * (normalMax - normalMin);

	if (fabs(slope) < MIN_SLOPE && range >= FLUCTUATION_THRESHOLD) {
		if (range <= safeOscRange) {
			result.type = TREND_OSCILLATING_NORMAL;
		}
		else {
			result.type = TREND_OSCILLATING_DANGEROUS;
		}
		return result;
	}

	if (!isMostMonotonic(values, count)) {
		result.type = TREND_STABLE;
		return result;
	}


	double totalDelta = values[count - 1] - values[0];

	if (abs(totalDelta) < MIN_TREND_DELTA) {
		result.type = TREND_STABLE;
		return result;
	}

	if (slope > 0) {
		result.type = TREND_UP;
	}
	else {
		result.type = TREND_DOWN;
	}

	return result;
}

trendResult computeLinearRegression(const double* values, int count, double normalMin, double normalMax) {
	trendResult result{ TREND_STABLE, 0.0, 0.0 };

	if (count < 2) return result;

	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0, sumY2 = 0;
	for (int i = 0; i < count; i++) {
		sumX += i;
		sumY += values[i];
		sumXY += (double)i * values[i];
		sumX2 += (double)i * i;
		sumY2 += values[i] * values[i];
	}

	double n = (double)count;
	double denominator = (n * sumX2 - sumX * sumX);
	if (abs(denominator) < 1e-9) return result;

	double slope = (n * sumXY - sumX * sumY) / denominator;
	result.slope = slope;
	result.avgValue = sumY / n;

	double range = computeRange(values, count);
	double sensorSpan = normalMax - normalMin;
	if (sensorSpan <= 0) sensorSpan = 1.0;

	double significanceThreshold = sensorSpan * 0.02;

	double dangerOscThreshold = sensorSpan * 0.15;

	double rNum = (n * sumXY - sumX * sumY);
	double rDen = sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
	double r = (abs(rDen) < 1e-9) ? 0 : rNum / rDen;

	if (fabs(slope) < 0.05) {
		if (range > dangerOscThreshold) {
			result.type = TREND_OSCILLATING_DANGEROUS;
		}
		else if (range > significanceThreshold) {
			result.type = TREND_OSCILLATING_NORMAL;
		}
		else {
			result.type = TREND_STABLE;
		}
		return result;
	}

	if (fabs(r) > 0.3) {
		if (slope > 0) result.type = TREND_UP;
		else result.type = TREND_DOWN;
	}
	else {
		result.type = TREND_STABLE;
	}

	result.range = range;

	return result;
}

void analyzeSensorTrend(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {

	ifstream bin("data.bin", ios::binary);
	if (!bin) return;

	int startYear, startMonth, startDay, daysCount;

	bin.read((char*)&startYear, sizeof(int));
	bin.read((char*)&startMonth, sizeof(int));
	bin.read((char*)&startDay, sizeof(int));
	bin.read((char*)&daysCount, sizeof(int));

	long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
	long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

	trendAccumulator acc;
	initTrendAccumulator(acc);

	BinaryReading r;

	while (bin.read((char*)&r, sizeof(r))) {
		if (r.sensorId != sensorId || r.dayIndex < startOffSet) continue;
		if (r.dayIndex > endOffSet) break;

		addTrendValue(acc, r.value);
	}

	if (acc.count > 0) {
		printFinalTrendReport(sensorId, acc.values, acc.count);
	}
	else {
		cout << "\n[!] No data found for the selected period.\n";
	}

	freeTrendAccumulator(acc);
	bin.close();
}

trendResult getTrendData(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {

	ifstream bin("data.bin", ios::binary);
	trendResult res{ TREND_STABLE, 0.0, 0.0, 0.0 };
	if (!bin) return res;

	int startYear, startMonth, startDay, daysCount;

	bin.read((char*)&startYear, sizeof(int));
	bin.read((char*)&startMonth, sizeof(int));
	bin.read((char*)&startDay, sizeof(int));
	bin.read((char*)&daysCount, sizeof(int));

	long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
	long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

	trendAccumulator acc;
	initTrendAccumulator(acc);

	BinaryReading r;
	double lastV = 0;

	while (bin.read((char*)&r, sizeof(r))) {
		if (r.sensorId != sensorId || r.dayIndex < startOffSet) continue;
		if (r.dayIndex > endOffSet) break;

		addTrendValue(acc, r.value);
		lastV = r.value;
	}

		if (acc.count >= MIN_POINTS) {
			res = computeLinearRegression(acc.values, acc.count, sensors[sensorId].normalMin, sensors[sensorId].normalMax);
			res.lastValue = lastV;
		}

		freeTrendAccumulator(acc);
		bin.close();
		return res;
}