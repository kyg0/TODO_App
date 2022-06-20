#pragma once
#include <string>

#define ERR_MSG_INVALID_EXEC_DATE "Invalid execution date.Must be in the future"

class InvalidExecutionDate : public std::exception {
	std::string err_msg;

public:
	InvalidExecutionDate(char* msg) {
		this->err_msg = std::string(msg);
	}

	InvalidExecutionDate(std::string msg) {
		this->err_msg = msg;
	}

	std::string what() {
		return err_msg;
	}
};
