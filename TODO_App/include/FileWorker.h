#pragma once
#include "../interface/FileWorkerInterface.h"

class FileWorker :
	public FileWorkerInterface
{
public:
	void writeInFile(std::string path, std::string output);
	std::string readFromFile(std::string path);
	StringVector readFromFileInLines(std::string path);
	bool deleteFile(std::string path);
	StringVector getAllFromDirectory(std::string dir_path);
	bool fileExists(std::string path);
};