#include "../pch.h"
#include "../../TODO_App/include/ReminderEntry.h"
#include "../mock/DateTimeWorkerMock.h"
#include "../mock/FileWorkerMock.h"
#include "../src/HelperClass.h"

using namespace ::testing;

TEST(ReminderEntryTests_Constructor, Constructor_WithFileOutput) {
	std::vector<std::string> fileReading;
	fileReading.push_back("Title: title");
	fileReading.push_back("Description: description");
	fileReading.push_back("Date created: 24/05/2022 13:30:43");
	fileReading.push_back("Execution date: 26/09/2022 05:35:00");
	fileReading.push_back("Status: NOT_FINISHED");

	ReminderEntry* e = new ReminderEntry(fileReading);

	EXPECT_EQ("title", e->getTitle());
	EXPECT_EQ("description", e->getDescription());
	EXPECT_TRUE(DateTime(24, 5, 2022, 13, 30, 43) == *e->getDateCreated());
	EXPECT_TRUE(DateTime(26, 9, 2022, 5, 35) == *e->getExecutionDate());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, e->getEntryStatus());
	std::string expected_path = "out/title_24052022_133043_26092022_053500";
	EXPECT_EQ(expected_path, e->getFilePath());
}

TEST(ReminderEntryTests_Constructor, DefaultConstructor) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;

	
	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+ "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(dtMock, fwMock);
	
	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Constructor, CorrectValues_ForConstructor) {
	HelperClass helper;
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	FileWorkerMock* fwMock = new FileWorkerMock();
	std::vector<int> date{ 26, 9, 2022, 13, 0, 0 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	
	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	ReminderEntry entry("title", "description", dtMock, fwMock);

	EXPECT_TRUE(*entry.getDateCreated() == *dt);
	EXPECT_TRUE(entry.getExecutionDate() == nullptr);
	EXPECT_EQ("title", entry.getTitle());
	EXPECT_EQ("description", entry.getDescription());
	EXPECT_EQ(entry.getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(entry.getFilePath(), _path);

	std::string titleBoundaryCase;
	std::string descriptionBoundaryCase;

	for (int i = 0; i < MAX_TITLE_LEN; i++) {
		titleBoundaryCase += "a";
	}
	for (int i = 0; i < MAX_DESCRIPTION_LEN; i++) {
		descriptionBoundaryCase += "a";
	}

	std::string _ppath = "out/" + titleBoundaryCase + "_" + helper.getPath(26, 9, 2022, 13, 0, 0) + "_"
		+ helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _ooutput = "Title: " + titleBoundaryCase + "\nDescription: " + descriptionBoundaryCase +
		"\nDate created: 26/09/2022 13:00:00\nExecution date: 00/00/0000 00:00:00\nStatus: NOT FINISHED";

	EXPECT_CALL(*fwMock, writeInFile(_ppath, _ooutput)).Times(1);

	ReminderEntry entry2(titleBoundaryCase, descriptionBoundaryCase, dtMock, fwMock);
	EXPECT_EQ(titleBoundaryCase, entry2.getTitle());
	EXPECT_EQ(descriptionBoundaryCase, entry2.getDescription());

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
}

TEST(ReminderEntryTests_Constructor, CorrectValues_ForConstructorWithExecDate) {
	DateTime * execDate = new DateTime(26,9,2100);
	
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;


	std::string _path = "out/title_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) +
		+"_" + helper.getPath(26,9,2100,0,0,0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);
	entry = new ReminderEntry(execDate, "title", "description", dtMock, fwMock);

	
	EXPECT_TRUE(*dt == *entry->getDateCreated());
	EXPECT_TRUE(*execDate == *entry->getExecutionDate());
	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(entry->getFilePath(), _path);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
}

TEST(ReminderEntryTests_Constructor, CorrectValues_ForConstructorWithExecDate_Boundary) {
	std::string titleBoundaryCase;
	std::string descriptionBoundaryCase;
	DateTime* execDate = new DateTime(std::string("01/01/2031 23:59:59"));
	for (int i = 0; i < MAX_TITLE_LEN; i++) {
		titleBoundaryCase += "a";
	}
	for (int i = 0; i < MAX_DESCRIPTION_LEN; i++) {
		descriptionBoundaryCase += "a";
	}
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2030, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;


	std::string _path = "out/" + titleBoundaryCase + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(1,1,2031,23,59,59);
	std::string _title = "Title: " + titleBoundaryCase;
	std::string _description = "Description: " + descriptionBoundaryCase;
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;


	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(execDate, titleBoundaryCase, descriptionBoundaryCase,dtMock, fwMock);


	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_EQ(entry->getTitle(), titleBoundaryCase);
	EXPECT_EQ(entry->getDescription(), descriptionBoundaryCase);
	EXPECT_EQ(entry->getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(entry->getFilePath(), _path);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
}

TEST(ReminderEntryTests_Constructor, CorrectValues_ForConstructorWithExecDate_Boundary_Feb) {
	std::string titleBoundaryCase;
	std::string descriptionBoundaryCase;
	DateTime* execDate = new DateTime(std::string("29/02/2032 01:01:01"));
	for (int i = 0; i < MAX_TITLE_LEN; i++) {
		titleBoundaryCase += "a";
	}
	for (int i = 0; i < MAX_DESCRIPTION_LEN; i++) {
		descriptionBoundaryCase += "a";
	}
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2030, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;


	std::string _path = "out/" + titleBoundaryCase + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(29, 2, 2032, 1, 1, 1);
	std::string _title = "Title: " + titleBoundaryCase;
	std::string _description = "Description: " + descriptionBoundaryCase;
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;


	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(execDate, titleBoundaryCase, descriptionBoundaryCase, dtMock, fwMock);


	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_EQ(entry->getTitle(), titleBoundaryCase);
	EXPECT_EQ(entry->getDescription(), descriptionBoundaryCase);
	EXPECT_EQ(entry->getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(entry->getFilePath(), _path);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
}

TEST(ReminderEntryTests_Constructor, InvalidValues_ForLongTitleAndDescription) {
	std::string longDescription;
	std::string longTitle;

	for (int i = 0; i != MAX_TITLE_LEN + 1; i++) {
		std::string charr = std::to_string(i % 10);
		longTitle += charr;
	}

	for (int i = 0; i != MAX_DESCRIPTION_LEN + 1; i++) {
		std::string charr = std::to_string(i % 10);
		longDescription += charr;
	}
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2030, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/" + longTitle.substr(0, MAX_TITLE_LEN) + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);

	std::string _title = "Title: " + longTitle.substr(0,MAX_TITLE_LEN);
	std::string _description = "Description: " + longDescription.substr(0,MAX_DESCRIPTION_LEN);
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(longTitle, longDescription, dtMock, fwMock);

	EXPECT_NE(longTitle ,entry->getTitle());
	EXPECT_NE(longDescription, entry->getDescription());
	EXPECT_TRUE(entry->getTitle() == longTitle.substr(0, MAX_TITLE_LEN));
	EXPECT_TRUE(entry->getDescription() == longDescription.substr(0, MAX_DESCRIPTION_LEN));

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
}

TEST(ReminderEntryTests_Constructor, InvalidValues_ForExecDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	
	try {
		DateTime* execDate = new DateTime(19, 5, 2022, 12, 30, 30);
		ReminderEntry* entry;

		entry = new ReminderEntry(execDate, "title", "description", dtMock);

		FAIL() << "Expected: Invalid execution date.Must be in the future";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERR_MSG);
	}

	try {
		DateTime* execDate = new DateTime(20, 5, 2022, 11, 30, 30);
		ReminderEntry* entry;

		entry = new ReminderEntry(execDate, "title", "description", dtMock);

		FAIL() << "Expected: Invalid execution date.Must be in the future";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERR_MSG);
	}

	try {
		DateTime* execDate = new DateTime(20, 5, 2022, 10, 29, 30);
		ReminderEntry* entry;

		entry = new ReminderEntry(execDate, "title", "description", dtMock);

		FAIL() << "Expected: Invalid execution date.Must be in the future";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERR_MSG);
	}

	try {
		DateTime* execDate = new DateTime(20, 5, 2022, 10, 30, 29);
		ReminderEntry* entry;

		entry = new ReminderEntry(execDate, "title", "description", dtMock);

		FAIL() << "Expected: Invalid execution date.Must be in the future";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERR_MSG);
	}

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Setter, InvalidValues_ForExecDate) {
	try {
		DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
		std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime* dt = new DateTime(dtMock);
		DateTime* execDate = new DateTime(19, 5, 2022, 12, 30, 30);

		ReminderEntry* entry;
		FileWorkerMock* fwMock = new FileWorkerMock();
		HelperClass helper;


		std::string _path = "out/_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) +
			+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
		std::string _title = "Title: ";
		std::string _description = "Description: ";
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
		std::string _status = "Status: NOT FINISHED";
		std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

		EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);


		entry = new ReminderEntry(dtMock, fwMock);

		Mock::AllowLeak(dtMock);
		Mock::VerifyAndClear(dtMock);
		Mock::AllowLeak(fwMock);
		Mock::VerifyAndClear(fwMock);

		entry->setExecutionDate(*execDate);


		FAIL() << "Expected: Invalid execution date.Must be in the future";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, ERR_MSG);
	}
}

