#include "correlation.h"
#include "storage.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;
using std::isnan;

static const int MIN_POINTS = 5;

void initCorrelationAccumulator(correlationAccumulator& acc) {
	acc.count = 0;
	acc.capacity = 128;
	acc.valuesA = new double[acc.capacity];
	acc.valuesB = new double[acc.capacity];
}

void addCorrelationValue(correlationAccumulator& acc, double a, double b) {
	if (acc.count >= acc.capacity) {
		int tempCap = acc.capacity;

		resizeArrays(acc.valuesA, tempCap);

		tempCap = acc.capacity;
		resizeArrays(acc.valuesB, tempCap);

		acc.capacity = tempCap;
	}
	acc.valuesA[acc.count] = a;
	acc.valuesB[acc.count] = b;
	acc.count++;
}

void freeCorrelationAccumulator(correlationAccumulator& acc) {
	delete[] acc.valuesA;
	delete[] acc.valuesB;
}

double computePearson(const double* a, const double* b, int count) {
	if (count < MIN_POINTS) return 0.0;

	double avgA = computeAverage(a, count);
	double avgB = computeAverage(b, count);

	long double num = 0.0;
	long double denA = 0.0;
	long double denB = 0.0;

	for (int i = 0; i < count; i++) {
		long double da = a[i] - avgA;
		long double db = b[i] - avgB;

		num += da * db;
		denA += da * da;
		denB += db * db;
	}

	if (denA <= 1e-12 || denB <= 1e-12) return 0.0;

	return (double)(num / sqrt(denA * denB));
}

void collectCorrelationData(
	int sensorA, int sensorB,
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours,
	correlationAccumulator& acc
) {
	ifstream bin("data.bin", ios::binary);
	if (!bin) return;

	int startYear, startMonth, startDay, daysCount;
	bin.read((char*)&startYear, sizeof(int));
	bin.read((char*)&startMonth, sizeof(int));
	bin.read((char*)&startDay, sizeof(int));
	bin.read((char*)&daysCount, sizeof(int));

	long startOffset = getDifference(startYear, startMonth, startDay, y1, m1, d1);
	long endOffset = getDifference(startYear, startMonth, startDay, y2, m2, d2);

	long startTime = startOffset * 24;
	long endTime = (endOffset + 1) * 24;

	int totalHours = endTime - startTime + 1;

	double* seriesA = new double[totalHours];
	double* seriesB = new double[totalHours];

	for (int i = 0; i < totalHours; i++) {
		seriesA[i] = NAN;
		seriesB[i] = NAN;
	}

	BinaryReading r;

	while (bin.read((char*)&r, sizeof(r))) {
		if (r.dayIndex < startOffset || r.dayIndex > endOffset)
			continue;

		long timeIndex = r.dayIndex * 24 + r.hour;
		int localIndex = timeIndex - startTime;

		if (localIndex < 0 || localIndex >= totalHours)
			continue;

		if (r.sensorId == sensorA)
			seriesA[localIndex] = r.value;
		else if (r.sensorId == sensorB)
			seriesB[localIndex] = r.value;
	}

	for (int t = 0; t < totalHours; t++) {
		int tb = t + lagHours;
		if (tb >= totalHours) break;

		if (!isnan(seriesA[t]) && !isnan(seriesB[tb])) {
			addCorrelationValue(acc, seriesA[t], seriesB[tb]);
		}
	}

	delete[] seriesA;
	delete[] seriesB;
	bin.close();
}


CorrelationType classifyCorrelation(double r) {
	if (r >= 0.7) return CORR_STRONG_POSITIVE;
	if (r >= 0.4) return CORR_MODERATE_POSITIVE;
	if (r <= -0.7) return CORR_STRONG_NEGATIVE;
	if (r <= -0.4) return CORR_MODERATE_NEGATIVE;
	return CORR_WEAK;
}


