#include "generator.h"
#include <fstream>
#include <iomanip>
#include <random>
#include <cmath>

using namespace std;

static mt19937 gen(random_device{}());

double getRandom(double min, double max) {
	uniform_real_distribution<double> dist(min, max);
	return dist(gen);
}

string typeToString(DayType t) {
	if (t == EMERGENCY) return "EMERGENCY";
	if (t == WARNING) return "WARNING";
	return "NORMAL";
}

static double factorCoupling[factorCount][factorCount] = {
	// T     P     V     O     C     ...
	{ 0.0,  0.35,  0.10,  0.05,  -0.40 }, // Temperature
	{ 0.20,  0.0,  0.30,  0.10,  -0.15}, // Pressure
	{ 0.10,  0.15,  0.0,  0.25,  0.0 }, // Vibration
	{ 0.05,  0.10,  0.30,  0.0,  -0.10 }, // Oil
	{ -0.45,  -0.20,  0.0,  -0.10,  0.0 }, // Cooling
};



void generateHistory(int daysCount, int startYear, int startMonth, int startDay) {

	Date currentDate;

	currentDate.year = startYear;
	currentDate.month = startMonth;
	currentDate.day = startDay;

	ofstream csv("data.csv");
	ofstream bin("data.bin", ios::binary);

	bin.write((char*)&startYear, sizeof(startYear));
	bin.write((char*)&startMonth, sizeof(startMonth));
	bin.write((char*)&startDay, sizeof(startDay));
	bin.write((char*)&daysCount, sizeof(daysCount));

	csv << "Day;Hour;Type;SensorID;SensorName;Value;Status\n";

	double factorState[factorCount] = { 0.0 };
	double factorVelocity[factorCount] = { 0.0 };

	for (int d = 0; d < daysCount; d++) {
		DayType todayType = NORMAL;
		double roll = getRandom(0, 100);


		if (roll > 95) {
			todayType = EMERGENCY;
		}
		else if (roll > 85) {
			todayType = WARNING;
		}



		int activeFactors = 0;
		if (todayType == WARNING) activeFactors = 2;
		if (todayType == EMERGENCY) activeFactors = 3;

		bool used[factorCount] = { false };
		for (int i = 0; i < activeFactors; i++) {
			int f;
			do {
				f = (int)getRandom(0, factorCount - 0.01);
			} while (used[f]);
			used[f] = true;
		}

		double phase1 = getRandom(0, 2 * 3.1415926);
		double phase2 = getRandom(0, 2 * 3.1415926);

		for (int f = 0; f < factorCount; f++) {
			factorState[f] = getRandom(-0.2, 0.2);
			factorVelocity[f] = 0.0;
		}

		int triggerFactor = -1;

		if (todayType != NORMAL) {
			triggerFactor = (int)getRandom(0, factorCount - 0.01);
		}

		for (int h = 0; h < 24; h++) {

			phase1 += getRandom(0.02, 0.05); // main daily
			phase2 += getRandom(0.005, 0.02); // slow technological

			double dailyWave = 0.7 * sin(phase1) + 0.3 * sin(phase2);

			for (int f = 0; f < factorCount; f++) {

				double inertia = 0.85;
				double noice = getRandom(-0.02, 0.02);

				double couplingInfluence = 0.0;

				for (int k = 0; k < factorCount; k++) {
					couplingInfluence += factorCoupling[k][f] * factorState[k];
				}

				double waveDrive = dailyWave * 0.05;

				double impulse = 0.0;
				if (getRandom(0, 100) > 99.5) {
					impulse = getRandom(-0.4, 0.4);
				}

				double dayImpact = 0.0;
				if (f == triggerFactor) {
					if (todayType == WARNING)   dayImpact = 0.08;
					if (todayType == EMERGENCY) dayImpact = 0.25;
				}

				factorVelocity[f] = inertia * factorVelocity[f] + couplingInfluence + waveDrive + impulse + dayImpact + noice;

				factorState[f] += factorVelocity[f];

				if (factorState[f] > 1.5)  factorState[f] = 1.5;
				if (factorState[f] < -1.5) factorState[f] = -1.5;
			}

			for (int i = 0; i < sensorCount; i++) {
				sensorTemplate& s = sensors[i];

				double base = getRandom(s.normalMin, s.normalMax);

				double influence = factorState[s.relatedFactor] * (s.normalMax - s.normalMin) * 0.08;



				double noise = getRandom(-0.03, 0.03) * (s.normalMax - s.normalMin);

				double value = base + influence + noise;

				if (value < 0) value = 0;
				int status = 0;

				if (value > s.alarmMax || value < s.alarmMin) {
					status = 2;
				}
				else if (value > s.normalMax || value < s.normalMin) {
					status = 1;
				}

				csv << currentDate.year << "-"
					<< setfill('0') << setw(2) << currentDate.month << "-"
					<< setw(2) << currentDate.day << ";"
					<< setfill(' ') << setw(2) << h << ":00;"
					<< typeToString(todayType) << ";"
					<< i << ";"
					<< s.name << ";"
					<< fixed << setprecision(2) << value << ";"
					<< status << "\n";


				BinaryReading record;
				record.value = (float)value;
				record.sensorId = i;
				record.status = (char)status;
				record.dayIndex = d;
				record.hour = h;

				bin.write((char*)&record, sizeof(record));
			}
		}
		nextDay(currentDate);
	}
	csv.close();
	bin.close();
}