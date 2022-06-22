#pragma once
#include <string>

#define ERR_MSG_FOR_DAYS "Wrong value for days"
#define ERR_MSG_FOR_MONTH "Wrong value for month"
#define ERR_MSG_FOR_YEAR "Wrong value for year"
#define ERR_MSG_FOR_HOURS "Wrong value for hours"
#define ERR_MSG_FOR_MINUTES "Wrong value for minutes"
#define ERR_MSG_FOR_SECONDS "Wrong value for seconds"
#define ERR_MSG_FOR_FORMAT "Wrong format"

class DateTimeException : public std::exception {
	std::string err_msg;

public:
	DateTimeException(char* err_msg) {
		this->err_msg = std::string(err_msg);
	}

	DateTimeException(std::string err_msg) {
		this->err_msg = err_msg;
	}

	std::string what() {
		return err_msg;
	}
};
