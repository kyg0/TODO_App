#pragma once
#include "../include/ReportException.h"


#define TYPE_DATETIME_EXCEPTION "DateTime"

#define ERR_MSG_FOR_DAYS "Wrong value for days"
#define ERR_MSG_FOR_MONTH "Wrong value for month"
#define ERR_MSG_FOR_YEAR "Wrong value for year"
#define ERR_MSG_FOR_HOURS "Wrong value for hours"
#define ERR_MSG_FOR_MINUTES "Wrong value for minutes"
#define ERR_MSG_FOR_SECONDS "Wrong value for seconds"
#define ERR_MSG_FOR_FORMAT "Wrong format"

class DateTimeException : public ReportException {
public:
	DateTimeException(char* err_msg, std::string type = "");
	DateTimeException(std::string err_msg, std::string type = "");
};