#include <iostream>
#include <string>

#include "types.h"
#include "sensor.h"
#include "date.h"
#include "utilities.h"
#include "generator.h"
#include "Analytics.h"
#include "anomalies.h"
#include "trends.h"
#include "correlation.h"
#include "unitHealth.h"
#include "alertLog.h"
#include "prediction.h"

using namespace std;

//Main
int main() {
	initSensors();
	initUnitRegistry();

	
	
	bool systemRunning = true;

	cout << "==========================================================\n";
	cout << "   THERMAL POWER PLANT (TPP) IOT CONTROL SYSTEM v1.0      \n";
	cout << "==========================================================\n\n";


	while (systemRunning) {
		int daysCount;
		int startYear, startMonth, startDay;
		int choice;
		printMenuHeader("STAGE 1: SYSTEM INITIALIZATION");
		cout << "To begin analysis, please generate a new operational history.\n";
		daysCount = getSafeInt("\nEnter the number of days recorded by the plant's sensors: ", 0);


		startYear = getSafeInt("Enter the date from which you've been monitoring.\nFirst, enter the year (after 1900): ", 1900);
		startMonth = getSafeInt("Then the month (1-12): ", 1, 12);
		startDay = getSafeInt("And the day (1-" + to_string(daysInMonth(startYear, startMonth)) + "): ", 1, daysInMonth(startYear, startMonth));

		cout << "\n[PROCESS] Generating binary telemetry data..." << endl;
		generateHistory(daysCount, startYear, startMonth, startDay);
		cout << "[SUCCESS] History generated. Files 'data.bin' and 'data.csv' are ready.\n";

		cout << "Welcome to the menu.\n";
		bool analyticsActive = true;
		while (analyticsActive) {
			printMenuHeader("MAIN ANALYTICS MENU");

			cout << " 1.  Display Sensor Registry\n";
			cout << " 2.  Basic Statistical Summary\n";
			cout << " 3.  Temporal Analysis (Hours/Days/Seasons)\n";
			cout << " 4.  Anomaly & Deviation Detection\n";
			cout << " 5.  Linear Trend Analysis\n";
			cout << " 6.  Cross-Sensor Correlation\n";
			cout << " 7.  Structural Unit Health Report\n";
			cout << " 8.  Global Incident Log (Alerts)\n";
			cout << " 9.  Predictive Maintenance (Forecast)\n";
			cout << " 10. RE-GENERATE Simulation (Reset)\n";
			cout << " 11. EXIT System\n";

			choice = getSafeInt("Select an operation (1-11): ", 1, 11);

			switch (choice) {
			case 1: {
				displaySensorList();
				break;
			}
			case 2: {
				int sensorId, y1, y2, m1, m2, d1, d2;

				cout << "=== BASIC STATISTICS ===\n";
				getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
				sensorId = getSafeInt("Please enter the sensor ID to calculate statistics (0-23): ", 0, 23);
				basicStatistics(sensorId, y1, m1, d1, y2, m2, d2);
				break;
			}
			case 3: {
				int sensorId, y1, y2, m1, m2, d1, d2, ch;
				int hour;
				int weekday, month, season;
				do {
					printMenuHeader("=== TEMPORAL ANALYSIS SUBMENU ===");
					cout << "1. Hourly Distribution\n2. Day-of-Week Patterns\n3. Monthly Statistics\n4. Seasonal Variations\n5. Return to Main Menu\n";
					ch = getSafeInt("Enter your choice: ", 1, 5);
					switch (ch) {
					case 1: {
						cout << "\n=== ANALYSIS BY HOURS OF THE DAY ===\n";
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						hour = getSafeInt("Please enter the hour for collecting statistics (0-23): ", 0, 23);
						sensorId = getSafeInt("Please enter the sensor ID to calculate statistics (0-23): ", 0, 23);
						analysisByHour(sensorId, y1, m1, d1, y2, m2, d2, hour);
						break;
					}
					case 2: {
						cout << "\n=== ANALYSIS BY DAYS OF THE WEEK ===\n";
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						weekday = getSafeInt("Please enter the weekday for collecting statistics (0-6): ", 0, 6);
						sensorId = getSafeInt("Please enter the sensor ID to calculate statistics (0-23): ", 0, 23);
						analysisByWeekday(sensorId, y1, m1, d1, y2, m2, d2, weekday);
						break;
					}
					case 3: {
						cout << "\n=== ANALYSIS BY MONTH ===\n";
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						month = getSafeInt("Please enter the month for collecting statistics (1-12): ", 1, 12);
						sensorId = getSafeInt("Please enter the sensor ID to calculate statistics (0-23): ", 0, 23);
						analysisByMonth(sensorId, y1, m1, d1, y2, m2, d2, month);
						break;
					}
					case 4: {
						cout << "\n=== ANALYSIS BY SEASON ===\n";
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						cout << "0-Winter, 1-Spring, 2-Summer, 3-Autumn\n";
						season = getSafeInt("Please enter the season for collecting statistics (0-3): ", 0, 3);
						sensorId = getSafeInt("Please enter the sensor ID to calculate statistics (0-23): ", 0, 23);
						analysisBySeason(sensorId, y1, m1, d1, y2, m2, d2, season);
						break;
					}
					case 5: {
						break;
					}
					default: {
						cout << "Invalid choice number. Please try again!" << endl;
						break;
					}
					}
				} while (ch != 5);
				break;
			}
			case 4: {
				int sensorId, y1, y2, m1, m2, d1, d2;
				printMenuHeader("=== ANOMALY DETECTION ===");

				getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
				sensorId = getSafeInt("Please enter the sensor ID to calculate anomalies (0-23): ", 0, 23);

				detectAnomalies(sensorId, y1, m1, d1, y2, m2, d2);
				break;
			}
			case 5: {
				int sensorId, y1, y2, m1, m2, d1, d2;
				printMenuHeader("=== TREND ANALYSIS ===");

				getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
				sensorId = getSafeInt("Please enter the sensor ID to calculate trends (0-23): ", 0, 23);

				analyzeSensorTrend(sensorId, y1, m1, d1, y2, m2, d2);
				break;
			}
			case 6: {
				int sensorA, sensorB, y1, y2, m1, m2, d1, d2, lagHours, ch;
				printMenuHeader("=== CORRELATION ANALYSIS ===");
				do {
					cout << "1. Compare Two Specific Sensors\n2. Run Global Correlation Matrix\n3. Back\n";
					ch = getSafeInt("Enter your choice: ", 1, 3);
					switch (ch) {
					case 1: {
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						sensorA = getSafeInt("Please enter the first sensor ID to calculate correlation (0-23): ", 0, 23);
						sensorB = getSafeInt("Please enter the second sensor ID to calculate correlation (0-23): ", 0, 23);
						lagHours = getSafeInt("Enter time lag in hours: ", 0, 24);

						analyzeSensorCorrelation(sensorA, sensorB, y1, m1, d1, y2, m2, d2, lagHours);
						break;
					}
					case 2: {
						getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
						lagHours = getSafeInt("Please enter lag (hours) (0-24): ", 0, 24);

						analyzeAllSensorsCorrelation(y1, m1, d1, y2, m2, d2, lagHours);
						break;
					}
					case 3: {
						break;
					}
					default: {
						cout << "Invalid choice number. Please try again!" << endl;
						break;
					}
					}
				} while (ch != 3);
				break;
			}
			case 7: {
				int unitId, y1, y2, m1, m2, d1, d2;
				printMenuHeader("=== UNIT HEALTH REPORT ===");

				getDateRange(startYear, startMonth, startDay, daysCount, y1, m1, d1, y2, m2, d2);
				cout << "1. Steam Boiler Island\n2. Main Steam Turbine\n3. Combustion Control\n4. Power Generation & Grid\n5. Station Aux & Environment\n6. Full Station Report\n";
				unitId = getSafeInt("Select Unit (1-6): ", 1, 6);

				unitHealth(unitId - 1, y1, m1, d1, y2, m2, d2);
				break;
			}
			case 8: {
				printMenuHeader("=== GLOBAL INCIDENT LOG ===");

				generateGlobalIncidentReport(startYear, startMonth, startDay, daysCount);
				break;
			}
			case 9: {
				int sensorId, endYear, endMonth, endDay, windowDays, ch;
				printMenuHeader("=== PREDICTIVE FORECASTING ===");

				long endAbs = dateToAbsDays(startYear, startMonth, startDay) + daysCount - 1;
				absDaysToDate(endAbs, endYear, endMonth, endDay);

				do {
					cout << "1. Specific Sensor Forecast\n2. All Sensors Health Projection\n3. Back\n";
					ch = getSafeInt("Enter your choice: ", 1, 3);
					switch (ch) {
					case 1: {
						sensorId = getSafeInt("Please enter the sensor ID to create a forecast (0-23): ", 0, 23);

						windowDays = getSafeInt("Enter analysis window (days history to use, e.g. 14) (5-100): ", 5, 100);

						predictSensorFailure(sensorId, endYear, endMonth, endDay, windowDays);
						break;
					}
					case 2: {
						windowDays = getSafeInt("Enter analysis window (days history to use, e.g. 14) (5-100): ", 5, 100);
						for (int i = 0; i < sensorCount; i++) {
							predictSensorFailure(i, endYear, endMonth, endDay, windowDays);
						}
						break;
					}
					case 3: {
						break;
					}
					default: {
						cout << "Invalid choice number. Please try again!" << endl;
						break;
					}
					}
				} while (ch != 3);

				break;
			}
			case 10: {
				cout << "\n[RESET] Returning to simulation stage...\n";
				analyticsActive = false;
				break;
			}
			case 11: {
				analyticsActive = false;
				systemRunning = false;
				break;
			}
			default: {
				cout << "\n [!] CRITICAL ERROR: Menu selection out of bounds.\n";
				cout << " Please contact system administrator.\n";
				break;
			}

			}
			if (systemRunning && analyticsActive) {
				cout << "\n---\nAnalysis complete. Press Enter to return to panel...";
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
	}

	printExitCat();

	cout << "\nPress any key to close this window..." << endl;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin.get();

	freeUnitRegistry();
	delete[] sensors;
	return 0;
}