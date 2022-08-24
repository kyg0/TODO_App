#pragma once

#include <regex>
#include "DateTimeWorker.h"
#include "../exception/include/DateTimeException.h"

#define FULL_DATE_TIME_FORMAT_SIZE 19			// dd.mm.yyyy hh:mm:ss
#define DATE_TIME_FORMAT_SIZE_NOSEC 16			// dd.mm.yyyy hh:mm
#define DATE_FORMAT_SIZE 10						// dd.mm.yyyy

#define MIN_DAY_VALUE 1
#define MIN_MONTH_VALUE 1
#define MIN_YEAR_VALUE 2020
#define MIN_HOURS_VALUE 0
#define MIN_MINUTES_VALUE 0
#define MIN_SECONDS_VALUE 0

#define MAX_MONTH_VALUE 12
#define MAX_YEAR_VALUE 9999
#define MAX_HOURS_VALUE 23
#define MAX_MINUTES_VALUE 59
#define MAX_SECONDS_VALUE 59

#define MAX_DAY_VALUE_FEBRUARY_NOLEAP 28
#define MAX_DAY_VALUE_FEBRUATY_LEAP 29
#define MAX_DAY_VALUE_30DAY_MONTH 30
#define MAX_DAY_VALUE_31DAY_MONTH 31

#define DEFAULT_TIME_VALUE 0

enum Month {
	JANUARY = 1,
	FEBRUARY = 2,
	MARCH = 3,
	APRIL = 4,
	MAY = 5,
	JUNE = 6,
	JULY = 7,
	AUGUST = 8,
	SEPTEMBER = 9,
	OCTOBER = 10,
	NOVEMBER = 11,
	DECEMBER = 12
};

class DateTime
{
private:
	int day;
	int month;
	int year;
	int hours;
	int minutes;
	int seconds;

public:
	DateTimeWorkerInterface* dtWorker = new DateTimeWorker();

	DateTime(DateTimeWorkerInterface* worker = nullptr);
	DateTime(int day, int month, int year);
	DateTime(int day, int month, int year, int hours, int minutes);
	DateTime(int day, int month, int year, int hours, int minutes, int seconds);
	DateTime(std::string& dt_format);
	DateTime(const DateTime& other);
	DateTime(DateTime&& other);
	~DateTime();

	DateTime& operator=(const DateTime& other);
	DateTime& operator=(DateTime&& other);
	bool operator>(DateTime& other);			// A>B means A is earlier than B; example: 06/06/2022 > 07/06/2022
	bool operator<(DateTime& other);
	bool operator==(DateTime& other);

	void setDay(int day);
	void setMonth(int month);
	void setYear(int year);
	void setHours(int hours);
	void setMinutes(int minutes);
	void setSeconds(int seconds);

	int getDay();
	int getMonth();
	int getYear();
	int getHours();
	int getMinutes();
	int getSeconds();

	std::string getFormat(bool withSeconds); //with_seconds = true;
private:
	// Returns the value for value in date or time in correct format
	// example: If date is 6/6/2022 => for dt_value=6 it will return 06
	// correct format would be 06/06/2022
	std::string getCorrectFormat(int dt_value);
	void InitDate(int day, int month, int year);
	void InitTime(int hours, int minutes, int seconds);
};