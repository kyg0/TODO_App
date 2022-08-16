#pragma once
#include <string>

#define ERR_MSG_NULL_NEW_OBJ "New object is null"
#define ERR_MSG_NULL_OBJ "Object is null"
#define ERR_MSG_WRONG_FORMAT "Wrong format";
#define ERR_MSG_DEFAULT "Something went wrong"

class NullObjectException : public std::exception {
	std::string err_msg;

public:
	// Same as for DateTimeException. Move definitions into a separate file
	NullObjectException(char* msg) {
		this->err_msg = "NullObject Exception => " + std::string(msg);
	}

	NullObjectException(std::string msg) {
		this->err_msg = "NullObject Exception => " + msg;
	}

	std::string what() {
		return err_msg;
	}
};
