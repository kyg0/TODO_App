#include "../include/ReportException.h"

ReportException::ReportException() {
	this->type = "";
	this->err_msg = AUX_MSG;
}

ReportException::ReportException(std::string msg, std::string type) {
	if (type == "") {
		this->err_msg = AUX_MSG + msg;
	}
	else {
		this->err_msg = type + AUX_MSG + msg;
	}
}

ReportException::ReportException(char* msg, std::string type) {
	if (type == "") {
		this->err_msg = AUX_MSG + std::string(msg);
	}
	else {
		this->err_msg = type + AUX_MSG + std::string(msg);
	}
}

std::string ReportException::what() {
	return err_msg;
}