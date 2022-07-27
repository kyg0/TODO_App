#pragma once
#include <string>

#define ERR_MSG_FW_NULL "Unable to perfom operation. FileWorker is null."

class FileException : public std::exception {
	std::string err_msg;

public:
	FileException(char* err_msg) {
		this->err_msg = "File Exception => " + std::string(err_msg);
	}

	FileException(std::string err_msg) {
		this->err_msg = "File Exception => "  + err_msg;
	}

	std::string what() {
		return err_msg;
	}
};