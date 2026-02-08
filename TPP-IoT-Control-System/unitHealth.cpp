#include "unitHealth.h"
#include "sensorHealth.h"
#include <iomanip>
#include <iostream>

using namespace std;


unitNode* stationUnits = nullptr;
const int UNIT_COUNT = 5;

string getIssuesString(int issues) {
    if (issues == 0) return "CLEAN";

    string result = "";
    if (issues & 1)  result += "[LOW_DATA]";
    if (issues & 2)  result += "[BAD_VAL]";
    if (issues & 4)  result += "[TREND]";
    if (issues & 8)  result += "[UNSTABLE]";
    if (issues & 16) result += "[ANOMALY]";

    return result;
}




void initUnitRegistry() {
    stationUnits = new unitNode[UNIT_COUNT];

    stationUnits[0].id = 0;
    stationUnits[0].name = "Steam Boiler Island";
    stationUnits[0].sensorCount = 6;
    stationUnits[0].sensorIds = new int[6] {0, 1, 4, 5, 6, 14};

    stationUnits[1].id = 1;
    stationUnits[1].name = "Main Steam Turbine";
    stationUnits[1].sensorCount = 4;
    stationUnits[1].sensorIds = new int[4] {7, 8, 9, 10};

    stationUnits[2].id = 2;
    stationUnits[2].name = "Combustion Control";
    stationUnits[2].sensorCount = 4;
    stationUnits[2].sensorIds = new int[4] {11, 12, 13, 15};

    stationUnits[3].id = 3;
    stationUnits[3].name = "Power Generation & Grid";
    stationUnits[3].sensorCount = 4;
    stationUnits[3].sensorIds = new int[4] {16, 17, 18, 19};

    stationUnits[4].id = 4;
    stationUnits[4].name = "Station Aux & Environment";
    stationUnits[4].sensorCount = 6;
    stationUnits[4].sensorIds = new int[6] {2, 3, 20, 21, 22, 23};
}

void freeUnitRegistry() {
    if (stationUnits == nullptr) return;
    for (int i = 0; i < UNIT_COUNT; i++) {
        delete[] stationUnits[i].sensorIds;
    }
    delete[] stationUnits;
}



UnitHealthReport collectUnitHealth(const unitNode& unit, int y1, int m1, int d1, int y2, int m2, int d2) {
	UnitHealthReport ur{};
	ur.healthScore = 0;
	ur.issues = UNIT_NONE;
	ur.issueCount = 0;

	int badSensors = 0;

	for (int i = 0; i < unit.sensorCount; i++) {

		SensorHealthReport sr = collectUnitSensorHealth(unit.sensorIds[i], y1, m1, d1, y2, m2, d2);

		ur.healthScore += sr.score;

		if (sr.issues != SENSOR_NONE) {
			badSensors++;
			ur.issueCount++;
		}
	}

	if (unit.sensorCount > 0) {
		ur.healthScore /= unit.sensorCount;
	}

	if (badSensors > unit.sensorCount / 2) {
		ur.issues = (UnitIssues)(ur.issues | UNIT_SENSOR_FAILURE);
	}

	if (ur.healthScore > 80) ur.state = HEALTH_OK;
	else if (ur.healthScore > 50) ur.state = HEALTH_WARNING;
	else ur.state = HEALTH_CRITICAL;

	return ur;

}


static const char* healthStateStr(HealthState s) {
    switch (s) {
    case HEALTH_OK: return "OK (STABLE)";
    case HEALTH_WARNING: return "WARNING (CHECK)";
    case HEALTH_CRITICAL: return "CRITICAL (ACTION REQUIRED)";
    default: return "UNKNOWN";
    }
}

void printUnitHealthReport(const unitNode& unit, const UnitHealthReport& report, int y1, int m1, int d1, int y2, int m2, int d2) {
    cout << "\n" << string(50, '=') << "\n";
    cout << " SYSTEM HEALTH REPORT | Unit ID: " << unit.id << "\n";
    cout << string(50, '=') << "\n";

    cout << left << setw(20) << "Unit Name:" << unit.name << "\n";
    cout << left << setw(20) << "Active Sensors:" << unit.sensorCount << "\n";
    cout << left << setw(20) << "Status:" << healthStateStr(report.state) << "\n";

    int barWidth = 20;
    int progress = (int)(report.healthScore / 100.0 * barWidth);
    cout << left << setw(20) << "Health Index: [";
    for (int i = 0; i < barWidth; ++i) cout << (i < progress ? "#" : ".");
    cout << "] " << fixed << setprecision(1) << report.healthScore << "%\n";

    cout << string(50, '-') << "\n";
    cout << left << setw(12) << "SENSOR" << setw(10) << "SCORE" << "ISSUES / FLAGS\n";
    cout << string(50, '-') << "\n";

    for (int i = 0; i < unit.sensorCount; i++) {
        int sid = unit.sensorIds[i];
        SensorHealthReport sr = collectUnitSensorHealth(sid, y1, m1, d1, y2, m2, d2);

        cout << "  #" << left << setw(9) << sid
            << " | " << setw(7) << sr.score
            << " | " << getIssuesString(sr.issues) << "\n";
    }

    cout << string(50, '=') << "\n\n";
}

void unitHealth(int unitId, int y1, int m1, int d1, int y2, int m2, int d2) {
    initSensors();
    initUnitRegistry();

    for (int i = 0; i < UNIT_COUNT; i++) {

        if (unitId != 5 && stationUnits[i].id != unitId) {
            continue;
        }

        bool found = true;

        UnitHealthReport ur = collectUnitHealth(
            stationUnits[i],
            y1, m1, d1,
            y2, m2, d2
        );

        printUnitHealthReport(
            stationUnits[i],
            ur,
            y1, m1, d1,
            y2, m2, d2
        );
    }

    freeUnitRegistry();
}



