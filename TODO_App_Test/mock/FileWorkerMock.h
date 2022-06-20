#pragma once
#include "../../TODO_App/interface/FileWorkerInterface.h"
#include "../pch.h"

namespace fs = std::filesystem;

class FileWorkerMock : public FileWorkerInterface {
public:
	MOCK_METHOD(void, writeInFile, (std::string, std::string), (override));
	MOCK_METHOD(std::string, readFromFile, (std::string), (override));
	MOCK_METHOD(std::vector<std::string>, readFromFileInLines, (std::string), (override));
	MOCK_METHOD(bool, deleteFile, (std::string), (override));
	MOCK_METHOD(std::vector<std::string>, getAllFromDirectory, (std::string), (override));
	MOCK_METHOD(bool, fileExists, (std::string), (override));
};