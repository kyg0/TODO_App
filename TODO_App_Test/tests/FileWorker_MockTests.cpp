#include "../pch.h"
#include "../../TODO_App/include/ReminderEntry.h"
#include "../mock/FileWorkerMock.h"
#include "../mock/DateTimeWorkerMock.h"
#include "../src/HelperClass.h"

using namespace ::testing;

class Adapter {
public:
	FileWorkerInterface* fw;

	Adapter() {};
	void InvokeWriteInFile(std::string path, std::string output) {
		fw->writeInFile(path, output);
	}

	std::string InvokeReadFromFile(std::string path) {
		return fw->readFromFile(path);
	}

	std::vector<std::string> InvokeReadFromFileInLines(std::string path) {
		return fw->readFromFileInLines(path);
	}

	bool InvokeExists(std::string path) {
		return fw->exists(path);
	}

	std::vector<std::string> InvokeGetAllFromDir(std::string path) {
		return fw->getAllFromDirectory(path);
	}

	bool InvokeDeleteFile(std::string path) {
		return fw->deleteFile(path);
	}
};

TEST(FileWorkerTest, Exists) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path1 = "out/test.txt";
	std::string path2 = "test.txt";
	EXPECT_CALL(*mock, exists(path1)).WillOnce(Return(true));
	EXPECT_CALL(*mock, exists(path2)).WillOnce(Return(false));

	EXPECT_TRUE(adapter.InvokeExists(path1));
	EXPECT_FALSE(adapter.InvokeExists(path2));

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, WriteAndReadFromFile) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	std::string output = "This is some random text to write in file";

	EXPECT_CALL(*mock, exists(path)).WillRepeatedly(Return(true));
	EXPECT_CALL(*mock, writeInFile(path, output)).Times(1);
	EXPECT_CALL(*mock, readFromFile(path)).WillOnce(Return(output));

	adapter.InvokeWriteInFile(path, output);
	std::string reading = adapter.InvokeReadFromFile(path);
	EXPECT_EQ(reading, output);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, InvalidWriteAndReadFromFile) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	std::string output = "This is some random text to write in file";
	std::string reading;

	EXPECT_CALL(*mock, readFromFile(path)).WillOnce(Throw(ERROR_MSG));
	try {
		reading = adapter.InvokeReadFromFile(path);

		FAIL() << "Expected: Failed to open file";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERROR_MSG);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, WriteAndReadInLines) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	std::vector<std::string> output;
	std::string outputInLines;
	for (int i = 1; i <= 10; i++) {
		output.push_back("This is some output in line " + std::to_string(i));
		outputInLines += "This is some output in line " + std::to_string(i);
	}

	for (int i = 0; i < output.size(); i++) {
		EXPECT_CALL(*mock, writeInFile(path, output[i])).Times(1);
	}
	EXPECT_CALL(*mock, readFromFileInLines(path)).WillOnce(Return(output));

	for (int i = 0; i < output.size(); i++) {
		adapter.InvokeWriteInFile(path, output[i]);
	}
	std::vector<std::string> lines = adapter.InvokeReadFromFileInLines(path);
	
	for (int i = 0; i < output.size(); i++) {
		EXPECT_EQ(lines[0], output[0]);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, InvalidWriteAndReadInLines) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	std::string output = "This is some random text to write in file";
	std::vector<std::string> reading;

	EXPECT_CALL(*mock, readFromFileInLines(path)).WillOnce(Throw(ERROR_MSG));
	try {
		reading = adapter.InvokeReadFromFileInLines(path);

		FAIL() << "Expected: Failed to open file!";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERROR_MSG);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, DeleteFile) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string output = "random output";

	std::string correct_path = "out/test.txt";
	std::string incorrect_path = "test.txt";

	EXPECT_CALL(*mock, deleteFile(incorrect_path)).WillOnce(Return(false));
	EXPECT_CALL(*mock, deleteFile(correct_path)).WillOnce(Return(true));
	

	EXPECT_FALSE(adapter.InvokeDeleteFile(incorrect_path));
	EXPECT_TRUE(adapter.InvokeDeleteFile(correct_path));

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}
