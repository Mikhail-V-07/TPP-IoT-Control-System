#pragma once

struct IncidentRecord {
    int sensorId;
    double maxValue;
    int badReadingsCount;
};

void generateGlobalIncidentReport(int startYear, int startMonth, int startDay, int daysCount);