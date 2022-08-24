#pragma once
#include "../include/ReportException.h"

#define TYPE_INV_COMMAND_EXCEPTION "InvalidCommand"

#define TOO_MANY_ARGS "Too many arguments"
#define TOO_FEW_ARGS "Too few arguments"
#define WRONG_FORMAT "Wrong format for this command"
#define WRONG_ARGUMENT "Wrong argument"
#define WRONG_USE "Wrong use of this command"
#define NO_DATE "You have to define date"
#define UNKNOWN_ERROR "Unknown error occured"
#define WRONG_ARGS "Wrong set of arguments"

class InvalidCommandException : public ReportException {
public:
	InvalidCommandException(char* msg, std::string type = "");
	InvalidCommandException(std::string msg, std::string = "");
};