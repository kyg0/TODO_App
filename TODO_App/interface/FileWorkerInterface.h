#pragma once
#include <fstream> // doesn't seem that you need all those includes here. Move unnecessary ones to .c files
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#define ERR_MSG_FILE_FAILED_TO_OPEN "File failed to open"

namespace fs = std::filesystem;

class FileWorkerInterface {
public:
	virtual void writeInFile(std::string path, std::string output) = 0;
	virtual std::string readFromFile(std::string path) = 0;
	virtual std::vector<std::string> readFromFileInLines(std::string path) = 0;
	virtual bool deleteFile(std::string path) = 0;
	virtual std::vector<std::string> getAllFromDirectory(std::string dir_path) = 0;
	virtual bool fileExists(std::string path) = 0;
};
