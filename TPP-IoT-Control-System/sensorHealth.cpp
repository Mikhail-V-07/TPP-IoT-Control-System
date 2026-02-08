#include "sensorHealth.h"
#include "trends.h"
#include "anomalies.h"

#include <fstream>
#include <iostream>

using namespace std;

SensorHealthReport collectUnitSensorHealth(int sensorId, int y1, int m1, int d1, int y2, int m2, int d2) {

	SensorHealthReport r{};

	r.sensorId = sensorId;
	r.score = 100.0;
	r.issues = SENSOR_NONE;

	trendResult t = getTrendData(sensorId, y1, m1, d1, y2, m2, d2);

	double nMax = sensors[sensorId].normalMax;
	double nMin = sensors[sensorId].normalMin;
	double sensorSpan = nMax - nMin;
	if (sensorSpan <= 0) sensorSpan = 1.0;

	if (t.type == TREND_DOWN) {
		r.score -= 20.0;
		r.issues = (SensorIssueFlags)(r.issues | SENSOR_BAD_TREND);

	}

	if (t.type == TREND_OSCILLATING_DANGEROUS) {
		double oscSeverity = (t.range / sensorSpan) * 30.0;

		if (oscSeverity > 30.0) oscSeverity = 30.0;
		if (oscSeverity < 10.0) oscSeverity = 10.0;

		r.score -= oscSeverity;
		r.issues = (SensorIssueFlags)(r.issues | SENSOR_UNSTABLE);
	}


	double anomalyRatio = getAnomalyRatio(sensorId, y1, m1, d1, y2, m2, d2);

	if (anomalyRatio > 0) {
		double penalty = anomalyRatio * 30.0;
		if (penalty > 60.0) penalty = 60.0;

		r.score -= penalty;
		r.issues = (SensorIssueFlags)(r.issues | SENSOR_ANOMALIES);
	}

	if (r.score < 5.0) r.score = 5.0;

	return r;

}
