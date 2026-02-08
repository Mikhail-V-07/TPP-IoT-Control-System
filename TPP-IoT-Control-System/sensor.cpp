#include "sensor.h"

int sensorCount = 24;

sensorTemplate* sensors = new sensorTemplate[sensorCount];

void initSensors() {
	sensors[0].name = "Steam temperature";
	sensors[0].unit = "C";
	sensors[0].normalMax = 560;
	sensors[0].normalMin = 420;
	sensors[0].alarmMax = 600;
	sensors[0].alarmMin = 380;
	sensors[0].relatedFactor = boilerLoad;

	sensors[1].name = "Steam pressure";
	sensors[1].unit = "MPa";
	sensors[1].normalMax = 25;
	sensors[1].normalMin = 10;
	sensors[1].alarmMax = 27;
	sensors[1].alarmMin = 9;
	sensors[1].relatedFactor = boilerLoad;

	sensors[2].name = "Feedwater temperature";
	sensors[2].unit = "C";
	sensors[2].normalMax = 180;
	sensors[2].normalMin = 90;
	sensors[2].alarmMax = 200;
	sensors[2].alarmMin = 80;
	sensors[2].relatedFactor = boilerLoad;

	sensors[3].name = "Condensate temperature";
	sensors[3].unit = "C";
	sensors[3].normalMax = 60;
	sensors[3].normalMin = 30;
	sensors[3].alarmMax = 70;
	sensors[3].alarmMin = 25;
	sensors[3].relatedFactor = boilerLoad;

	sensors[4].name = "Boiler water level";
	sensors[4].unit = "%";
	sensors[4].normalMax = 65;
	sensors[4].normalMin = 45;
	sensors[4].alarmMax = 75;
	sensors[4].alarmMin = 35;
	sensors[4].relatedFactor = boilerLoad;

	sensors[5].name = "Water flow rate";
	sensors[5].unit = "m3/h";
	sensors[5].normalMax = 450;
	sensors[5].normalMin = 120;
	sensors[5].alarmMax = 520;
	sensors[5].alarmMin = 90;
	sensors[5].relatedFactor = boilerLoad;

	sensors[6].name = "Steam flow rate";
	sensors[6].unit = "t/h";
	sensors[6].normalMax = 200;
	sensors[6].normalMin = 50;
	sensors[6].alarmMax = 230;
	sensors[6].alarmMin = 40;
	sensors[6].relatedFactor = boilerLoad;

	sensors[7].name = "Turbine temperature";
	sensors[7].unit = "C";
	sensors[7].normalMax = 450;
	sensors[7].normalMin = 250;
	sensors[7].alarmMax = 500;
	sensors[7].alarmMin = 220;
	sensors[7].relatedFactor = turbineLoad;

	sensors[8].name = "Turbine oil pressure";
	sensors[8].unit = "bar";
	sensors[8].normalMax = 6;
	sensors[8].normalMin = 3;
	sensors[8].alarmMax = 7;
	sensors[8].alarmMin = 2.5;
	sensors[8].relatedFactor = turbineLoad;

	sensors[9].name = "Turbine vibration";
	sensors[9].unit = "mm/s";
	sensors[9].normalMax = 3;
	sensors[9].normalMin = 0.5;
	sensors[9].alarmMax = 6;
	sensors[9].alarmMin = 0;
	sensors[9].relatedFactor = turbineLoad;

	sensors[10].name = "Bearing temperature";
	sensors[10].unit = "C";
	sensors[10].normalMax = 80;
	sensors[10].normalMin = 40;
	sensors[10].alarmMax = 95;
	sensors[10].alarmMin = 0;
	sensors[10].relatedFactor = turbineLoad;

	sensors[11].name = "Flue gas temperature";
	sensors[11].unit = "C";
	sensors[11].normalMax = 220;
	sensors[11].normalMin = 120;
	sensors[11].alarmMax = 260;
	sensors[11].alarmMin = 100;
	sensors[11].relatedFactor = combustionQuality;

	sensors[12].name = "Oxygen in flue gas";
	sensors[12].unit = "%";
	sensors[12].normalMax = 6;
	sensors[12].normalMin = 2;
	sensors[12].alarmMax = 8;
	sensors[12].alarmMin = 1;
	sensors[12].relatedFactor = combustionQuality;

	sensors[13].name = "CO in flue gas";
	sensors[13].unit = "ppm";
	sensors[13].normalMax = 200;
	sensors[13].normalMin = 0;
	sensors[13].alarmMax = 400;
	sensors[13].alarmMin = 0;
	sensors[13].relatedFactor = combustionQuality;

	sensors[14].name = "Fuel gas pressure";
	sensors[14].unit = "bar";
	sensors[14].normalMax = 6;
	sensors[14].normalMin = 2;
	sensors[14].alarmMax = 7;
	sensors[14].alarmMin = 1.5;
	sensors[14].relatedFactor = boilerLoad;

	sensors[15].name = "Combustion air temperature";
	sensors[15].unit = "C";
	sensors[15].normalMax = 120;
	sensors[15].normalMin = 20;
	sensors[15].alarmMax = 150;
	sensors[15].alarmMin = 10;
	sensors[15].relatedFactor = combustionQuality;

	sensors[16].name = "Generator temperature";
	sensors[16].unit = "C";
	sensors[16].normalMax = 90;
	sensors[16].normalMin = 40;
	sensors[16].alarmMax = 105;
	sensors[16].alarmMin = 0;
	sensors[16].relatedFactor = electricalLoad;

	sensors[17].name = "Generator current";
	sensors[17].unit = "A";
	sensors[17].normalMax = 1200;
	sensors[17].normalMin = 300;
	sensors[17].alarmMax = 1500;
	sensors[17].alarmMin = 0;
	sensors[17].relatedFactor = electricalLoad;

	sensors[18].name = "Generator voltage";
	sensors[18].unit = "kV";
	sensors[18].normalMax = 10.5;
	sensors[18].normalMin = 9.5;
	sensors[18].alarmMax = 11;
	sensors[18].alarmMin = 9.5;
	sensors[18].relatedFactor = electricalLoad;

	sensors[19].name = "Transformer temperature";
	sensors[19].unit = "C";
	sensors[19].normalMax = 90;
	sensors[19].normalMin = 40;
	sensors[19].alarmMax = 110;
	sensors[19].alarmMin = 0;
	sensors[19].relatedFactor = electricalLoad;

	sensors[20].name = "Hall humidity";
	sensors[20].unit = "%";
	sensors[20].normalMax = 60;
	sensors[20].normalMin = 30;
	sensors[20].alarmMax = 70;
	sensors[20].alarmMin = 20;
	sensors[20].relatedFactor = coolingEfficiency;

	sensors[21].name = "Hall temperature";
	sensors[21].unit = "C";
	sensors[21].normalMax = 30;
	sensors[21].normalMin = 15;
	sensors[21].alarmMax = 35;
	sensors[21].alarmMin = 10;
	sensors[21].relatedFactor = coolingEfficiency;

	sensors[22].name = "Boiler room pressure";
	sensors[22].unit = "kPa";
	sensors[22].normalMax = 103;
	sensors[22].normalMin = 100;
	sensors[22].alarmMax = 107;
	sensors[22].alarmMin = 95;
	sensors[22].relatedFactor = coolingEfficiency;

	sensors[23].name = "Fuel level";
	sensors[23].unit = "%";
	sensors[23].normalMax = 90;
	sensors[23].normalMin = 30;
	sensors[23].alarmMax = 95;
	sensors[23].alarmMin = 15;
	sensors[23].relatedFactor = fuelLevel;
}