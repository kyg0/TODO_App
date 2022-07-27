#include "../pch.h"
#include "../../TODO_App/include/ReminderEntry.h"
#include "../mock/FileWorkerMock.h"
#include "../mock/DateTimeWorkerMock.h"
#include "../src/HelperClass.h"



using namespace ::testing;

class Adapter {
public:
	FileWorkerInterface* fw = nullptr;

	Adapter() {};
	void InvokeWriteInFile(std::string path, std::string output) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		fw->writeInFile(path, output);
	}

	std::string InvokeReadFromFile(std::string path) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		return fw->readFromFile(path);
	}

	StringVector InvokeReadFromFileInLines(std::string path) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		return fw->readFromFileInLines(path);
	}

	bool InvokeFileExists(std::string path) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		return fw->fileExists(path);
	}

	StringVector InvokeGetAllFromDir(std::string path) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		return fw->getAllFromDirectory(path);
	}

	bool InvokeDeleteFile(std::string path) {
		if (fw == nullptr) {
			throw FileException(ERR_MSG_FW_NULL);
		}
		return fw->deleteFile(path);
	}
};

TEST(FileWorkerTest, NullCheckForFileWorker) {
	Adapter adapter;

	std::string path = "out/path";
	std::string output = "output";

	try {
		adapter.InvokeDeleteFile(path);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}


	try {
		adapter.InvokeGetAllFromDir(path);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}


	try {
		adapter.InvokeReadFromFile(path);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}


	try {
		adapter.InvokeReadFromFileInLines(path);

		"Expected: File Exception => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}


	try {
		adapter.InvokeReadFromFileInLines(path);

		FAIL() << "Expected: FileException => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		adapter.InvokeWriteInFile(path, output);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FW_NULL);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FW_NULL);
		EXPECT_EQ(err.what(), msg);
	}
}

TEST(FileWorkerTest, FileExistsCheck_NullCheckForFileWorker) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path1 = "out/test.txt";
	std::string path2 = "test.txt";
	EXPECT_CALL(*mock, fileExists(path1)).WillOnce(Return(true));
	EXPECT_CALL(*mock, fileExists(path2)).WillOnce(Return(false));

	EXPECT_TRUE(adapter.InvokeFileExists(path1));
	EXPECT_FALSE(adapter.InvokeFileExists(path2));

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, WriteAndReadFromFile) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	std::string output = "This is some random text to write in file";

	EXPECT_CALL(*mock, fileExists(path)).WillRepeatedly(Return(true));
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

	EXPECT_CALL(*mock, readFromFile(path)).WillOnce(Throw(FileException(ERR_MSG_FILE_FAILED_TO_OPEN)));
	try {
		reading = adapter.InvokeReadFromFile(path);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FILE_FAILED_TO_OPEN);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FILE_FAILED_TO_OPEN);
		EXPECT_EQ(err.what(), msg);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(FileWorkerTest, WriteAndReadInLines) {
	FileWorkerMock* mock = new FileWorkerMock();
	Adapter adapter;
	adapter.fw = mock;

	std::string path = "out/test.txt";
	StringVector output;
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
	StringVector lines = adapter.InvokeReadFromFileInLines(path);
	
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
	StringVector reading;

	EXPECT_CALL(*mock, readFromFileInLines(path)).WillOnce(Throw(FileException(ERR_MSG_FILE_FAILED_TO_OPEN)));
	try {
		reading = adapter.InvokeReadFromFileInLines(path);

		FAIL() << "Expected: File Exception => " + std::string(ERR_MSG_FILE_FAILED_TO_OPEN);
	}
	catch (FileException err) {
		std::string msg = "File Exception => " + std::string(ERR_MSG_FILE_FAILED_TO_OPEN);
		EXPECT_EQ(err.what(), msg);
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
