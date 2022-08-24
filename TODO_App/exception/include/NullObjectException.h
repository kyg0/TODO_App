#pragma once
#include "../include/ReportException.h"

#define TYPE_NULL_OBJ_EXCEPTION "NullObject"

#define ERR_MSG_NULL_NEW_OBJ "New object is null"
#define ERR_MSG_NULL_OBJ "Object is null"
#define ERR_MSG_WRONG_FORMAT "Wrong format";
#define ERR_MSG_DEFAULT "Something went wrong"

class NullObjectException : public ReportException {
public:
	NullObjectException(char* msg, std::string type = "");
	NullObjectException(std::string msg, std::string type = "");
};