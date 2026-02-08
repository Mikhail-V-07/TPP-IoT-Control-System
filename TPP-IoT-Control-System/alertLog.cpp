#include "alertLog.h"
#include "Analytics.h"
#include "storage.h"
#include "sensor.h"
#include "trends.h"
#include "utilities.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

using namespace std;

void printIncidentDossier(int year, int month, int day, IncidentRecord* incidents, int count, int totalReadings) {
    if (count == 0) return;

    cout << "\n";
    printMenuHeader("INCIDENT DOSSIER: " + to_string(year) + "-" + to_string(month) + "-" + to_string(day));

    cout << " [!] CRITICAL EVENT DETECTED\n";
    cout << " Affected subsystems: " << count << "\n";
    cout << string(60, '-') << "\n";
    cout << left << setw(25) << "SENSOR NAME" << setw(10) << "PEAK" << setw(10) << "LIMIT" << setw(15) << "SEVERITY" << "\n";
    cout << string(60, '-') << "\n";

    int worstId = -1;
    double maxDeviation = -1.0;

    for (int i = 0; i < count; i++) {
        IncidentRecord& inc = incidents[i];

        double limit = sensors[inc.sensorId].alarmMax;
        double deviation = (inc.maxValue - limit);
        string severity;

        if (deviation > limit * 0.1) {
            severity = "CRITICAL";
        }
        else {
            severity = "WARNING";
        }
        

        if (inc.maxValue < sensors[inc.sensorId].normalMin) {
            limit = sensors[inc.sensorId].alarmMin;
            deviation = limit - inc.maxValue;
        }

        if (abs(deviation) > maxDeviation) {
            maxDeviation = abs(deviation);
            worstId = inc.sensorId;
        }

        cout << left << setw(25) << sensors[inc.sensorId].name
            << fixed << setprecision(2) << setw(10) << inc.maxValue
            << setw(10) << limit
            << setw(15) << severity << "\n";
    }
    cout << "\n";

    if (worstId != -1) {
        cout << " >>> ROOT CAUSE ANALYSIS (Deep Dive for " << sensors[worstId].name << ")\n";

        trendResult tr = getTrendData(worstId, year, month, day, year, month, day);

        cout << " 1. Trend Dynamics: ";
        if (tr.type == TREND_UP) cout << "Rapid Growth detected (Slope: " << tr.slope << ")\n";
        else if (tr.type == TREND_DOWN) cout << "Rapid Drop detected (Slope: " << tr.slope << ")\n";
        else if (tr.type == TREND_OSCILLATING_DANGEROUS) cout << "High Instability / Vibration\n";
        else cout << "Sudden Spike (Impulse)\n";

        cout << " 2. Related Factor: " << getFactorName(sensors[worstId].relatedFactor) << "\n";
        cout << " 3. Recommendation: ";

        if (tr.type == TREND_OSCILLATING_DANGEROUS)
            cout << "Check PID controller settings.\n";
        else if (sensors[worstId].relatedFactor == boilerLoad)
            cout << "Inspect boiler pressure valves immediately.\n";
        else
            cout << "Scheduled maintenance check suggested.\n";
    }

    cout << string(60, '=') << "\n";
}


void generateGlobalIncidentReport(int startYear, int startMonth, int startDay, int daysCount) {
    ifstream bin("data.bin", ios::binary);
    if (!bin) {
        cout << "Error: No data file found.\n";
        return;
    }

    bin.seekg(HEADER_SIZE, ios::beg);

    BinaryReading r;

    int currentDayIdx = -1;

    IncidentRecord* dailyIncidents = new IncidentRecord[sensorCount];
    int incidentCount = 0;

    int dailyReadingsCount = 0;

    cout << "Scanning " << daysCount << " days of history... Please wait.\n";

    while (bin.read((char*)&r, sizeof(r))) {
        if (r.dayIndex != currentDayIdx) {
            if (currentDayIdx != -1 && incidentCount > 0) {
                int y, m, d;
                absDaysToDate(dateToAbsDays(startYear, startMonth, startDay) + currentDayIdx, y, m, d);
                printIncidentDossier(y, m, d, dailyIncidents, incidentCount, dailyReadingsCount);
            }

            currentDayIdx = r.dayIndex;
            incidentCount = 0;
            dailyReadingsCount = 0;
        }

        dailyReadingsCount++;

        if (r.status == 2) {

            bool found = false;
            for (int i = 0; i < incidentCount; i++) {
                if (dailyIncidents[i].sensorId == r.sensorId) {
                    if (r.value > dailyIncidents[i].maxValue) {
                        dailyIncidents[i].maxValue = r.value;
                    }
                    dailyIncidents[i].badReadingsCount++;
                    found = true;
                    break;
                }
            }

            if (!found && incidentCount < sensorCount) {
                dailyIncidents[incidentCount].sensorId = r.sensorId;
                dailyIncidents[incidentCount].maxValue = r.value;
                dailyIncidents[incidentCount].badReadingsCount = 1;
                incidentCount++;
            }
        }
    }

    if (incidentCount > 0) {
        int y, m, d;
        absDaysToDate(dateToAbsDays(startYear, startMonth, startDay) + currentDayIdx, y, m, d);
        printIncidentDossier(y, m, d, dailyIncidents, incidentCount, dailyReadingsCount);
    }

    delete[] dailyIncidents;
    bin.close();
    cout << "\n[DONE] Global report generation completed.\n";
}