#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#include "../exception/include/FileException.h"

#define ERR_MSG_FILE_FAILED_TO_OPEN "File failed to open"

namespace fs = std::filesystem;
using StringVector = std::vector<std::string>;

class FileWorkerInterface {
public:
	virtual void writeInFile(std::string path, std::string output) = 0;
	virtual std::string readFromFile(std::string path) = 0;
	virtual StringVector readFromFileInLines(std::string path) = 0;
	virtual bool deleteFile(std::string path) = 0;
	virtual StringVector getAllFromDirectory(std::string dir_path) = 0;
	virtual bool fileExists(std::string path) = 0;
};