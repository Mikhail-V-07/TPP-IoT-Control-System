#include "utilities.h"
#include <iostream>
#include <limits>
#include <string>

using namespace std;

int getSafeInt(string message, int minVal, int maxVal) {
	int value;

	while (true) {
		cout << message;
		cin >> value;

		if (cin.fail() || value < minVal || value > maxVal) {
			cout << "Invalid input. Please enter a number";
			if (minVal != -2147483647 || maxVal != 2147483647) {
				cout << " between " << minVal << " and " << maxVal << "\n";

				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
			}
		}
		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			return value;
		}
	}
}

char getSafeChar(string message, string validChars) {

	char value;
	while (true) {
		cout << message;
		cin >> value;

		cin.ignore(numeric_limits<streamsize>::max(), '\n');

		value = toupper(value);

		if (validChars.find(value) != string::npos) {
			return value;
		}

		cout << "Invalid input. Please enter one of: " << validChars << "\n";
	}
}


void getDateRange(int startYear, int startMonth, int startDay, int daysCount, int& y1, int& m1, int& d1, int& y2, int& m2, int& d2){

	long startAbs = dateToAbsDays(startYear, startMonth, startDay);
	long endAbs = startAbs + daysCount - 1;

	int maxYear, maxMonth, maxDay;
	absDaysToDate(endAbs, maxYear, maxMonth, maxDay);

	string dateLimitMsg = "(" + to_string(startYear) + "." + to_string(startMonth) + "." + to_string(startDay) +
	" - " + to_string(maxYear) + "." + to_string(maxMonth) + "." + to_string(maxDay) + ")";

	cout << "\nSimulation range: " << dateLimitMsg << endl;
	char option = getSafeChar("Start analysis from the simulation's first day? (Y/N): ", "YN");

	if (option == 'Y') {
		y1 = startYear; m1 = startMonth; d1 = startDay;
	}
	else {
		cout << "Enter start date for analysis:\n";
		y1 = getSafeInt("  Year: ", startYear, maxYear);
		m1 = getSafeInt("  Month (1-12): ", 1, 12);
		d1 = getSafeInt("  Day: ", 1, daysInMonth(y1, m1));
	}


	option = getSafeChar("Use the simulation's last day as the end date? (Y/N): ", "YN");

	if (option == 'Y') {
		y2 = maxYear; m2 = maxMonth; d2 = maxDay;
	}
	else {
		bool validRange = false;
		do {
			cout << "Enter end date for analysis:\n";
			y2 = getSafeInt("  Year: ", y1, maxYear);
			m2 = getSafeInt("  Month (1-12): ", 1, 12);
			d2 = getSafeInt("  Day: ", 1, daysInMonth(y2, m2));

			if (dateToAbsDays(y2, m2, d2) < dateToAbsDays(y1, m1, d1)) {
				cout << "Error: End date cannot be earlier than start date. Try again.\n";
			}
			else {
				validRange = true;
			}
		} while (!validRange);
	}
}


void printMenuHeader(string title) {
	int minWidth = 34;
	int titleLen = (int)title.length();

	int width = (titleLen + 6 > minWidth) ? titleLen + 6 : minWidth;

	int paddingLeft = (width - titleLen) / 2;
	int paddingRight = width - titleLen - paddingLeft;

	cout << "\n\t+" << string(width, '-') << "+\n";
	cout << "\t|" << string(paddingLeft, ' ') << title << string(paddingRight, ' ') << "|\n";
	cout << "\t+" << string(width, '-') << "+\n";
}


double computeAverage(const double* values, int count) {
	double sum = 0;
	for (int i = 0; i < count; i++) {
		sum += values[i];
	}
	return sum / count;
}

double computeStdDev(const double* values, int count, double avg) {
	long double dispersion = 0.0;
	for (int i = 0; i < count; i++) {
		dispersion += (values[i] - avg) * (values[i] - avg);
	}

	return sqrt(dispersion / count);
}


string getFactorName(int factor) {
	switch (factor) {
	case boilerLoad: return "Boiler Load";
	case turbineLoad: return "Turbine Load";
	case combustionQuality: return "Combustion Quality";
	case electricalLoad: return "Electrical Load";
	case coolingEfficiency: return "Cooling Efficiency";
	case fuelLevel: return "Fuel Management";
	default: return "General";
	}
}

void displaySensorList() {
	initSensors();
	int nameWidth = 30;
	int unitWidth = 10;
	int factorWidth = 20;

	cout << "\n" << string(75, '=') << "\n";
	cout << left << setw(5) << " ID"
		<< setw(nameWidth) << "SENSOR NAME"
		<< setw(unitWidth) << "UNIT"
		<< "RELATED FACTOR" << endl;
	cout << string(75, '-') << "\n";

	for (int i = 0; i < 24; i++) {
		cout << " [" << setw(2) << i << "] "
			<< left << setw(nameWidth) << sensors[i].name
			<< setw(unitWidth) << sensors[i].unit
			<< getFactorName(sensors[i].relatedFactor) << endl;
	}

	cout << string(75, '=') << "\n";
	cout << " Total sensors available: " << sensorCount << "\n\n";
}

string centerText(string text, int width) {
	int len = (int)text.length();
	if (len >= width) return text;
	int leftPad = (width - len) / 2;
	return string(leftPad, ' ') + text;
}



void printExitCat() {
	cout << "\n\n";

	cout << R"(                               
       \`*-.                    
        )  _`-.                 
       .  : `. .                
       : _   '  \               
       ; *` _.   `*-._          
       `-.-'          `-.       
         ;       `       `.     
         :.       .        \    
         . \  .   :   .-'   .   
         '  `+.;  ;  '      :   
         :  '  |    ;       ;-. 
         ; '   : :`-:     _.`* ;
[bug] .*' /  .*' ; .*`- +'  `*' 
      `*-*   `*-*  `*-*'
)";
	cout << "\n      [ SYSTEM SHUTDOWN COMPLETED ]\n";
	cout << "   Thank you for using TPP IoT Analytics!\n";
	cout << " Take care of yourself and don't forget to rest. \n\n";
}