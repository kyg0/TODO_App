#include "../include/NullObjectException.h"

NullObjectException::NullObjectException(char* msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_NULL_OBJ_EXCEPTION) + std::string(AUX_MSG) + std::string(msg);
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + std::string(msg);
	}
}

NullObjectException::NullObjectException(std::string msg,std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_NULL_OBJ_EXCEPTION) + std::string(AUX_MSG) + msg;
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + msg;
	}
}