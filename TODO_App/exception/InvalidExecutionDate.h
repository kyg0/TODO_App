#pragma once
#include <string>

#define ERR_MSG_INVALID_EXEC_DATE "Invalid execution date.Must be in the future"

class InvalidExecutionDate : public std::exception {
	std::string err_msg;

public:
	// Same as for DateTimeException. Move definitions into a separate file
	InvalidExecutionDate(char* msg) {
		this->err_msg = "InvalidExecutionDate Exception => " + std::string(msg);
	}

	InvalidExecutionDate(std::string msg) {
		this->err_msg = "InvalidExecutionDate Exception => " + msg;
	}

	std::string what() {
		return err_msg;
	}
};
