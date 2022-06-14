#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

class FileWorkerInterface {
public:
	virtual void writeInFile(std::string, std::string) = 0;
	virtual std::string readFromFile(std::string) = 0;
	virtual std::vector<std::string> readFromFileInLines(std::string) = 0;
	virtual bool deleteFile(std::string) = 0;
	virtual std::vector<std::string> getAllFromDirectory(std::string) = 0;
	virtual bool exists(std::string) = 0;
};
