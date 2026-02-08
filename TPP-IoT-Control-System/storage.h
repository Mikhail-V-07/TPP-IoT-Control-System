#pragma once
#include <cstdint>

#pragma pack(push, 1)
struct BinaryReading {
	float value;
	int32_t sensorId;
	int8_t status;
	int32_t dayIndex;
	int8_t hour;
};
#pragma pack(pop)

struct binHeader {
	int32_t year, month, day, daysCount;
};

const int RECORDS_PER_HOUR = 24;
const int HOURS_PER_DAY = 24;

const int HEADER_SIZE = sizeof(binHeader);