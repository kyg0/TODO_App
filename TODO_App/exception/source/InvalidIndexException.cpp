#include "../include/InvalidIndexException.h"

InvalidIndexException::InvalidIndexException(char* msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_INV_INDEX_EXCEPTION) + std::string(AUX_MSG) + std::string(msg);
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + std::string(msg);
	}
}

InvalidIndexException::InvalidIndexException(std::string msg, std::string type) {
	if (type == "") {
		this->err_msg = std::string(TYPE_INV_INDEX_EXCEPTION) + std::string(AUX_MSG) + msg;
	}
	else {
		this->err_msg = type + std::string(AUX_MSG) + msg;
	}
}