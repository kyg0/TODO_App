#pragma once
#pragma warnings(disable:4996)
//#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include <string>
#include <iostream>
#include "DateTimeWorker.h"
#include <regex>

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
	DateTime(int, int, int);
	DateTime(int, int, int, int, int);
	DateTime(int, int, int, int, int, int);
	DateTime(std::string&);
	DateTime(const DateTime&);
	DateTime(DateTime&&);
	~DateTime();

	DateTime& operator=(const DateTime&);
	DateTime& operator=(DateTime&&);
	bool operator>(DateTime&); // A>B means A is earlier than B; example: 06/06/2022 > 07/06/2022
	bool operator<(DateTime&);
	bool operator==(DateTime&);

	void setDay(int);
	void setMonth(int);
	void setYear(int);
	void setHours(int);
	void setMinutes(int);
	void setSeconds(int);
	void setDateToNull();

	int getDay();
	int getMonth();
	int getYear();
	int getHours();
	int getMinutes();
	int getSeconds();

	std::string getFormat(bool); //with_seconds = true;
private:
	std::string getCorrectFormat(int);
	bool isDateValid(int, int, int);
};
