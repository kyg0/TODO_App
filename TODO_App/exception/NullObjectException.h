#pragma once
#include <string>

#define ERR_MSG_NULL_NEW_OBJ "New object is null"
#define ERR_MSG_NULL_OBJ "Object is null"

class NullObjectException : public std::exception {
	std::string err_msg;

public:
	NullObjectException(char* msg) {
		this->err_msg = std::string(msg);
	}

	NullObjectException(std::string msg) {
		this->err_msg = msg;
	}

	std::string what() {
		return err_msg;
	}
};
