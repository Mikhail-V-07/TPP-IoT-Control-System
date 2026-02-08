#include "stats.h"
#include "storage.h"
#include <iostream>
#include <iomanip>

using namespace std;



void bubbleSort(double values[], int count) {
	for (int pass = 0; pass < count; pass++) {
		for (int i = 0; i < count - 1 - pass; i++) {
			if (values[i] > values[i + 1]) {
				double temp = values[i];
				values[i] = values[i + 1];
				values[i + 1] = temp;
			}
		}
	}
}

void initStats(statsAccumulator& s) {
	s.sum = 0;
	s.dispersion = 0;
	s.count = 0;

	s.min = 1e9;
	s.max = -1e9;

	s.timeNormal = 0;
	s.timeEmergency = 0;
	s.timeWarning = 0;

	s.capacity = 128;
	s.valuesCount = 0;
	s.values = new double[s.capacity];

	s.median = 0;
}

void addReading(statsAccumulator& s, const BinaryReading& r) {
	if (r.value > s.max) s.max = r.value;
	if (r.value < s.min) s.min = r.value;

	if (r.status == 0) s.timeNormal++;
	if (r.status == 2) s.timeEmergency++;
	if (r.status == 1) s.timeWarning++;

	s.sum += r.value;
	s.count++;

	if (s.valuesCount >= s.capacity) {
		resizeArrays(s.values, s.capacity);
	}

	s.values[s.valuesCount++] = r.value;
}

void finalizeStats(statsAccumulator& s) {
	double avg = s.sum / s.count;
	for (int i = 0; i < s.valuesCount; i++) {
		s.dispersion += (s.values[i] - avg) * (s.values[i] - avg);
	}
	bubbleSort(s.values, s.valuesCount);

	if (s.valuesCount % 2 == 1) {
		s.median = s.values[s.valuesCount / 2];
	}
	else {
		s.median = (s.values[s.valuesCount / 2 - 1] + s.values[s.valuesCount / 2]) / 2.0;
	}
}

void printStats(statsAccumulator& s, int sensorId) {
	if (s.count == 0) {
		cout << "\n[!] NO DATA AVAILABLE FOR THE SELECTED PERIOD\n";
		return;
	}

	double avg = s.sum / s.count;
	double stddev = sqrt(s.dispersion / s.count);
	string unit = sensors[sensorId].unit;
	string sensorName = sensors[sensorId].name;

	cout << "\n" << string(50, '=') << "\n";
	cout << "  ANALYTICS REPORT: " << sensorName << "\n";
	cout << string(50, '=') << "\n";

	cout << left << setw(25) << "  Total Readings:" << right << setw(20) << s.count << "\n";
	cout << left << setw(25) << "  Measurement Unit:" << right << setw(20) << unit << "\n";
	cout << string(50, '-') << "\n";

	cout << fixed << setprecision(2);
	cout << "  " << setw(10) << "MIN" << setw(12) << "MAX" << setw(12) << "AVG" << setw(12) << "MEDIAN" << "\n";
	cout << "  " << string(46, '.') << "\n";
	cout << "  " << setw(10) << s.min << setw(12) << s.max << setw(12) << avg << setw(12) << s.median << "\n\n";

	cout << left << setw(25) << "  Std. Deviation:" << right << setw(20) << stddev << "\n";
	cout << string(50, '-') << "\n";

	cout << "  STATUS DISTRIBUTION:\n\n";

	auto printRow = [&](string label, int val, char symbol) {
		double pct = (double)val / s.count * 100.0;
		cout << "  " << left << setw(12) << label
			<< " [" << string(pct / 5, symbol) << string(20 - pct / 5, ' ') << "] "
			<< right << setw(6) << pct << "%\n";
		};

	printRow("Normal", s.timeNormal, '=');
	printRow("Warning", s.timeWarning, '/');
	printRow("Emergency", s.timeEmergency, '!');

	cout << string(50, '=') << "\n\n";
}


void freeStats(statsAccumulator& s) {
	delete[]s.values;
}



