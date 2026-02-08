#include "prediction.h"
#include "trends.h"
#include "utilities.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>

using namespace std;

void predictSensorFailure(int sensorId, int endYear, int endMonth, int endDay, int windowDays) {
    long endAbs = dateToAbsDays(endYear, endMonth, endDay);
    long startAbs = endAbs - windowDays;

    int startY, startM, startD;
    absDaysToDate(startAbs, startY, startM, startD);

    trendResult tr = getTrendData(sensorId, startY, startM, startD, endYear, endMonth, endDay);

    if (tr.lastValue == 0 && tr.slope == 0) {
        cout << " [!] ERROR: No data found in the specified window.\n";
        cout << " Check if the simulation has reached these dates.\n";
        return;
    }



    printMenuHeader("PREDICTION MODULE: " + sensors[sensorId].name);

    cout << " Analysis Base: Last " << windowDays << " days\n";
    cout << " Current Value: " << fixed << setprecision(2) << tr.lastValue << " " << sensors[sensorId].unit << "\n";
    cout << " Trend Slope: " << tr.slope << " units/step\n";

    cout << string(60, '-') << "\n";

    if (abs(tr.slope) < 0.001) {
        cout << " [OK] System is STABLE.\n";
        cout << " Forecast: Value will remain steady.\n";
        return;
    }

    double limit;

    if (tr.slope > 0) {
        limit = sensors[sensorId].alarmMax;
    }
    else {
        limit = sensors[sensorId].alarmMin;
    }
    double criticalDelta = limit - tr.lastValue;

    double stepsToFailure = criticalDelta / tr.slope;
    double daysToFailure = stepsToFailure / 24.0;

    cout << " PROJECTED SCENARIO:\n";

    if (daysToFailure < 0) {
        cout << " [SAFE] Parameter is moving AWAY from critical limit.\n";
    }
    else {
        cout << " [WARNING] Parameter is drifting towards CRITICAL LIMIT (" << limit << ").\n";

        if (daysToFailure < 1.0) {
            cout << " [!!!] DANGER: CRITICAL FAILURE IMMINENT (< 24 HOURS)!\n";
        }
        else if (daysToFailure < 7.0) {
            cout << " [!] ALERT: Failure predicted in " << daysToFailure << " days.\n";
        }
        else {
            cout << " [i] INFO: Long-term drift. Failure in " << daysToFailure << " days.\n";
        }

        cout << "\n Forecast Table (Next 5 Days):\n";
        cout << " Day | Predicted Value\n";
        cout << " ----+----------------\n";
        for (int i = 1; i <= 5; i++) {
            double predVal = tr.lastValue + (tr.slope * 24 * i);
            cout << " +" << i << "  | " << predVal << " " << sensors[sensorId].unit;
            if ((tr.slope > 0 && predVal > limit) || (tr.slope < 0 && predVal < limit)) {
                cout << " (FAILURE)";
            }
            cout << "\n";
        }
    }
    cout << "\n";
}