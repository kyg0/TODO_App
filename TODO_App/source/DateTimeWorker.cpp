#include "../include/DateTimeWorker.h"

std::vector<int> DateTimeWorker::GetCurrentDateAndTime() {
		std::time_t _current = std::time(0);
		std::tm* _localTime = std::localtime(&_current);

		int day = _localTime->tm_mday;
		int month = 1 + _localTime->tm_mon;
		int year = 1900 + _localTime->tm_year;
		int hours = _localTime->tm_hour;
		int minutes = _localTime->tm_min;
		int seconds = _localTime->tm_sec;

		std::vector<int> date;
		date.push_back(day);
		date.push_back(month);
		date.push_back(year);
		date.push_back(hours);
		date.push_back(minutes);
		date.push_back(seconds);

		return date;
}