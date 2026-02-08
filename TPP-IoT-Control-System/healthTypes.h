#pragma once
#include "sensor.h"

#include <string>

enum HealthState {
    HEALTH_OK,
    HEALTH_WARNING,
    HEALTH_CRITICAL
};

enum SensorIssueFlags {
    SENSOR_NONE = 0,
    SENSOR_NO_DATA = 1 << 0,
    SENSOR_BAD_TREND = 1 << 1,
    SENSOR_ANOMALIES = 1 << 2,
    SENSOR_OUT_OF_NORM = 1 << 3,
    SENSOR_UNSTABLE = 1 << 4
};


enum UnitIssues {
    UNIT_NONE = 0,
    UNIT_SENSOR_FAILURE = 1 << 0,
    UNIT_SYSTEMATIC_BAD = 1 << 1,
    UNIT_DATA_MISSING = 1 << 2
};

struct SensorHealthReport {
    int sensorId;
    double score;
    SensorIssueFlags issues;
};

struct UnitHealthReport {
    double healthScore;
    HealthState state;
    UnitIssues issues;
    int issueCount;
};

