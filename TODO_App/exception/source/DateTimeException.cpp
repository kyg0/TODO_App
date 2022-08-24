#include "../include/DateTimeException.h"

DateTimeException::DateTimeException(std::string msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_DATETIME_EXCEPTION) + std::string(AUX_MSG) + msg;
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + msg;
	}
}

DateTimeException::DateTimeException(char* msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_DATETIME_EXCEPTION) + std::string(AUX_MSG) + std::string(msg);
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + std::string(msg);
	}
}