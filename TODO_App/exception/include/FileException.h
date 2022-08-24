#pragma once
#include "../include/ReportException.h"

#define TYPE_FILE_EXCEPTION "File"

#define ERR_MSG_FW_NULL "Unable to perfom operation. FileWorker is null."

class FileException : public ReportException {
public:
	FileException(char* err_msg, std::string type = "");
	FileException(std::string err_msg, std::string type = "");
};