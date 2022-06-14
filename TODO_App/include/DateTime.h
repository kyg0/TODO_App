#pragma once /// be careful with this. It is not always reliable or supported by all complilers. Usually #ifdef is used. It can stay like this, just a reminder
#pragma warnings(disable:4996) // not a good idea to disable warnings in this phase of the project. They are present for the reason :)
//#define _CRT_SECURE_NO_WARNINGS
#include <ctime>
#include <string>
#include <iostream> // is it needed to be included here, instead on top of the .c file
#include <regex>  // is it needed to be included here, instead on top of the .c file

#include "DateTimeWorker.h"

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
	DateTimeWorkerInterface* dtWorker = new DateTimeWorker(); // Why this initialization is done here and not in constructor? And why is it public?

	DateTime(DateTimeWorkerInterface* worker = nullptr);
	DateTime(int, int, int); // You should also add descriptive argument names in all those definitions, instead of just types, so that it is clear what each argument represents.
                             // It has great significance for later maintenace. Otherwise, it needs to be documented thorougly somewhere else
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
