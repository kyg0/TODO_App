#include "../include/InvalidCommandException.h"

InvalidCommandException::InvalidCommandException(char* msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_INV_COMMAND_EXCEPTION) + std::string(AUX_MSG) + std::string(msg);
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + std::string(msg);
	}
}

InvalidCommandException::InvalidCommandException(std::string msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_INV_COMMAND_EXCEPTION) + std::string(AUX_MSG) + msg;
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + msg;
	}
}