#pragma once
#include "../interface/FileWorkerInterface.h"

class FileWorker :
    public FileWorkerInterface
{
public:
	void writeInFile(std::string path, std::string output);
	std::string readFromFile(std::string path);
	std::vector<std::string> readFromFileInLines(std::string path);
	bool deleteFile(std::string path);
	std::vector<std::string> getAllFromDirectory(std::string dir_path);
	bool fileExists(std::string path);
};

