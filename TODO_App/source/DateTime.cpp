#include "../include/DateTime.h"


DateTime::DateTime(DateTimeWorkerInterface* worker)
{
	if (worker != nullptr) {
		this->dtWorker = worker;
	}
	std::vector<int> vec = dtWorker->GetCurrentDateAndTime();

	this->day = vec[0];
	this->month = vec[1];
	this->year = vec[2];
	this->hours = vec[3];
	this->minutes = vec[4];
	this->seconds = vec[5];
}

DateTime::DateTime(int day, int month, int year)
{
	try {
		isDateValid(day, month, year);
		this->day = day;
		this->month = month;
		this->year = year;


		this->minutes = 0;
		this->hours = 0;
		this->seconds = 0;
	}
	catch (const char* err) {
		throw err;
	}
}

DateTime::DateTime(int day, int month, int year, int hours, int minutes) 
{
	try {
		isDateValid(day, month, year);
	}
	catch(const char* err) {
		throw err;
	}

	if (hours < 0 || hours > 23) {
		throw "Hours can have value between 0 and 23";
	}
	if (minutes < 0 || minutes >59) {
		throw "Minutes can have value between 0 and 59";
	}

	this->day = day;
	this->month = month;
	this->year = year;
	this->hours = hours;
	this->minutes = minutes;
	this->seconds = 0;
}

DateTime::DateTime(int day, int month, int year, int hours, int minutes, int seconds) 
{
	try {
		isDateValid(day, month, year);
	}
	catch (const char* err) {
		throw err;
	}

	if (hours < 0 || hours > 23) {
		throw "Hours can have value between 0 and 23";
	}
	if (minutes < 0 || minutes >59) {
		throw "Minutes can have value between 0 and 59";
	}
	if (seconds < 0 || seconds > 59) {
		throw "Seconds can have value between 0 and 59";
	}

	this->day = day;
	this->month = month;
	this->year = year;
	this->hours = hours;
	this->minutes = minutes;
	this->seconds = seconds;
}

DateTime::DateTime(std::string& format)
{
	if (format.size() != 19 && format.size() != 16 && format.size() != 10) {
		throw "Wrong format";
	}
	std::string tmp;
	std::regex reg_pattern("[0-9]{2}.[0-9]{2}.[0-9]{4}( [0-9]{2}.[0-9]{2}.[0-9]{2}| [0-9]{2}.[0-9]{2}|)");
	std::smatch match;
	
	if (!regex_search(format, match, reg_pattern)) {
		throw "Wrong format";
	}
	std::string _day;
	std::string _month;
	std::string _year;
	std::string _hour;
	std::string _minute;
	std::string _second;

	if (format.size() == 19) {
		_day = format.substr(0, 2);
		_month = format.substr(3, 2);
		_year = format.substr(6, 4);
		_hour = format.substr(11, 2);
		_minute = format.substr(14, 2);
		_second = format.substr(17, 2);
	}
	else if (format.size() == 16) {
		_day = format.substr(0, 2);
		_month = format.substr(3, 2);
		_year = format.substr(6, 4);
		_hour= format.substr(11, 2);
		_minute = format.substr(14, 2);
		_second = "0";
	}
	else{
		_day = format.substr(0, 2);
		_month = format.substr(3, 2);
		_year = format.substr(6, 4);
		_hour = "0";
		_minute = "0";
		_second = "0";
	}
	

	try {
		isDateValid(std::stoi(_day), std::stoi(_month), std::stoi(_year));
	}
	catch (const char* err) {
		throw err;
	}

	
	if (std::stoi(_hour) < 0 || std::stoi(_hour) > 23) {
		throw "Wrong value for hours";
	}
	if (std::stoi(_minute) < 0 || std::stoi(_minute) > 59) {
		throw "Wrong value for minutes";
	}
	if (std::stoi(_second) < 0 || std::stoi(_second) > 59) {
		throw "Wrong value for seconds";
	}
	

	this->day = std::stoi(_day);
	this->month = std::stoi(_month);
	this->year = std::stoi(_year);
	this->hours = std::stoi(_hour);
	this->minutes = std::stoi(_minute);
	this->seconds = std::stoi(_second);
}

DateTime::DateTime(const DateTime& other)
{
	*this = other;
}

DateTime::DateTime(DateTime&& other) {
	std::swap(*this, other);

}

DateTime::~DateTime()
{
}

DateTime& DateTime::operator=(const DateTime& other)
{
	if (&other == this)
		return *this;

	this->day, other.day;
	this->day = other.day;
	this->month = other.month;
	this->year = other.year;
	this->hours = other.hours;
	this->minutes = other.minutes;
	this->seconds = other.seconds;
}

