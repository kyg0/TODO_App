#include "../include/DateTime.h" // Just remark: Usualy you can avoid specifying full path through
                                 // build system, but it is ok to leave it here as it is


DateTime::DateTime(DateTimeWorkerInterface* worker)
{
	if (worker != nullptr) {
		this->dtWorker = worker; // Q: What happens if worker is null? You intentionaly added this logic for optionally changing worker? If dtWorker is null, program will crash
								 // IMPORTANT: it should be also covered with tests
	}

	try {
		std::vector<int> vec = dtWorker->GetCurrentDateAndTime();
		InitDate(vec[0], vec[1], vec[2]);
		InitTime(vec[3], vec[4], vec[5]);
	}
	catch (DateTimeException err) {
		throw err;
	}
	catch (std::exception err) {
		throw err;
	}
}

DateTime::DateTime(int day, int month, int year)
{
	try {
		InitDate(day, month, year);
		InitTime(DEFAULT_TIME_VALUE, DEFAULT_TIME_VALUE, DEFAULT_TIME_VALUE);
	}
	catch (DateTimeException err) {
		throw err;
	}
}

DateTime::DateTime(int day, int month, int year, int hours, int minutes)
{
	try {
		InitDate(day, month, year);
		InitTime(hours, minutes, DEFAULT_TIME_VALUE);
	}
	catch(DateTimeException err) {
		throw err;
	}
}

DateTime::DateTime(int day, int month, int year, int hours, int minutes, int seconds)
{
	try {
		InitDate(day, month, year);
		InitTime(hours, minutes, seconds);
	}
	catch (DateTimeException err) {
		throw err;
	}
}

DateTime::DateTime(std::string& dt_format)
{
	if (dt_format.size() != FULL_DATE_TIME_FORMAT_SIZE
		&& dt_format.size() != DATE_TIME_FORMAT_SIZE_NOSEC
		&& dt_format.size() != DATE_FORMAT_SIZE) {
		throw DateTimeException(ERR_MSG_FOR_FORMAT);
	}
	std::string tmp;
	std::regex reg_pattern("[0-9]{2}.[0-9]{2}.[0-9]{4}( [0-9]{2}.[0-9]{2}.[0-9]{2}| [0-9]{2}.[0-9]{2}|)$");
	std::smatch match;

	if (!regex_search(dt_format, match, reg_pattern)) {
		throw DateTimeException(ERR_MSG_FOR_FORMAT);
	}
	std::string _day;
	std::string _month;
	std::string _year;
	std::string _hour;
	std::string _minute;
	std::string _second;

	if (dt_format.size() == FULL_DATE_TIME_FORMAT_SIZE) {
		_day = dt_format.substr(0, 2);
		_month = dt_format.substr(3, 2);
		_year = dt_format.substr(6, 4);
		_hour = dt_format.substr(11, 2);
		_minute = dt_format.substr(14, 2);
		_second = dt_format.substr(17, 2);
	}
	else if (dt_format.size() == DATE_TIME_FORMAT_SIZE_NOSEC) {
		_day = dt_format.substr(0, 2);
		_month = dt_format.substr(3, 2);
		_year = dt_format.substr(6, 4);
		_hour= dt_format.substr(11, 2);
		_minute = dt_format.substr(14, 2);
		_second = std::to_string(DEFAULT_TIME_VALUE);
	}
	else{
		_day = dt_format.substr(0, 2);
		_month = dt_format.substr(3, 2);
		_year = dt_format.substr(6, 4);
		_hour = std::to_string(DEFAULT_TIME_VALUE);
		_minute = std::to_string(DEFAULT_TIME_VALUE);
		_second = std::to_string(DEFAULT_TIME_VALUE);
	}


	try {
		InitDate(std::stoi(_day), std::stoi(_month), std::stoi(_year));
		InitTime(std::stoi(_hour), std::stoi(_minute), std::stoi(_second));
	}
	catch (DateTimeException err) {
		throw err;
	}
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
	if (&other == this) {
		return *this;
	}

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

	if (this->getYear() < other.getYear()) {
		return true;
	}
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

	if (other.getYear() < this->getYear()) {
		return true;
	}
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
		InitDate(day, this->month, this->year);
	}
	catch (DateTimeException err) {
		throw err;
	}
}

void DateTime::setMonth(int month) {
	try {
		InitDate(this->day, month, this->year);
	}
	catch (DateTimeException err) {
		throw err;
	}
}

void DateTime::setYear(int year) {
	try {
		InitDate(this->day, this->month, year);
	}
	catch (DateTimeException err) {
		throw err;
	}
}

void DateTime::setHours(int hours) {
	if (hours < MIN_HOURS_VALUE || hours > MAX_HOURS_VALUE) {
		throw DateTimeException(ERR_MSG_FOR_HOURS);
	}

	this->hours = hours;
}

void DateTime::setMinutes(int minutes) {
	if (minutes < MIN_MINUTES_VALUE || minutes > MAX_MINUTES_VALUE) {
		throw DateTimeException(ERR_MSG_FOR_MINUTES);
	}

	this->minutes = minutes;
}

void DateTime::setSeconds(int seconds) {
	if (seconds < MIN_SECONDS_VALUE || seconds > MAX_SECONDS_VALUE) {
		throw DateTimeException(ERR_MSG_FOR_SECONDS);
	}

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

void DateTime::InitDate(int day, int month, int year) {
	bool leap = false;
	if (year % 400 == 0) {
		leap = true;
	}
	else if (year % 100 == 0) {
		leap = false;
	}
	else if (year % 4 == 0) {
		leap = true;
	}
	else {
		leap = false;
	}

	if (day < MIN_DAY_VALUE || day > MAX_DAY_VALUE_31DAY_MONTH) {
		throw DateTimeException(ERR_MSG_FOR_DAYS);
	}

	if (month < MIN_MONTH_VALUE || month > MAX_MONTH_VALUE) {
		throw DateTimeException(ERR_MSG_FOR_MONTH);
	}

	if (year < MIN_YEAR_VALUE || year > MAX_YEAR_VALUE) {
		throw DateTimeException(ERR_MSG_FOR_YEAR);
	}

	if (leap) {
		if (month == Month::FEBRUARY && day > MAX_DAY_VALUE_FEBRUATY_LEAP) {
			throw DateTimeException(ERR_MSG_FOR_DAYS);
		}
	}
	else {
		if (month == Month::FEBRUARY && day > MAX_DAY_VALUE_FEBRUARY_NOLEAP) {
			throw DateTimeException(ERR_MSG_FOR_DAYS);
		}
	}

	if ((month == Month::APRIL || month == Month::JUNE || month == Month::SEPTEMBER || month == Month::NOVEMBER) && day > MAX_DAY_VALUE_30DAY_MONTH) {
		throw DateTimeException(ERR_MSG_FOR_DAYS);
	}

	this->day = day;
	this->month = month;
	this->year = year;
}

void DateTime::InitTime(int hours, int minutes, int seconds) {
	if (hours < 0 || hours > 23) {
		throw DateTimeException(ERR_MSG_FOR_HOURS);
	}

	if (minutes < 0 || minutes > 59) {
		throw DateTimeException(ERR_MSG_FOR_MINUTES);
	}

	if (seconds < 0 || seconds > 59) {
		throw DateTimeException(ERR_MSG_FOR_SECONDS);
	}

	this->hours = hours;
	this->minutes = minutes;
	this->seconds = seconds;
}




