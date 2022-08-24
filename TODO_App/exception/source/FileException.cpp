#include "../include/FileException.h"

FileException::FileException(char* err_msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_FILE_EXCEPTION) + std::string(AUX_MSG) + std::string(err_msg);
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + std::string(err_msg);
	}
}

FileException::FileException(std::string err_msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_FILE_EXCEPTION) + std::string(AUX_MSG) + err_msg;
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + err_msg;
	}
}