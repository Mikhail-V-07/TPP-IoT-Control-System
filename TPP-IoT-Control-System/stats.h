#pragma once
#include "sensor.h"
#include "storage.h"

struct statsAccumulator {
	long double sum;
	long double dispersion;

	double min;
	double max;

	int count;

	int timeNormal;
	int timeEmergency;
	int timeWarning;

	double* values;
	int valuesCount;
	int capacity;

	double median;
};


void addReading(statsAccumulator& s, const BinaryReading& r);

void initStats(statsAccumulator& s);

void bubbleSort(double values[], int count);

template <typename T>
void resizeArrays(T*& arr, int& capacity);

template <typename T>

void resizeArrays(T*& arr, int& capacity) {
	int newCapacity = capacity * 2;

	T* newArr = new T[newCapacity];

	for (int i = 0; i < capacity; i++) {
		newArr[i] = arr[i];
	}

	delete[] arr;

	arr = newArr;

	capacity = newCapacity;
}

void finalizeStats(statsAccumulator& s);

void freeStats(statsAccumulator& s);

void printStats(statsAccumulator& s, int sensorId);