DateTime& DateTime::operator=(DateTime&& other)
{
	if (&other == this)
		return *this;

	std::swap(this->day, other.day);
	std::swap(this->month, other.month);
	std::swap(this->year, other.year);
	std::swap(this->hours, other.hours);
	std::swap(this->minutes, other.minutes);
	std::swap(this->seconds, other.seconds);
}

bool DateTime::operator>(DateTime& other)
{
	if (this == NULL || &other == NULL) {
		return false;
	}

	if (this->getYear() < other.getYear())
		return true;
	else if (this->getYear() == other.getYear() && this->getMonth() < other.getMonth()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() < other.getDay()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && this->getHours() < other.getHours()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && this->getHours() == other.getHours()
		&& this->getMinutes() < other.getMinutes()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && this->getHours() == other.getHours()
		&& this->getMinutes() == other.getMinutes() && this->getSeconds() < other.getSeconds()) {
		return true;
	}
	return false;
}

bool DateTime::operator<(DateTime& other)
{
	if (this == NULL || &other == NULL) {
		return false;
	}

	if (other.getYear() < this->getYear())
		return true;
	else if (this->getYear() == other.getYear() && other.getMonth() < this->getMonth()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && other.getDay() < this->getDay()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && other.getHours() < this->getHours()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && this->getHours() == other.getHours()
		&& other.getMinutes() < this->getMinutes()) {
		return true;
	}
	else if (this->getYear() == other.getYear() && this->getMonth() == other.getMonth() && this->getDay() == other.getDay() && this->getHours() == other.getHours()
		&& this->getMinutes() == other.getMinutes() && other.getSeconds() < this->getSeconds()) {
		return true;
	}
	return false;
}

bool DateTime::operator==(DateTime& other)
{
	if (!(* this > other) && !(*this < other)) {
		return true;
	}
	return false;
}

void DateTime::setDay(int day)
{
	try {
		isDateValid(day, month, year);
	}
	catch (const char* err) {
		throw err;
	}

	this->day = day;
}

void DateTime::setMonth(int month) {
	try {
		isDateValid(day, month, year);
	}
	catch (const char* err) {
		throw err;
	}

	this->month = month;
}

void DateTime::setYear(int year) {
	try {
		isDateValid(day, month, year);
	}
	catch (const char* err) {
		throw err;
	}

	this->year = year;
}

void DateTime::setHours(int hours) {
	if (hours < 0 || hours > 23)
		throw "Wrong value for hours";

	this->hours = hours;
}

void DateTime::setMinutes(int minutes) {
	if (minutes < 0 || minutes > 59)
		throw "Wrong value for minutes";
	
	this->minutes = minutes;
}

void DateTime::setSeconds(int seconds) {
	if (seconds < 0 || seconds > 59)
		throw "Wrong value for seconds";
	
	this->seconds = seconds;
}

int DateTime::getDay()
{
	return day;
}

int DateTime::getMonth()
{
	return month;
}

int DateTime::getYear()
{
	return year;
}

int DateTime::getHours()
{
	return hours;
}

int DateTime::getMinutes()
{
	return minutes;
}

int DateTime::getSeconds()
{
	return seconds;
}

std::string DateTime::getFormat(bool withSeconds)
{
	std::string format = getCorrectFormat(day) + "/" + getCorrectFormat(month) + "/" + getCorrectFormat(year) + " " 
		+ getCorrectFormat(hours) + ":" + getCorrectFormat(minutes);

	if (withSeconds) {
		std::string str = format + ":" + getCorrectFormat(seconds);
		return str;
	}
	return format;
}

std::string DateTime::getCorrectFormat(int n)
{
	if (n < 10) {
		return "0" + std::to_string(n);
	}
	return std::to_string(n);
}

bool DateTime::isDateValid(int dd, int mm, int yy) {
	bool leap = false;
	if (yy % 400 == 0)
		leap = true;
	else if (yy % 100 == 0)
		leap = false;
	else if (yy % 4 == 0)
		leap = true;
	else
		leap = false;

	if (dd < 1 || dd > 31) {
		throw "Wrong value for days";
	}

	if (mm < 1 || mm > 12) {
		throw "Wrong value for month";
	}

	if (yy < 2020 || yy >9999) {
		throw "Wrong value for year";
	}

	if (leap) {
		if (mm == 2 && dd > 29) {
			throw "Wrong value for days";
		}
		
	}
	else {
		if (mm == 2 && dd > 28) {
			throw "Wrong value for days";
		}
	}

	if ((mm == 4 || mm == 6 || mm == 9 || mm == 11) && dd>30) {
		switch (mm) {
		case 4:
			throw "Wrong value for days";
		case 6:
			throw "Wrong value for days";
		case 9:
			throw "Wrong value for days";
		case 11:
			throw "Wrong value for days";
		}
	}

	return true;
}


