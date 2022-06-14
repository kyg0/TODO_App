#pragma once
#include "../interface/FileWorkerInterface.h"
#define ERROR_MSG "Failed to open file"

class FileWorker :
    public FileWorkerInterface
{
public:
	void writeInFile(std::string, std::string);
	std::string readFromFile(std::string);
	std::vector<std::string> readFromFileInLines(std::string);
	bool deleteFile(std::string);
	std::vector<std::string> getAllFromDirectory(std::string);
	bool exists(std::string);
};

