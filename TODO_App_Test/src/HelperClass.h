#pragma once
#include <string>

class HelperClass {
public:
	std::string getFormat(int n) {
		if (n >= 10)
			return std::to_string(n);
		return "0" + std::to_string(n);
	}

	std::string getPath(int day, int month, int year, int hour, int min, int sec) {
		std::string yy;
		if (year < 1000 || year > 9999)
			yy = "0000";
		else
			yy = std::to_string(year);
		return getFormat(day) + getFormat(month) + yy + "_" + getFormat(hour) + getFormat(min) + getFormat(sec);
	}
};