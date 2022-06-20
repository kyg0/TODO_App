#pragma once
#include <string>

#define ERR_MSG_INVALID_INDEX "Invalid index"

class InvalidIndexException : public std::exception {
	std::string err_msg;

public:
	InvalidIndexException(char* msg) {
		this->err_msg = std::string(msg);
	}

	InvalidIndexException(std::string msg) {
		this->err_msg = msg;
	}

	std::string what() {
		return err_msg;
	}
};