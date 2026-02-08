#include "anomalies.h"
#include "storage.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

void initReport(AnomalyReport& rep) {
	rep.count = 0;
    rep.capacity = 16;
    rep.events = new AnomalyEvent[rep.capacity];
}

void addAnomaly(AnomalyReport& rep, const BinaryReading& r, int day, int hr, int duration, AnomalyType type, double val, double info) {
	if (rep.count >= rep.capacity) {
		resizeArrays(rep.events, rep.capacity);
	}
    rep.events[rep.count++] = { day, hr, duration, type, val, info };
}


void printAnomalyReport(const AnomalyReport& rep, int sensorId) {

    string title = " FINAL ANOMALY REPORT: " + sensors[sensorId].name + " ";
    int totalWidth = 85;
    int sidePadding = (totalWidth - (int)title.length()) / 2;

    cout << "\n" << string(totalWidth, '=') << "\n";
    cout << string(sidePadding, ' ') << title << "\n";
    cout << string(totalWidth, '=') << "\n";

    if (rep.count == 0) {
        cout << "\n\t   >>> [CLEAN RECORD: No anomalies detected] <<<\n\n";
        cout << string(totalWidth, '=') << endl;
        return;
    }

    cout << left << setw(8) << " Day"
        << setw(8) << "Start"
        << setw(18) << "Type"
        << setw(15) << "Peak Value"
        << setw(12) << "Duration"
        << "Detailed Information" << endl;
    cout << string(totalWidth, '-') << endl;

    cout << fixed << setprecision(2);

    for (int i = 0; i < rep.count; i++) {
        const auto& ev = rep.events[i];

        string typeStr;
        if (ev.type == THRESHOLD) typeStr = "CRITICAL LIMIT";
        else if (ev.type == SPIKE) typeStr = "SUDDEN SPIKE";
        else typeStr = "UNSTABLE VAR";

        cout << " " << left << setw(7) << ev.startDay
            << setw(8) << (to_string(ev.startHour) + ":00")
            << setw(18) << typeStr
            << setw(15) << ev.peakValue;

        if (ev.type == THRESHOLD) {
            cout << setw(12) << (to_string(ev.duration) + "h")
                << "Value stayed out of safe bounds";
        }
        else if (ev.type == SPIKE) {
            cout << setw(12) << "Instant"
                << "Jump of " << ev.info << "% in 1 hour";
        }
        else if (ev.type == UNSTABLE) {
            cout << setw(12) << (to_string(ev.duration) + "h")
                << "Max fluctuation range: " << ev.info;
        }

        cout << endl;
    }

    cout << string(totalWidth, '=') << endl;
    cout << " TOTAL EVENTS DETECTED: " << rep.count << " | Unit: " << sensors[sensorId].unit << "\n\n";
}

void freeReport(AnomalyReport& rep) {
    delete[]rep.events;
    rep.events = nullptr;
}