TEST(ReminderEntryTests_Setter, CorrectValues_ForExecDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(26, 5, 2022, 12, 30, 30);

	ReminderEntry* entry;
	FileWorkerMock* fwMock = new FileWorkerMock();
	HelperClass helper;


	std::string _path = "out/_" + helper.getPath(date[0],date[1],date[2],date[3],date[4],date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(dtMock, fwMock);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	EXPECT_CALL(*fwMock, deleteFile(_path)).WillOnce(Return(true));
	std::string newPath = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26,5, 2022, 12, 30, 30);
	std::string newOutput = _title + "\n" + _description + "\n" + _dateCreated + "\n" + "Execution date: " + execDate->getFormat(true) + "\n" + _status;
	EXPECT_CALL(*fwMock, writeInFile(newPath, newOutput)).Times(1);

	entry->setExecutionDate(*execDate);

	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_EQ(entry->getFilePath(), newPath);

	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Setter, CorrectValues_ForTitleAndDescription_Boundary) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	FileWorkerMock* fwMock1 = new FileWorkerMock();

	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	HelperClass helper;


	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;


	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(dtMock, fwMock1);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	std::string title = "";
	std::string description = "";
	for (int i = 0; i < MAX_TITLE_LEN; i++) {
		title += "a";
	}
	for (int i = 0; i < MAX_DESCRIPTION_LEN; i++) {
		description += "a";
	}
	std::string newPath = "out/" + title + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(0, 0, 0, 0, 0, 0);
	std::string newOutput_title = _title + title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;
	std::string newOutput_description = _title + title + "\n" + _description + description +"\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);

	FileWorkerMock* fwMock2 = new FileWorkerMock();
	
	EXPECT_CALL(*fwMock2, deleteFile(_path)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock2, writeInFile(newPath, newOutput_title)).Times(1);

	entry->fileWorker = fwMock2;
	entry->setTitle(title);

	EXPECT_EQ(title, entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == newPath);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);

	FileWorkerMock* fwMock3 = new FileWorkerMock();
	
	EXPECT_CALL(*fwMock3, deleteFile(newPath)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock3, writeInFile(newPath, newOutput_description)).Times(1);

	entry->fileWorker = fwMock3;
	entry->setDescription(description);

	EXPECT_EQ(description, entry->getDescription());
	EXPECT_EQ(title, entry->getTitle());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == newPath);

	Mock::AllowLeak(fwMock3);
	Mock::VerifyAndClear(fwMock3);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Setter, InvalidValues_ForTitleAndDescription) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	FileWorkerMock* fwMock1 = new FileWorkerMock();

	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	HelperClass helper;


	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;


	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(dtMock, fwMock1);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	std::string longTitle = "";
	std::string longDescription = "";
	for (int i = 0; i <= MAX_TITLE_LEN; i++) {
		longTitle += "a";
	}
	for (int i = 0; i <= MAX_DESCRIPTION_LEN; i++) {
		longDescription += "a";
	}
	std::string newPath = "out/" + longTitle.substr(0, MAX_TITLE_LEN) + "_" + 
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(0, 0, 0, 0, 0, 0);
	std::string newOutput_title = _title + longTitle.substr(0, MAX_TITLE_LEN) + "\n" + _description + "\n" + _dateCreated + "\n" 
		+ _executionDate + "\n" + _status;
	std::string newOutput_description = _title + longTitle.substr(0,MAX_TITLE_LEN) + "\n" 
		+ _description + longDescription.substr(0,MAX_DESCRIPTION_LEN) + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);

	FileWorkerMock* fwMock2 = new FileWorkerMock();

	EXPECT_CALL(*fwMock2, deleteFile(_path)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock2, writeInFile(newPath, newOutput_title)).Times(1);

	entry->fileWorker = fwMock2;
	entry->setTitle(longTitle);

	EXPECT_NE(longTitle, entry->getTitle());
	EXPECT_EQ(longTitle.substr(0, MAX_TITLE_LEN), entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == newPath);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);

	FileWorkerMock* fwMock3 = new FileWorkerMock();

	EXPECT_CALL(*fwMock3, deleteFile(newPath)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock3, writeInFile(newPath, newOutput_description)).Times(1);

	entry->fileWorker = fwMock3;
	entry->setDescription(longDescription);

	EXPECT_NE(longDescription, entry->getDescription());
	EXPECT_EQ(longDescription.substr(0,MAX_DESCRIPTION_LEN), entry->getDescription());
	EXPECT_EQ(longTitle.substr(0,MAX_TITLE_LEN), entry->getTitle());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == newPath);

	Mock::AllowLeak(fwMock3);
	Mock::VerifyAndClear(fwMock3);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Setter, CorrectValues_ForStatus) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	FileWorkerMock* fwMock1 = new FileWorkerMock();

	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	HelperClass helper;

	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	ReminderEntry* entry;
	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	entry = new ReminderEntry(dtMock, fwMock1);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	

	std::string newOutput = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\nStatus: FINISHED";

	EXPECT_CALL(*fwMock1, deleteFile(_path)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock1, writeInFile(_path, newOutput)).Times(1);
	entry->setStatus(EntryStatus::FINISHED);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	
	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Copy, CopyAssignment) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* fwMock1 = new FileWorkerMock();
	ReminderEntry* entry;
	HelperClass helper;


	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26, 9, 2100, 12, 30, 0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 26/09/2100 12:30:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(new DateTime(26,9,2100,12,30), "title", "description", dtMock, fwMock1);
	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	DateTime* execDate = new DateTime(26, 9, 2100, 12, 30);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);

	FileWorkerMock* fwMock2 = new FileWorkerMock();
	std::string _path2 = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0,0,0,0,0,0);
	std::string _title2 = "Title: ";
	std::string _description2 = "Description: ";
	std::string _dateCreated2 = "Date created: " + dt->getFormat(true);

	std::string _executionDate2 = "Execution date: 00/00/0000 00:00:00";
	std::string _status2 = "Status: NOT FINISHED";
	std::string _output2 = _title2 + "\n" + _description2 + "\n" + _dateCreated2 + "\n" + _executionDate2 + "\n" + _status2;

	EXPECT_CALL(*fwMock2, writeInFile(_path2, _output2)).Times(1);

	ReminderEntry* entry2 = new ReminderEntry(dtMock, fwMock2);
	EXPECT_EQ("", entry2->getTitle());
	EXPECT_EQ("", entry2->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry2->getEntryStatus());
	EXPECT_TRUE(*entry2->getDateCreated() == *dt);
	EXPECT_TRUE(entry2->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry2->getFilePath() == _path2);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);

	entry2 = entry;
	EXPECT_EQ("title", entry2->getTitle());
	EXPECT_EQ("description", entry2->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry2->getEntryStatus());
	EXPECT_TRUE(*entry2->getDateCreated() == *dt);
	EXPECT_TRUE(*entry2->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry2->getFilePath() == _path);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_MoveAndCopy, CopyConstructor) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* fwMock1 = new FileWorkerMock();
	ReminderEntry* entry;
	HelperClass helper;


	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26, 9, 2100, 12, 30, 0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 26/09/2100 12:30:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);

	entry = new ReminderEntry(new DateTime(26, 9, 2100, 12, 30), "title", "description", dtMock, fwMock1);
	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	DateTime* execDate = new DateTime(26, 9, 2100, 12, 30);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);

	FileWorkerMock* fwMock2 = new FileWorkerMock();
	//EXPECT_CALL(*fwMock2, deleteFile(_path)).WillOnce(Return(true));
	EXPECT_CALL(*fwMock2, writeInFile(_path, _output)).Times(1);
	
	ReminderEntry* entry2 = new ReminderEntry(*entry, fwMock2);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);

	EXPECT_EQ("title", entry2->getTitle());
	EXPECT_EQ("description", entry2->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry2->getEntryStatus());
	EXPECT_TRUE(*entry2->getDateCreated() == *dt);
	execDate = new DateTime(26, 9, 2100, 12, 30);
	EXPECT_TRUE(*entry2->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry2->getFilePath() == _path);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Comparison, OperatorEquals_1) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	ReminderEntry* entry;
	FileWorkerMock* fwMock1 = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	entry = new ReminderEntry(dtMock, fwMock1);
	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);


	ReminderEntry* entry2;
	FileWorkerMock* fwMock2 = new FileWorkerMock();
	EXPECT_CALL(*fwMock2, writeInFile(_path, _output)).Times(1);
	entry2 = new ReminderEntry(dtMock, fwMock2);

	EXPECT_EQ("", entry->getTitle());
	EXPECT_EQ("", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(entry->getExecutionDate() == nullptr);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);


	EXPECT_TRUE(*entry == *entry2);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Comparison, OperatorEquals_2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(26, 9, 2100, 13, 30);

	ReminderEntry* entry;
	FileWorkerMock* fwMock1 = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26,9,2100,13,30,0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	entry = new ReminderEntry(execDate, "title", "description", dtMock, fwMock1);
	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);


	ReminderEntry* entry2;
	FileWorkerMock* fwMock2 = new FileWorkerMock();
	EXPECT_CALL(*fwMock2, writeInFile(_path, _output)).Times(1);
	entry2 = new ReminderEntry(execDate, "title", "description", dtMock, fwMock2);

	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);


	EXPECT_TRUE(*entry == *entry2);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Comparison, OperatorEquals_3) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(26, 9, 2100, 13, 30);

	//ReminderEntry* entry;
	FileWorkerMock* fwMock1 = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26, 9, 2100, 13, 30, 0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	ReminderEntry entry(execDate, "title", "description", dtMock, fwMock1);
	EXPECT_EQ("title", entry.getTitle());
	EXPECT_EQ("description", entry.getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry.getEntryStatus());
	EXPECT_TRUE(*entry.getDateCreated() == *dt);
	EXPECT_TRUE(*entry.getExecutionDate() == *execDate);
	EXPECT_TRUE(entry.getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);


	FileWorkerMock* fwMock2 = new FileWorkerMock();
	EXPECT_CALL(*fwMock2, writeInFile(_path, _output)).Times(1);
	ReminderEntry entry2(execDate, "title", "description", dtMock, fwMock2);

	EXPECT_EQ("title", entry.getTitle());
	EXPECT_EQ("description", entry.getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry.getEntryStatus());
	EXPECT_TRUE(*entry.getDateCreated() == *dt);
	EXPECT_TRUE(*entry.getExecutionDate() == *execDate);
	EXPECT_TRUE(entry.getFilePath() == _path);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);


	EXPECT_TRUE(entry == entry2);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Comparison, OperatorEquals_4) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* fwMock1 = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _title = "Title: ";
	std::string _description = "Description: ";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: 00/00/0000 00:00:00";
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	ReminderEntry entry(dtMock, fwMock1);
	EXPECT_EQ("", entry.getTitle());
	EXPECT_EQ("", entry.getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry.getEntryStatus());
	EXPECT_TRUE(*entry.getDateCreated() == *dt);
	EXPECT_TRUE(entry.getExecutionDate() == nullptr);
	EXPECT_TRUE(entry.getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);


	FileWorkerMock* fwMock2 = new FileWorkerMock();
	EXPECT_CALL(*fwMock2, writeInFile(_path, _output)).Times(1);
	ReminderEntry entry2(dtMock, fwMock2);

	EXPECT_EQ("", entry.getTitle());
	EXPECT_EQ("", entry.getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry.getEntryStatus());
	EXPECT_TRUE(*entry.getDateCreated() == *dt);
	EXPECT_TRUE(entry.getExecutionDate() == nullptr);
	EXPECT_TRUE(entry.getFilePath() == _path);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);


	EXPECT_TRUE(entry == entry2);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderEntryTests_Comparison, OperatorEquals_5) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 20, 5 , 2022, 12, 30, 30 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(26, 9, 2100, 13, 30);

	ReminderEntry* entry;
	FileWorkerMock* fwMock1 = new FileWorkerMock();
	HelperClass helper;

	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26, 9, 2100, 13, 30, 0);
	std::string _title = "Title: title";
	std::string _description = "Description: description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);

	std::string _executionDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _executionDate + "\n" + _status;

	EXPECT_CALL(*fwMock1, writeInFile(_path, _output)).Times(1);
	entry = new ReminderEntry(execDate, "title", "description", dtMock, fwMock1);
	EXPECT_EQ("title", entry->getTitle());
	EXPECT_EQ("description", entry->getDescription());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock1);
	Mock::VerifyAndClear(fwMock1);


	ReminderEntry* entry2;
	FileWorkerMock* fwMock2 = new FileWorkerMock();

	std::string _path2 = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) +
		+"_" + helper.getPath(26, 9, 2100, 13, 30, 0);
	std::string _title2 = "Title: title";
	std::string _description2 = "Description: description";
	std::string _dateCreated2 = "Date created: " + dt->getFormat(true);

	std::string _executionDate2 = "Execution date: " + execDate->getFormat(true);
	std::string _status2 = "Status: FINISHED";
	std::string _output2 = _title2 + "\n" + _description2 + "\n" + _dateCreated2 + "\n" + _executionDate2 + "\n" + _status2;

	std::vector<std::string> _outputInLines;
	_outputInLines.push_back(_title2); _outputInLines.push_back(_description2); _outputInLines.push_back(_dateCreated2);
	_outputInLines.push_back(_executionDate2); _outputInLines.push_back(_status2);

	//EXPECT_CALL(*fwMock2, writeInFile(_path2, _output2)).Times(1);
	entry2 = new ReminderEntry(_outputInLines, dtMock, fwMock2);

	EXPECT_EQ("title", entry2->getTitle());
	EXPECT_EQ("description", entry2->getDescription());
	EXPECT_EQ(EntryStatus::FINISHED, entry2->getEntryStatus());
	EXPECT_TRUE(*entry->getDateCreated() == *dt);
	EXPECT_TRUE(*entry->getExecutionDate() == *execDate);
	EXPECT_TRUE(entry->getFilePath() == _path);

	Mock::AllowLeak(fwMock2);
	Mock::VerifyAndClear(fwMock2);


	EXPECT_FALSE(*entry == *entry2);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}
