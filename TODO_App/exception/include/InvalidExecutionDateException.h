#pragma once
#include "../include/ReportException.h"

#define TYPE_INV_EXECDATE_EXCEPTION "InvalidExecutionDate"

#define ERR_MSG_INVALID_EXEC_DATE "Invalid execution date.Must be in the future"

class InvalidExecutionDateException : public ReportException {
public:
	InvalidExecutionDateException(char* msg, std::string type = "");
	InvalidExecutionDateException(std::string msg, std::string type = "");
};