void detectAnomalies(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {
	ifstream bin("data.bin", ios::binary);
    if (!bin) return;

    int startYear, startMonth, startDay, daysCount;

    bin.read((char*)&startYear, sizeof(int));
    bin.read((char*)&startMonth, sizeof(int));
    bin.read((char*)&startDay, sizeof(int));
    bin.read((char*)&daysCount, sizeof(int));

    long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
    long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

	AnomalyReport report;
    initReport(report);

    BinaryReading r;

    bool activeThresh = false;    int durThresh = 0, startHrThresh = 0, startDayThresh = 0;
    bool activeUnstable = false;  int durUnstable = 0, startHrUnstable = 0, startDayUnstable = 0;

    double peakValThresh = 0;
    double maxRangeUnstable = 0;

    double sensorRange = sensors[sensorId].normalMax - sensors[sensorId].normalMin;
    double window[12] = { 0 };
    int wCount = 0;


    double lastValue = -1e9;
    bool hasLast = false;


    while (bin.read((char*)&r, sizeof(r))) {
        if (r.sensorId != sensorId || r.dayIndex < startOffSet) continue;
        if (r.dayIndex > endOffSet) break;

        double val = r.value;

        bool outOfBonds = (val > sensors[sensorId].alarmMax || val < sensors[sensorId].alarmMin);
        if (outOfBonds) {
            if (!activeThresh) {
                activeThresh = true;
                startHrThresh = r.hour;
                startDayThresh = r.dayIndex;
                durThresh = 1;
                peakValThresh = val;
            }
            else {
                durThresh++;
                if (abs(val) > abs(peakValThresh)) peakValThresh = val;
            }
        }
        else if (activeThresh && durThresh >= 3) {
            if (durThresh >= 2) {
                addAnomaly(report, r, startDayThresh, startHrThresh, durThresh, THRESHOLD, peakValThresh, (double)durThresh);
            }
            activeThresh = false;
        }


        window[wCount % 12] = val;
        wCount++;

        if (wCount >= 12) {
            double wMin = window[0], wMax = window[0];
            for (int i = 1; i < 12; i++) {
                if (window[i] < wMin) wMin = window[i];
                if (window[i] > wMax) wMax = window[i];
            }

            double currentRange = wMax - wMin;
            if (currentRange > sensorRange * 0.60) {
                if (!activeUnstable) {
                    activeUnstable = true;
                    startHrUnstable = r.hour;
                    startDayUnstable = r.dayIndex;
                    durUnstable = 12;
                    maxRangeUnstable = currentRange;
                }
                else {
                    durUnstable++;
                    if (currentRange > maxRangeUnstable) maxRangeUnstable = currentRange;
                }
            }
            else if (activeUnstable) {
                addAnomaly(report, r, startDayUnstable, startHrUnstable, durUnstable, UNSTABLE, val, maxRangeUnstable);
                activeUnstable = false;
            }
        }

        static double lastV = -1e9;
        if (hasLast) {
            double diff = abs(val - lastValue);
            if (diff > sensorRange * 0.50) {
                double pct = (diff / sensorRange) * 100.0;
                addAnomaly(report, r, r.dayIndex, r.hour, 0, SPIKE, val, pct);
            }
        }

        lastValue = val;
        hasLast = true;
    }

    bin.close();
    printAnomalyReport(report, sensorId);
    freeReport(report);

}


bool hasSensorAnomalies(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {
    ifstream bin("data.bin", ios::binary);
    if (!bin) return false;

    int startYear, startMonth, startDay, daysCount;
    bin.read((char*)&startYear, sizeof(int));
    bin.read((char*)&startMonth, sizeof(int));
    bin.read((char*)&startDay, sizeof(int));
    bin.read((char*)&daysCount, sizeof(int));

    long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
    long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

    BinaryReading r;

    double sensorRange = sensors[sensorId].normalMax - sensors[sensorId].normalMin;
    double lastValue = 0;
    bool hasLast = false;

    double window[12]{};
    int wCount = 0;

    while (bin.read((char*)&r, sizeof(r))) {
        if (r.sensorId != sensorId || r.dayIndex < startOffSet) continue;
        if (r.dayIndex > endOffSet) break;

        double val = r.value;

        if (val > sensors[sensorId].alarmMax || val < sensors[sensorId].alarmMin)
            return true;

        if (hasLast) {
            if (abs(val - lastValue) > sensorRange * 0.50)
                return true;
        }

        lastValue = val;
        hasLast = true;

        window[wCount % 12] = val;
        wCount++;

        if (wCount >= 12) {
            double wMin = window[0], wMax = window[0];
            for (int i = 1; i < 12; i++) {
                if (window[i] < wMin) wMin = window[i];
                if (window[i] > wMax) wMax = window[i];
            }
            if ((wMax - wMin) > sensorRange * 0.60)
                return true;
        }
    }

    return false;
}



double getAnomalyRatio(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {
    ifstream bin("data.bin", ios::binary);
    if (!bin) return 0.0;

    int startYear, startMonth, startDay, daysCount;
    bin.read((char*)&startYear, sizeof(int));
    bin.read((char*)&startMonth, sizeof(int));
    bin.read((char*)&startDay, sizeof(int));
    bin.read((char*)&daysCount, sizeof(int));

    long startOffSet = getDifference(startYear, startMonth, startDay, y1, m1, d1);
    long endOffSet = getDifference(startYear, startMonth, startDay, y2, m2, d2);

    BinaryReading r;
    int totalReadings = 0;
    int badReadings = 0;

    double sensorRange = sensors[sensorId].normalMax - sensors[sensorId].normalMin;
    if (sensorRange <= 0) sensorRange = 1.0;

    double lastValue = 0;
    bool hasLast = false;

    while (bin.read((char*)&r, sizeof(r))) {
        if (r.sensorId != sensorId || r.dayIndex < startOffSet) continue;
        if (r.dayIndex > endOffSet) break;

        totalReadings++;
        bool isBad = false;

        if (r.value > sensors[sensorId].alarmMax || r.value < sensors[sensorId].alarmMin) {
            isBad = true;
        }

        if (hasLast && !isBad) {
            if (abs(r.value - lastValue) > sensorRange * 0.50) isBad = true;
        }

        if (isBad) badReadings++;
        lastValue = r.value;
        hasLast = true;
    }
    bin.close();

    if (totalReadings == 0) return 0.0;
    return (double)badReadings / totalReadings;
}