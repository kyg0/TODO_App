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
	// Even in case of such simple functions, good practice is to avoid having definition in .h file.
	// Move implementation into a separate DateTimeException.c file, keep only declerations here. In that way someone who uses your
	// library could mock your functions in their tests.
	DateTimeException(char* err_msg) {
		this->err_msg = "DateTime Exception => " + std::string(err_msg);
	}

	DateTimeException(std::string err_msg) {
		this->err_msg = "DateTime Exception => " + err_msg;
	}

	std::string what() {
		return err_msg;
	}

	// Additional remark: When checking other .h files in exception, I noticed a pattern. They are almost the same. You can use some ReportException parent
	// class that all those DateTimeException, FileException, InvalidExecutionDate... classes could inherit. That class would have this implementation.
	// Child classes could have std::string type (type= "Date Time Exception" or type = "File Exception"... ) that is set in a constructor of child class
};
