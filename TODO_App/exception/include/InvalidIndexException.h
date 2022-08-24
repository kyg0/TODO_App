#pragma once
#include "../include/ReportException.h"

#define TYPE_INV_INDEX_EXCEPTION "InvalidIndex"

#define ERR_MSG_INVALID_INDEX "Invalid index"
#define ERR_MSG_INVALID_OBJECT "Invalid object"
#define ERR_MSG_INVALID_TITLE_ARGUMENT "Index out of bounds. You haven't define title but you have used argument -t"
#define ERR_MSG_INVALID_DESCRIPTION_ARGUMENT "Index out of bounds. You haven't define description but you have used argument -d"
#define ERR_MSG_INVALID_DATE_ARGUMENT "Index out of bounds. You didn't define date and time argument correctly."

class InvalidIndexException : public ReportException{
public:
	InvalidIndexException(char* msg, std::string type = "");
	InvalidIndexException(std::string msg, std::string type = "");
};