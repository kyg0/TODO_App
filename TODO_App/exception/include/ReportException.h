#pragma once
#include <string>
#define AUX_MSG " Exception => "

class ReportException : public std::exception
{
protected:
	std::string type;
	std::string err_msg;
public:
	ReportException();
	ReportException(char* err_msg, std::string type = "");
	ReportException(std::string err_msg, std::string type = "");
	std::string what();
};