void printCorrelationReport(
	int sensorA,
	int sensorB,
	const correlationResult& res
) {
	int totalWidth = 90;
	string title = " CORRELATION ANALYSIS REPORT ";
	int pad = (totalWidth - title.length()) / 2;

	cout << "\n" << string(totalWidth, '=') << "\n";
	cout << string(pad, ' ') << title << "\n";
	cout << string(totalWidth, '=') << "\n\n";

	cout << " SIGNALS\n";
	cout << string(totalWidth, '-') << "\n";
	cout << left << setw(30) << " Sensor A:" << sensors[sensorA].name << "\n";
	cout << left << setw(30) << " Sensor B:" << sensors[sensorB].name << "\n";
	cout << left << setw(30) << " Time Lag:" << res.lagHours << " hours\n\n";

	cout << " STATISTICS\n";
	cout << string(totalWidth, '-') << "\n";
	cout << left << setw(30) << " Data points:" << res.points << "\n";
	cout << left << setw(30) << " Avg A:" << res.avgA << " " << sensors[sensorA].unit << "\n";
	cout << left << setw(30) << " Avg B:" << res.avgB << " " << sensors[sensorB].unit << "\n";
	cout << left << setw(30) << " Std A:" << res.stdA << "\n";
	cout << left << setw(30) << " Std B:" << res.stdB << "\n\n";

	cout << " PEARSON CORRELATION\n";
	cout << string(totalWidth, '-') << "\n";
	cout << left << setw(30) << " r value:" << fixed << setprecision(4) << res.r << "\n";

	CorrelationType type = classifyCorrelation(res.r);
	string interpretation;

	if (type == CORR_STRONG_POSITIVE)
		interpretation = "Strong positive correlation. Likely causal or tightly coupled processes.";
	else if (type == CORR_MODERATE_POSITIVE)
		interpretation = "Moderate positive correlation. Possible indirect influence.";
	else if (type == CORR_STRONG_NEGATIVE)
		interpretation = "Strong negative correlation. One process suppresses the other.";
	else if (type == CORR_MODERATE_NEGATIVE)
		interpretation = "Moderate negative correlation.";
	else
		interpretation = "Weak or no correlation. Independent behavior.";

	cout << left << setw(30) << " Interpretation:" << interpretation << "\n\n";

	cout << " ENGINEERING VERDICT\n";
	cout << string(totalWidth, '-') << "\n";

	if (fabs(res.r) < 0.3)
		cout << " Signals are effectively independent. No coupling detected.\n";
	else
		cout << " Correlation detected. Recommended to analyze node-level causality.\n";

	cout << string(totalWidth, '=') << "\n\n";
}

correlationResult analyzeCorrelation(
	int sensorA,
	int sensorB,
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
) {
	correlationAccumulator acc;
	acc.capacity = 128;
	initCorrelationAccumulator(acc);

	collectCorrelationData(
		sensorA, sensorB,
		y1, m1, d1,
		y2, m2, d2,
		lagHours,
		acc
	);

	correlationResult res{};
	res.points = acc.count;
	res.lagHours = lagHours;

	if (acc.count >= MIN_POINTS) {
		res.avgA = computeAverage(acc.valuesA, acc.count);
		res.avgB = computeAverage(acc.valuesB, acc.count);
		res.stdA = computeStdDev(acc.valuesA, acc.count, res.avgA);
		res.stdB = computeStdDev(acc.valuesB, acc.count, res.avgB);
		res.r = computePearson(acc.valuesA, acc.valuesB, acc.count);
	}
	else {
		res.r = 0.0;
	}

	freeCorrelationAccumulator(acc);
	return res;
}

void analyzeSensorCorrelation(
	int sensorA,
	int sensorB,
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
) {
	correlationResult res = analyzeCorrelation(
		sensorA, sensorB,
		y1, m1, d1,
		y2, m2, d2,
		lagHours
	);

	if (res.points == 0) {
		cout << "\n[!] No data found for correlation analysis.\n";
		return;
	}

	printCorrelationReport(sensorA, sensorB, res);
}

void analyzeAllSensorsCorrelation(
	int y1, int m1, int d1,
	int y2, int m2, int d2,
	int lagHours
) {
	double threshold = 0.3;

	cout << "\n" << string(100, '=') << "\n";
	cout << " CROSS-SENSOR CORRELATION MATRIX (Summary)\n";
	cout << string(100, '=') << "\n";

	cout << left << setw(5) << "ID A"
		<< setw(5) << "ID B"
		<< setw(30) << "Sensor A"
		<< setw(30) << "Sensor B"
		<< setw(10) << "r-value"
		<< "Status" << endl;
	cout << string(100, '-') << "\n";

	int strongPairsCount = 0;

	for (int i = 0; i < sensorCount; i++) {
		for (int j = i + 1; j < sensorCount; j++) {
			correlationResult res = analyzeCorrelation(i, j, y1, m1, d1, y2, m2, d2, lagHours);

			if (res.points < MIN_POINTS) continue;

			cout << left << setw(5) << i
				<< setw(5) << j
				<< setw(30) << sensors[i].name.substr(0, 28)
				<< setw(30) << sensors[j].name.substr(0, 28)
				<< setw(10) << fixed << setprecision(3) << res.r;

			CorrelationType type = classifyCorrelation(res.r);
			string statusText;
			switch (type) {
			case CORR_STRONG_POSITIVE: statusText = "[STRONG +]"; break;
			case CORR_MODERATE_POSITIVE: statusText = "[MODERATE +]"; break;
			case CORR_STRONG_NEGATIVE: statusText = "[STRONG -]"; break;
			case CORR_MODERATE_NEGATIVE: statusText = "[MODERATE -]"; break;
			default: statusText = "Weak"; break;
			}
			cout << statusText << endl;
			if (fabs(res.r) >= threshold) {
				// printCorrelationReport(i, j, res);
				strongPairsCount++;
			}
		}
	}

	cout << string(100, '=') << "\n";
	cout << " Analysis complete. Found " << strongPairsCount << " pairs with |r| >= " << threshold << "\n\n";
}