#pragma once
#include <string>

#define ERR_MSG_INVALID_INDEX "Invalid index"
#define ERR_MSG_INVALID_OBJECT "Invalid object"
#define ERR_MSG_INVALID_TITLE_ARGUMENT "Index out of bounds. You haven't define title but you have used argument -t"
#define ERR_MSG_INVALID_DESCRIPTION_ARGUMENT "Index out of bounds. You haven't define description but you have used argument -d"
#define ERR_MSG_INVALID_DATE_ARGUMENT "Index out of bounds. You didn't define date and time argument correctly."

class InvalidIndexException : public std::exception {
	std::string err_msg;

public:
	// Same as for DateTimeException. Move definitions into a separate file
	InvalidIndexException(char* msg) {
		this->err_msg = "InvalidIndex Exception => " + std::string(msg);
	}

	InvalidIndexException(std::string msg) {
		this->err_msg = "InvalidIndex Exception => " + msg;
	}

	std::string what() {
		return err_msg;
	}
};