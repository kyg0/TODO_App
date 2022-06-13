#include "../include/FileWorker.h"


void FileWorker::writeInFile(std::string path, std::string input)
{
	std::ofstream outFile(path);
	outFile << input;
	outFile.close();
}

std::string FileWorker::readFromFile(std::string path)
{
	std::ifstream inFile(path);
	if (!inFile) {
		std::cout << ERROR_MSG << std::endl;
		return std::string();
	}

	std::string returnValue = "";

	std::string line;

	while (getline(inFile, line)) {
		returnValue += line + "\n";
	}

	returnValue.pop_back();
	return returnValue;
}

std::vector<std::string> FileWorker::readFromFileInLines(std::string path)
{
	std::ifstream inFile(path);
	if (!inFile) {
		std::cout << ERROR_MSG << std::endl;
		return std::vector<std::string>();
	}

	std::vector<std::string> returnValue;

	std::string line;

	while (getline(inFile, line)) {
		returnValue.push_back(line);
	}

	return returnValue;
}

bool FileWorker::deleteFile(std::string path) {
	if (std::remove(path.c_str()) == 0) {
		return true;
	}
	else
	{
		return false;
	}
	
}

std::vector<std::string> FileWorker::getAllFromDirectory(std::string path)
{
	std::vector<std::string> files;
	for (auto& x : fs::directory_iterator(path)) {
		if (!x.is_directory()) {
			files.push_back(x.path().string());
		}
	}
	return files;
}

bool FileWorker::exists(std::string path) {
	std::ifstream file(path);

	if (file.is_open())
		return true;
	return false;
}

