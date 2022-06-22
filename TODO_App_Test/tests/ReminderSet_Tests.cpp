#include "../pch.h"
#include "../../TODO_App/include/ReminderSet.h"
#include "../mock/FileWorkerMock.h"
#include "../src/HelperClass.h"
#include "../mock/DateTimeWorkerMock.h"
#include <chrono>
#include <thread>
#include <algorithm>
#include <random>


using namespace ::testing;
using namespace std::chrono_literals;


TEST(ReminderSet_Tests, ConstructorWithMock) {
	HelperClass helper;
	FileWorkerMock* mock = new FileWorkerMock();
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,9,2030, 12,30,50 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(20, 5, 2100, 12, 30);
	std::vector<std::string> filePaths;
	std::vector<std::vector<std::string>> fileReadings;

	int i = 1;
	std::string finished = "FINISHED";
	std::string not_finished = "NOT FINISHED";

	for (; i < 5; i++) {
		execDate->setDay(execDate->getDay() + 1);
		execDate->setMonth(execDate->getMonth() + 1);
		execDate->setHours(execDate->getHours() + 1);

		std::vector<std::string> fileReading;
		fileReading.push_back("Title: title_" + std::to_string(i));
		fileReading.push_back("Description: random description " + std::to_string(i));
		fileReading.push_back("Date created: " + dt->getFormat(true));
		fileReading.push_back("Execution date: " + execDate->getFormat(true));
		
		if (i % 2 == 0) {
			fileReading.push_back("Status: " + finished);
		}
		else {
			fileReading.push_back("Status: " + not_finished);
		}
		
		std::string filePath = "out/title_" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		filePaths.push_back(filePath);
		fileReadings.push_back(fileReading);

		std::string _output = "";
		for (std::string x : fileReading) {
			_output += x + "\n";
		}
		_output.erase(_output.size() - 1, 1);


		EXPECT_CALL(*mock, fileExists(filePath)).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(filePath)).WillOnce(Return(fileReading));
		//EXPECT_CALL(*mock, writeInFile(filePath, _output)).Times(1);
	}
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(filePaths));
	execDate->setDay(execDate->getDay() - 4);
	execDate->setMonth(execDate->getMonth() - 4);
	execDate->setHours(execDate->getHours() - 4);

	ReminderSet set(mock);

	std::map<int, ReminderEntry*> entries = set.getAll();
	std::map<int, ReminderEntry*>::iterator it;

	i = 1;
	for (; i < 5; i++){
		it = entries.find(i);
		EXPECT_EQ(it->second->getTitle(), "title_" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "random description " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dt);

		execDate->setDay(execDate->getDay() + 1);
		execDate->setMonth(execDate->getMonth() + 1);
		execDate->setHours(execDate->getHours() + 1);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
		if (i % 2 == 0) {
			EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);
		}
		else {
			EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);
		}
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, MakeNewEntryCase) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	DateTime* dt;
	std::vector<int> date{ 24,5,2022, 14,5,0 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2022, 13, 0);

	std::string path1 = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(0, 0, 0, 0, 0, 0);
	std::string output1 = "Title: \nDescription: \nDate created: 24/05/2022 14:05:00\nExecution date: 00/00/0000 00:00:00\nStatus: NOT FINISHED";
	std::string path2 = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(0, 0, 0, 0, 0, 0);
	std::string output2 = "Title: title\nDescription: description\nDate created: 24/05/2022 14:05:00\nExecution date: 00/00/0000 00:00:00\nStatus: NOT FINISHED";
	std::string path3 = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26,9,2022,13,0,0);
	std::string output3 = "Title: title\nDescription: description\nDate created: 24/05/2022 14:05:00\nExecution date: 26/09/2022 13:00:00\nStatus: NOT FINISHED";
	
	FileWorkerMock* entryMock1 = new FileWorkerMock();
	EXPECT_CALL(*entryMock1, writeInFile(path1, output1)).Times(1);
	ReminderEntry* entry1 = new ReminderEntry(dtMock, entryMock1);
	EXPECT_EQ("", entry1->getTitle());
	EXPECT_EQ("", entry1->getDescription());
	EXPECT_TRUE(DateTime(24,5,2022,14,5) == *entry1->getDateCreated());
	EXPECT_TRUE(entry1->getExecutionDate() == nullptr);
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry1->getEntryStatus());
	std::string expected_path = "out/_24052022_140500_00000000_000000";
	EXPECT_EQ(expected_path, entry1->getFilePath());
	Mock::AllowLeak(entryMock1);
	Mock::VerifyAndClear(entryMock1);


	FileWorkerMock* entryMock2 = new FileWorkerMock();
	EXPECT_CALL(*entryMock2, writeInFile(path2, output2)).Times(1);
	ReminderEntry* entry2 = new ReminderEntry("title", "description", dtMock, entryMock2);
	EXPECT_EQ("title", entry2->getTitle());
	EXPECT_EQ("description", entry2->getDescription());
	EXPECT_TRUE(DateTime(24, 5, 2022, 14, 5) == *entry2->getDateCreated());
	EXPECT_TRUE(entry2->getExecutionDate() == nullptr);
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry2->getEntryStatus());
	expected_path = "out/title_24052022_140500_00000000_000000";
	EXPECT_EQ(expected_path, entry2->getFilePath());
	Mock::AllowLeak(entryMock2);
	Mock::VerifyAndClear(entryMock2);

	FileWorkerMock* entryMock3 = new FileWorkerMock();
	EXPECT_CALL(*entryMock3, writeInFile(path3, output3)).Times(1);
	ReminderEntry* entry3 = new ReminderEntry(execDate, "title", "description", dtMock, entryMock3);
	EXPECT_EQ("title", entry3->getTitle());
	EXPECT_EQ("description", entry3->getDescription());
	EXPECT_TRUE(DateTime(24, 5, 2022, 14, 5) == *entry3->getDateCreated());
	EXPECT_TRUE(*entry2->getExecutionDate() == DateTime(26,9,2022,13,0));
	EXPECT_EQ(EntryStatus::NOT_FINISHED, entry3->getEntryStatus());
	expected_path = "out/title_24052022_140500_26092022_130000";
	EXPECT_EQ(expected_path, entry3->getFilePath());
	Mock::AllowLeak(entryMock3);
	Mock::VerifyAndClear(entryMock3);
	

	FileWorkerMock* setMock = new FileWorkerMock();
	
	EXPECT_CALL(*setMock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));
	EXPECT_CALL(*setMock, fileExists(path1)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, fileExists(path2)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, fileExists(path3)).WillOnce(Return(true));

	ReminderSet set(setMock);
	EXPECT_EQ(0, set.getAll().size());

	set.makeNewEntry(entry1);
	set.makeNewEntry(entry2);
	set.makeNewEntry(entry3);

	EXPECT_EQ(3, set.getAll().size());

	std::map<int, ReminderEntry*> m = set.getAll();
	std::map<int, ReminderEntry*>::iterator set_entry1 = m.find(1);
	EXPECT_EQ(set_entry1->second->getTitle(), "");
	EXPECT_EQ(set_entry1->second->getDescription(), "");
	EXPECT_TRUE(*set_entry1->second->getDateCreated() == *dt);
	EXPECT_TRUE(set_entry1->second->getExecutionDate() == nullptr);
	EXPECT_EQ(set_entry1->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(set_entry1->second->getFilePath(), path1);


	std::map<int, ReminderEntry*>::iterator set_entry2 = m.find(2);
	EXPECT_EQ(set_entry2->second->getTitle(), "title");
	EXPECT_EQ(set_entry2->second->getDescription(), "description");
	EXPECT_TRUE(*set_entry2->second->getDateCreated() == *dt);
	EXPECT_TRUE(set_entry2->second->getExecutionDate() == nullptr);
	EXPECT_EQ(set_entry2->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(set_entry2->second->getFilePath(), path2);


	std::map<int, ReminderEntry*>::iterator set_entry3 = m.find(3);
	EXPECT_EQ(set_entry3->second->getTitle(), "title");
	EXPECT_EQ(set_entry3->second->getDescription(), "description");
	EXPECT_TRUE(*set_entry3->second->getDateCreated() == *dt);
	EXPECT_TRUE(*set_entry3->second->getExecutionDate() == *execDate);
	EXPECT_EQ(set_entry3->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
	EXPECT_EQ(set_entry3->second->getFilePath(), path3);

	Mock::AllowLeak(setMock);
	Mock::VerifyAndClear(setMock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, DeleteEntry_ValidIndex) {
	HelperClass helper;
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	DateTime* dt;
	std::vector<int> date{ 25,5,2022,10,30,43 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);
	
	FileWorkerMock* entryMock = new FileWorkerMock();
	FileWorkerMock* fwMock = new FileWorkerMock();

	std::string _path = "out/title_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_"
		+ helper.getPath(0, 0, 0, 0, 0, 0);
	std::vector<std::string> outputInLines;
	outputInLines.push_back("Title: title");
	outputInLines.push_back("Description: description");
	outputInLines.push_back("Date created: 25/05/2022 10:30:43");
	outputInLines.push_back("Execution date: 00/00/0000 00:00:00");
	outputInLines.push_back("Status: NOT FINISHED");

	std::string _output = "Title: title\nDescription: description\nDate created: " + dt->getFormat(true) + "\nExecution date: 00/00/0000 00:00:00\nStatus: NOT FINISHED";

	EXPECT_CALL(*entryMock, writeInFile(_path, _output)).Times(1);

	ReminderEntry* e = new ReminderEntry("title", "description", dtMock, entryMock);
	Mock::AllowLeak(entryMock);
	Mock::VerifyAndClear(entryMock);

	std::vector<std::string> filePaths;
	filePaths.push_back(e->getFilePath());

	EXPECT_CALL(*fwMock, getAllFromDirectory("out/")).WillOnce(Return(filePaths));
	EXPECT_CALL(*fwMock, fileExists(e->getFilePath())).WillOnce(Return(true));
	EXPECT_CALL(*fwMock, readFromFileInLines(e->getFilePath())).WillOnce(Return(outputInLines));
	EXPECT_CALL(*fwMock, deleteFile(filePaths[0])).WillOnce(Return(true));

	ReminderSet set(fwMock);

	EXPECT_EQ(set.getAll().size(), 1);
	set.deleteEntry(1);

	EXPECT_EQ(set.getAll().size(), 0);

	Mock::AllowLeak(fwMock);
	Mock::VerifyAndClear(fwMock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, DeleteEntry_InvalidIndex) {
	HelperClass helper;
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022, 10,1,43 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* entryMock1 = new FileWorkerMock();
	FileWorkerMock* entryMock2 = new FileWorkerMock();
	std::string _path1 = "out/entry1_" + helper.getPath(26, 5, 2022, 10, 1, 43) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _path2 = "out/entry2_" + helper.getPath(26, 5, 2022, 10, 1, 43) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	
	std::vector<std::string> _outputs1;
	_outputs1.push_back("Title: entry1");
	_outputs1.push_back("Description: description 1");
	_outputs1.push_back("Date created: " + dt->getFormat(true));
	_outputs1.push_back("Execution date: 00/00/0000 00:00:00");
	_outputs1.push_back("Status: NOT FINISHED");
	std::string _output1 = _outputs1[0] + "\n" + _outputs1[1] + "\n" + _outputs1[2] + "\n" + _outputs1[3] + "\n" + _outputs1[4];

	std::vector<std::string> _outputs2;
	_outputs2.push_back("Title: entry2");
	_outputs2.push_back("Description: description 2");
	_outputs2.push_back("Date created: " + dt->getFormat(true));
	_outputs2.push_back("Execution date: 00/00/0000 00:00:00");
	_outputs2.push_back("Status: NOT FINISHED");
	std::string _output2 = _outputs2[0] + "\n" + _outputs2[1] + "\n" + _outputs2[2] + "\n" + _outputs2[3] + "\n" + _outputs2[4];
	
	
	EXPECT_CALL(*entryMock1, writeInFile(_path1, _output1)).Times(1);
	ReminderEntry* e1 = new ReminderEntry("entry1", "description 1", dtMock, entryMock1);
	Mock::AllowLeak(entryMock1);
	Mock::VerifyAndClear(entryMock1);

	EXPECT_CALL(*entryMock2, writeInFile(_path2, _output2)).Times(1);
	ReminderEntry* e2 = new ReminderEntry("entry2", "description 2", dtMock, entryMock2);
	Mock::AllowLeak(entryMock2);
	Mock::VerifyAndClear(entryMock2);


	std::vector<std::string> filePaths;
	filePaths.push_back(_path1);
	filePaths.push_back(_path2);

	FileWorkerMock* setMock = new FileWorkerMock();
	EXPECT_CALL(*setMock, getAllFromDirectory("out/")).WillOnce(Return(filePaths));
	EXPECT_CALL(*setMock, fileExists(_path1)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, readFromFileInLines(_path1)).WillOnce(Return(_outputs1));
	EXPECT_CALL(*setMock, fileExists(_path2)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, readFromFileInLines(_path2)).WillOnce(Return(_outputs2));


	ReminderSet set(setMock);

	EXPECT_EQ(set.getAll().size(), 2);

	try {
		set.deleteEntry(3);

		FAIL() << "Expected: Invalid index";
	}
	catch (const char* err) {
		EXPECT_STREQ("Invalid index", err);
	}

	EXPECT_EQ(set.getAll().size(), 2);

	Mock::AllowLeak(setMock);
	Mock::VerifyAndClear(setMock);

	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, EditEntry_WithConcreteObject) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 25,5,2022, 10,24,32 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	std::vector<std::string> paths; //OLD PATHS
	std::vector<std::string> outputs; //OLD OUTPUTS
	std::vector<std::vector<std::string>> outputInLines;

	std::vector<std::string> new_paths; // PATHS AFTER EDITING
	std::vector<std::string> new_outputs; // OUTPUTS AFTER EDITING
	std::vector<std::vector<std::string>> newOutputInLines;

	for (int i = 1; i < 5; i++) {
		DateTime* execDate = new DateTime(5 + i, 5 + i, 2030, 12 + i, 0);


		std::string _path = "out/title_" + std::to_string(i) + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5])
			+ "_" + helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _newPath = "out/title_" + std::to_string(i) + std::to_string(i) + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5])
			+ "_" + helper.getPath(execDate->getDay(), execDate->getMonth(), 2030, execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _title = "Title: title_" + std::to_string(i);
		std::string _description = "Description: description_" + std::to_string(i);
		std::string _date_created = "Date created: " + dt->getFormat(true);
		std::string _exec_date = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		if (i % 2 == 0)
			_status += "FINISHED";
		else
			_status += "NOT FINISHED";

		std::string _output = _title + "\n" + _description + "\n" + _date_created + "\n" + _exec_date + "\n" + _status;
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_date_created); lines.push_back(_exec_date); lines.push_back(_status);

		DateTime tmp(5 + i, i + 5, 2030, 12 + i, 0);
		std::string _newTitle = "Title: title_" + std::to_string(i) + std::to_string(i);
		std::string _newDescription = "Description: description_" + std::to_string(i) + std::to_string(i);
		std::string _newDateCreated = "Date created: " + dt->getFormat(true);
		std::string _newExecDate = "Execution date: " + tmp.getFormat(true);
		std::string _newStatus = "Status: ";
		if (i % 2 == 0)
			_newStatus += "FINISHED";
		else
			_newStatus += "NOT FINISHED";

		std::string _newOutput = _newTitle + "\n" + _newDescription + "\n" + _newDateCreated + "\n" + _newExecDate + "\n" + _newStatus;
		std::vector<std::string> new_lines;
		new_lines.push_back(_newTitle); new_lines.push_back(_newDescription); new_lines.push_back(_newDateCreated);
		new_lines.push_back(_newExecDate); new_lines.push_back(_newStatus);

		paths.push_back(_path);
		outputs.push_back(_output);
		new_paths.push_back(_newPath);
		new_outputs.push_back(_newOutput);
		outputInLines.push_back(lines);
		newOutputInLines.push_back(new_lines);
	}
	
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	for (int i = 0; i < 4; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}
	

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	std::map<int, ReminderEntry*>::iterator it;
	int i = 1;
	
	for (; i < 5; i++) {
		EXPECT_CALL(*mock, deleteFile(paths[i-1])).WillOnce(Return(true));
		EXPECT_CALL(*mock, writeInFile(new_paths[i-1], new_outputs[i-1])).Times(1);
	}
	
	i = 1;
	for (; i < 5; i++) {
		it = map.find(i);
		set.editEntry(it->second, new ReminderEntry(newOutputInLines[i-1], dtMock, mock));
	}
	
	map = set.getAll();
	i = 1;
	
	for (; i<5; i++) {
		it = map.find(i);

		EXPECT_EQ("title_" + std::to_string(i) + std::to_string(i), it->second->getTitle());
		EXPECT_EQ("description_" + std::to_string(i) + std::to_string(i), it->second->getDescription());
		EXPECT_TRUE(*dt == *it->second->getDateCreated());
		DateTime* tmp = new DateTime(5 + i, i + 5, 2030, 12 + i, 0);
		EXPECT_TRUE(*tmp == *it->second->getExecutionDate());
		if (i % 2 == 0)
			EXPECT_TRUE(EntryStatus::FINISHED == it->second->getEntryStatus());
		else
			EXPECT_TRUE(EntryStatus::NOT_FINISHED == it->second->getEntryStatus());
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, EditEntry_WithNullObject){
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 25,5,2022, 10,24,32 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	std::vector<std::string> paths; //OLD PATHS
	std::vector<std::string> outputs; //OLD OUTPUTS
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i < 5; i++) {
		DateTime* execDate = new DateTime(5 + i, 5 + i, 2030, 12 + i, 0);


		std::string _path = "out/title_" + std::to_string(i) + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5])
			+ "_" + helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());


		std::string _title = "Title: title_" + std::to_string(i);
		std::string _description = "Description: description_" + std::to_string(i);
		std::string _date_created = "Date created: " + dt->getFormat(true);
		std::string _exec_date = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		if (i % 2 == 0)
			_status += "FINISHED";
		else
			_status += "NOT FINISHED";

		std::string _output = _title + "\n" + _description + "\n" + _date_created + "\n" + _exec_date + "\n" + _status;
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_date_created); lines.push_back(_exec_date); lines.push_back(_status);

		paths.push_back(_path);
		outputs.push_back(_output);
		outputInLines.push_back(lines);
	}


	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 4; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map1 = set.getAll();
	std::map<int, ReminderEntry*>::iterator it1;
	int i = 1;

	for (; i < 5; i++) {
		it1 = map1.find(i);
		set.editEntry(it1->second,nullptr);
	}

	std::map<int, ReminderEntry*> map2 = set.getAll();
	std::map<int, ReminderEntry*>::iterator it2;


	i = 1;
	EXPECT_TRUE(map1.size() == map2.size());
	for (it1 = map1.begin(), it2 = map2.begin(); it1 != map1.end() && it2 != map2.end(); it1++, it2++) {
		EXPECT_TRUE(*it1->second == *it2 ->second);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, EditEntry_WithValidIndex) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 25,5,2022,12,50,13 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	std::vector<std::string> paths; //OLD PATHS
	std::vector<std::string> outputs; //OLD OUTPUTS
	std::vector<std::vector<std::string>> outputInLines;

	std::vector<std::string> new_paths; // PATHS AFTER EDITING
	std::vector<std::string> new_outputs; // OUTPUTS AFTER EDITING
	std::vector<std::vector<std::string>> newOutputInLines;

	for (int i = 1; i < 5; i++) {
		DateTime* execDate = new DateTime(5 + i, 5 + i, 2030, 12 + i, 0);

		std::string _path = "out/title_" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _newPath = "out/title_" + std::to_string(i) + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), 2030, execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _title = "Title: title_" + std::to_string(i);
		std::string _description = "Description: description_" + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		std::string _output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;


		DateTime tmp(5 + i, i + 5, 2030, 12 + i, 0);
		std::string _newTitle = "Title: title_" + std::to_string(i) + std::to_string(i);
		std::string _newDescription = "Description: description_" + std::to_string(i) + std::to_string(i);
		std::string _newDateCreated = "Date created: " + dt->getFormat(true);
		std::string _newExecDate = "Execution date: " + tmp.getFormat(true);
		std::string _newStatus = "Status: ";
		if (i % 2 == 0) {
			_newStatus += "FINISHED";
		}
		else {
			_newStatus += "NOT FINISHED";
		}
		std::string _newOutput = _newTitle + "\n" + _newDescription + "\n" + _newDateCreated + "\n" + _newExecDate + "\n" + _status;
		std::vector<std::string> _newLines;
		_newLines.push_back(_newTitle); _newLines.push_back(_newDescription); _newLines.push_back(_newDateCreated);
		_newLines.push_back(_newExecDate); _newLines.push_back(_newStatus);


		paths.push_back(_path);
		outputs.push_back(_output);
		outputInLines.push_back(lines);
		new_paths.push_back(_newPath);
		new_outputs.push_back(_newOutput);
		newOutputInLines.push_back(_newLines);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 4; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	std::map<int, ReminderEntry*>::iterator it;

	for (int i =1; i < 5; i++) {
		EXPECT_CALL(*mock, deleteFile(paths[i-1])).WillOnce(Return(true));
		EXPECT_CALL(*mock, writeInFile(new_paths[i-1], new_outputs[i-1])).Times(2);
		
		it = map.find(i);
		set.editEntry(i, new ReminderEntry(newOutputInLines[i-1], mock));


		EXPECT_EQ(it->second->getTitle(), "title_" + std::to_string(i) + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "description_" + std::to_string(i) + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dt);
		DateTime* tmp = new DateTime(5 + i, i + 5, 2030, 12 + i, 0);
		EXPECT_TRUE(*it->second->getExecutionDate() == *tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(EntryStatus::FINISHED, it->second->getEntryStatus());
		}
		else {
			EXPECT_EQ(EntryStatus::NOT_FINISHED, it->second->getEntryStatus());
		}

	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, EditEntry_WithInvalidIndex) {
	HelperClass helper;
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022, 10,1,43 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* entryMock1 = new FileWorkerMock();
	FileWorkerMock* entryMock2 = new FileWorkerMock();
	std::string _path1 = "out/entry1_" + helper.getPath(26, 5, 2022, 10, 1, 43) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string _path2 = "out/entry2_" + helper.getPath(26, 5, 2022, 10, 1, 43) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);

	std::vector<std::string> _outputs1;
	_outputs1.push_back("Title: entry1");
	_outputs1.push_back("Description: description 1");
	_outputs1.push_back("Date created: " + dt->getFormat(true));
	_outputs1.push_back("Execution date: 00/00/0000 00:00:00");
	_outputs1.push_back("Status: NOT FINISHED");
	std::string _output1 = _outputs1[0] + "\n" + _outputs1[1] + "\n" + _outputs1[2] + "\n" + _outputs1[3] + "\n" + _outputs1[4];

	std::vector<std::string> _outputs2;
	_outputs2.push_back("Title: entry2");
	_outputs2.push_back("Description: description 2");
	_outputs2.push_back("Date created: " + dt->getFormat(true));
	_outputs2.push_back("Execution date: 00/00/0000 00:00:00");
	_outputs2.push_back("Status: NOT FINISHED");
	std::string _output2 = _outputs2[0] + "\n" + _outputs2[1] + "\n" + _outputs2[2] + "\n" + _outputs2[3] + "\n" + _outputs2[4];


	EXPECT_CALL(*entryMock1, writeInFile(_path1, _output1)).Times(1);
	ReminderEntry* e1 = new ReminderEntry("entry1", "description 1", dtMock, entryMock1);
	Mock::AllowLeak(entryMock1);
	Mock::VerifyAndClear(entryMock1);

	EXPECT_CALL(*entryMock2, writeInFile(_path2, _output2)).Times(1);
	ReminderEntry* e2 = new ReminderEntry("entry2", "description 2", dtMock, entryMock2);
	Mock::AllowLeak(entryMock2);
	Mock::VerifyAndClear(entryMock2);


	std::vector<std::string> filePaths;
	filePaths.push_back(_path1);
	filePaths.push_back(_path2);

	FileWorkerMock* setMock = new FileWorkerMock();
	EXPECT_CALL(*setMock, getAllFromDirectory("out/")).WillOnce(Return(filePaths));
	EXPECT_CALL(*setMock, fileExists(_path1)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, readFromFileInLines(_path1)).WillOnce(Return(_outputs1));
	EXPECT_CALL(*setMock, fileExists(_path2)).WillOnce(Return(true));
	EXPECT_CALL(*setMock, readFromFileInLines(_path2)).WillOnce(Return(_outputs2));


	ReminderSet set(setMock);

	try {
		set.deleteEntry(0);
		FAIL() << "Expected: Invalid index";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Invalid index");
	}
	try {
		set.deleteEntry(3);
		FAIL() << "Expected: Invalid index";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Invalid index");
	}

	Mock::AllowLeak(setMock);
	Mock::VerifyAndClear(setMock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByDateCreated) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::pair<std::string, std::string>> paths_and_outputs;

	for (int i = 1; i <= 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);
		std::vector<int> date{ 26,5,2022, 5 + i , 5 + i,5  + i};
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(dtMock);
		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" + 
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;
		
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated); 
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 10);

	int i = 1;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	
	i = 10;
	std::map<int, ReminderEntry*> sortedByDateCreated_descending = set.sortByDateCreated(true);

	for (it = sortedByDateCreated_descending.begin(); it != sortedByDateCreated_descending.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 1;
	std::map<int, ReminderEntry*> sortedByDateCreated_ascending = set.sortByDateCreated(false);
	for (it = sortedByDateCreated_ascending.begin(); it != sortedByDateCreated_ascending.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}
	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByDateCreated_WithOneEntry) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54};
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12,0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths; 
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> sortedByDateCreated_descending = set.sortByDateCreated(true);
	EXPECT_TRUE(sortedByDateCreated_descending.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = sortedByDateCreated_descending.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	std::map<int, ReminderEntry*> sortedByDateCreated_ascending = set.sortByDateCreated(false);
	EXPECT_TRUE(sortedByDateCreated_descending.size() == 1);
	it = sortedByDateCreated_ascending.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByDateCreated_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> sortedByDateCreated_descending = set.sortByDateCreated(true);
	EXPECT_TRUE(sortedByDateCreated_descending.size() == 0);

	std::map<int, ReminderEntry*> sortedByDateCreated_ascending = set.sortByDateCreated(true);
	EXPECT_TRUE(sortedByDateCreated_descending.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, SortByExecDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022, 10 , 44, 32 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i <= 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);

		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 10);

	int i = 1;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}


	i = 10;
	std::map<int, ReminderEntry*> sortedByExecDate_descending = set.sortByExecDate(true);

	for (it = sortedByExecDate_descending.begin(); it != sortedByExecDate_descending.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 1;
	std::map<int, ReminderEntry*> sortedByExecDate_ascending = set.sortByExecDate(false);
	for (it = sortedByExecDate_ascending.begin(); it != sortedByExecDate_ascending.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}
	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByExecDate_WithOneEntry) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths;
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> sortedByExecDate_descending = set.sortByExecDate(true);
	EXPECT_TRUE(sortedByExecDate_descending.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = sortedByExecDate_descending.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	std::map<int, ReminderEntry*> sortedByExecDate_ascending = set.sortByExecDate(false);
	EXPECT_TRUE(sortedByExecDate_ascending.size() == 1);
	it = sortedByExecDate_ascending.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByExecDate_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> sortedByExecDate_descending = set.sortByExecDate(true);
	EXPECT_TRUE(sortedByExecDate_descending.size() == 0);

	std::map<int, ReminderEntry*> sortedByExecDate_ascending = set.sortByExecDate(true);
	EXPECT_TRUE(sortedByExecDate_ascending.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, SortByStatus) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i <= 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);
		std::vector<int> date{ 26,5,2022, 5 + i , 5 + i,5 + i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(dtMock);
		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 10);

	int i = 1;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	
	i = 1; int j = 5;
	std::map<int, ReminderEntry*> sortedByStatus_finishedFirst = set.sortByStatus(true);
	
	
	for (it = sortedByStatus_finishedFirst.begin(); it != sortedByStatus_finishedFirst.end(); it++) {

		if (i < 6) {
			DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + 2 * i, 5 + 2 * i, 5 + 2 * i);
			DateTime* exec_tmp = new DateTime(4 * i, 2 * i, 2030, 5 + 2 * i, 0);
			EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(2*i));
			EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(2*i));
			EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
			EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
			
		}
		else {
			DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + (i - j), 5 + (i - j), 5 + (i - j));
			DateTime* exec_tmp = new DateTime(2 * (i - j), i - j, 2030, 5 + (i - j), 0);
			EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i - j));
			EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i - j));
			EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
			EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
			j--;
		}

		i++; 
	}

	
	i = 1; j = 4;
	int k = 0;
	std::map<int, ReminderEntry*> sortedByStatus_notfinishedFirst = set.sortByStatus(false);
	for (it = sortedByStatus_notfinishedFirst.begin(); it != sortedByStatus_notfinishedFirst.end(); it++) {
		if (i < 6) {
			DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + (i+ k), 5 + i+k, 5 + i + k);
			DateTime* exec_tmp = new DateTime(2 * (i+k), i+k, 2030, 5 + i + k, 0);
			EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i + k));
			EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i + k));
			EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
			EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
			k++;
		}
		else {
			DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + (i - j), 5 + (i - j), 5 + (i - j));
			DateTime* exec_tmp = new DateTime(2 * (i - j), i - j, 2030, 5 + (i - j), 0);
			EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i - j));
			EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i - j));
			EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
			EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
			j--;
		}

		i++;
	}
	
	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByStatus_WithOneEntry) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54};
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12,0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths; 
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> sortedByStatus_finishedFirst = set.sortByStatus(true);
	EXPECT_TRUE(sortedByStatus_finishedFirst.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = sortedByStatus_finishedFirst.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	std::map<int, ReminderEntry*> sortedByStatus_notFinishedFirst = set.sortByStatus(false);
	EXPECT_TRUE(sortedByStatus_notFinishedFirst.size() == 1);
	it = sortedByStatus_notFinishedFirst.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByStatus_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> sortedByStatus_1 = set.sortByStatus(true);
	EXPECT_TRUE(sortedByStatus_1.size() == 0);

	std::map<int, ReminderEntry*> sortedByStatus_2 = set.sortByStatus(true);
	EXPECT_TRUE(sortedByStatus_2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, SortByTitle){
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i < 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);
		std::vector<int> date{ 26,5,2022, 5 + i , 5 + i,5 + i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(dtMock);
		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 9; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 9);

	int i = 1;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}


	i = 9;
	std::map<int, ReminderEntry*> sortedByTitle_descending = set.sortByTitle(true);


	for (it = sortedByTitle_descending.begin(); it != sortedByTitle_descending.end(); it++) {
			DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
			DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
			//std::cout << it->second->getTitle() << std::endl;
			EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
			EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
			EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
			EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
			if (i % 2 == 0) {
				EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
			}
			else {
				EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
			}
		i--;
	}


	i = 1;
	std::map<int, ReminderEntry*> sortedByTitle_ascending = set.sortByTitle(false);
	for (it = sortedByTitle_ascending.begin(); it != sortedByTitle_ascending.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}
		i++;
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByTitle_WithOneEntry) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths;
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> sortedByTitle_1 = set.sortByTitle(true);
	EXPECT_TRUE(sortedByTitle_1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = sortedByTitle_1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	std::map<int, ReminderEntry*> sortedByTitle_2 = set.sortByTitle(false);
	EXPECT_TRUE(sortedByTitle_2.size() == 1);
	it = sortedByTitle_2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByTitle_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> sorted_1 = set.sortByTitle(true);
	EXPECT_TRUE(sorted_1.size() == 0);

	std::map<int, ReminderEntry*> sorted_2 = set.sortByTitle(true);
	EXPECT_TRUE(sorted_2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, SortByDescription){
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i < 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);
		std::vector<int> date{ 26,5,2022, 5 + i , 5 + i,5 + i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(dtMock);
		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));

	for (int i = 0; i < 9; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 9);

	int i = 1;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}


	i = 9;
	std::map<int, ReminderEntry*> sortedByDescription_descending = set.sortByTitle(true);


	for (it = sortedByDescription_descending.begin(); it != sortedByDescription_descending.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}
		i--;
	}


	i = 1;
	std::map<int, ReminderEntry*> sortedByDescription_ascending = set.sortByTitle(false);
	for (it = sortedByDescription_ascending.begin(); it != sortedByDescription_ascending.end(); it++) {
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}
		i++;
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByDescription_WithOneEntry){
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths;
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> sortedByDescription_1 = set.sortByDescription(true);
	EXPECT_TRUE(sortedByDescription_1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = sortedByDescription_1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	std::map<int, ReminderEntry*> sortedByDescription_2 = set.sortByDescription(false);
	EXPECT_TRUE(sortedByDescription_2.size() == 1);
	it = sortedByDescription_2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, SortByDescription_WithZeroEntries){
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> sorted_1 = set.sortByDescription(true);
	EXPECT_TRUE(sorted_1.size() == 0);

	std::map<int, ReminderEntry*> sorted_2 = set.sortByDescription(true);
	EXPECT_TRUE(sorted_2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, FilterByStatus) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	for (int i = 1; i <= 10; i++) {
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);
		std::vector<int> date{ 26,5,2022, 5 + i , 5 + i,5 + i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(dtMock);
		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		outputInLines.push_back(lines);
		paths.push_back(_path);
		outputs.push_back(_output);
	}

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	std::map<int, ReminderEntry*>::iterator it;
	int i = 1;
	for (it = map.begin(); it != map.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + i, 5 + i, 5 + i);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);

		if (i % 2 == 0) {
			EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);
		}
		else {
			EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);
		}
		i++;
	}
	EXPECT_TRUE(map.size() == 10);

	
	std::map<int, ReminderEntry*> filteredByStatus1 = set.filterByStatus(EntryStatus::FINISHED);

	i = 1;
	for (it = filteredByStatus1.begin(); it != filteredByStatus1.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * 2*i, 2*i, 2030, 5 + 2*i, 0);
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + 2*i, 5 + 2*i, 5 + 2*i);
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(2*i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(2*i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		i++;
	}

	i = 1;
	std::map<int, ReminderEntry*> filteredByStatus0 = set.filterByStatus(EntryStatus::NOT_FINISHED);
	for (it = filteredByStatus0.begin(); it != filteredByStatus0.end(); it++) {
		DateTime* exec_tmp = new DateTime(2 * (2*i - 1), (2 * i - 1), 2030, 5 + (2 * i - 1), 0);
		DateTime* dc_tmp = new DateTime(26, 5, 2022, 5 + (2 * i - 1), 5 + (2 * i - 1), 5 + (2 * i - 1));
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string((2 * i - 1)));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string((2 * i - 1)));
		EXPECT_TRUE(*it->second->getDateCreated() == *dc_tmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *exec_tmp);
		EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		i++;
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByStatus_WithOneEntry) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths;
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	std::map<int, ReminderEntry*>::iterator it = map.begin();
	EXPECT_TRUE(map.size() == 1);
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered1 = set.filterByStatus(EntryStatus::NOT_FINISHED);
	EXPECT_TRUE(filtered1.size() == 1);
	it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);
	

	std::map<int, ReminderEntry*> filtered2 = set.filterByStatus(EntryStatus::FINISHED);
	EXPECT_TRUE(filtered2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByStatus_WithOneEntry2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 26,5,2022,10,23, 54 };
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime* dt = new DateTime(dtMock);

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;
	DateTime* execDate = new DateTime(26, 9, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(26, 9, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: DESCRIPTION";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: FINISHED";
	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> paths;
	paths.push_back(_path);
	std::vector<std::string> outputs;
	outputs.push_back(_output);
	std::vector<std::vector<std::string>> outputInLines;
	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);
	outputInLines.push_back(lines);

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(_path)).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(_path)).WillOnce(Return(outputInLines[0]));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	std::map<int, ReminderEntry*>::iterator it = map.begin();
	EXPECT_TRUE(map.size() == 1);
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);


	std::map<int, ReminderEntry*> filtered1 = set.filterByStatus(EntryStatus::FINISHED);
	EXPECT_TRUE(filtered1.size() == 1);
	it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByStatus(EntryStatus::NOT_FINISHED);
	EXPECT_TRUE(filtered2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByStatus_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> filtered1 = set.filterByStatus(EntryStatus::FINISHED);
	EXPECT_TRUE(filtered1.size() == 0);

	std::map<int, ReminderEntry*> filtered2 = set.filterByStatus(EntryStatus::NOT_FINISHED);
	EXPECT_TRUE(filtered2.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(ReminderSet_Tests, FilterByDateCreated) {
	DateTimeWorkerMock* filterDateMock = new DateTimeWorkerMock();
	std::vector<int> date{ 27, 5, 2022, 9, 56, 31 };
	EXPECT_CALL(*filterDateMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* filterDate = new DateTime(filterDateMock);

	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	std::vector<std::pair<std::string, std::string>> paths_and_outputs;

	for (int i = 1; i <= 10; i++) {
		std::vector<int> dtVector{ 27,5,2022,9, 56, 26 + i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(dtVector));
		DateTime* dt = new DateTime(dtMock);
		DateTime* execDate = new DateTime(2 * i, i, 2030, 5 + i, 0);

		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(dtVector[0], dtVector[1], dtVector[2], dtVector[3], dtVector[4], dtVector[5]) + "_" +
			helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}
		
		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);
		

		paths_and_outputs.push_back(std::pair(_path, _output));
		paths.push_back(_path);
		outputs.push_back(_output);
		outputInLines.push_back(lines);
	}

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(paths_and_outputs.begin(), paths_and_outputs.end(), g);


	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 10);


	int i;
	std::map<int, ReminderEntry*> filtered_beforeDateDescending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	std::map<int, ReminderEntry*>::iterator it;

	i = 5;
	for (it = filtered_beforeDateDescending.begin(); it != filtered_beforeDateDescending.end(); it++) {
		DateTime* dcTmp = new DateTime(27, 5, 2022, 9, 56, 26 + i);
		DateTime* execTmp = new DateTime(2 * i, i, 2030, 5 + i, 0);
		
		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 1;
	std::map<int, ReminderEntry*> filtered_beforeDateAscending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	for (it = filtered_beforeDateAscending.begin(); it != filtered_beforeDateAscending.end(); it++) {
		DateTime* dcTmp = new DateTime(27, 5, 2022, 9, 56, 26 + i);
		DateTime* execTmp = new DateTime(2 * i, i, 2030, 5 + i, 0);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	i = 10;
	std::map<int, ReminderEntry*> filtered_afterDateDescending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);

	for (it = filtered_afterDateDescending.begin(); it != filtered_afterDateDescending.end(); it++) {
		DateTime* dcTmp = new DateTime(27, 5, 2022, 9, 56, 26 + i);
		DateTime* execTmp = new DateTime(2 * i, i, 2030, 5 + i, 0);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 6;
	std::map<int, ReminderEntry*> filtered_afterDateAscending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
	for (it = filtered_afterDateAscending.begin(); it != filtered_afterDateAscending.end(); it++) {
		DateTime* dcTmp = new DateTime(27, 5, 2022, 9, 56, 26 + i);
		DateTime* execTmp = new DateTime(2 * i, i, 2030, 5 + i, 0);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(filterDateMock);
	Mock::VerifyAndClear(filterDateMock);
}

TEST(ReminderSet_Tests, FilterByDateCreated_WithOneEntry_BeforeDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 27,5,2022, 10,27,30 };
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 4, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17,4,2050,12,0, 0);

	std::string _output;// = "Title: TITLE\nDescription: This is description\nDate created: " +
		//dt.getFormat(true) + "\nExecution date: " + execDate->getFormat(true) + "\nStatus: NOT FINISHED";
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);
	
	std::map<int, ReminderEntry*> filtered4 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByDateCreated_WithOneEntry_BeforeDate_2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	
	DateTime* execDate = new DateTime(17, 4, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 4, 2050, 12, 0, 0);

	std::string _output;// = "Title: TITLE\nDescription: This is description\nDate created: " +
		//dt.getFormat(true) + "\nExecution date: " + execDate->getFormat(true) + "\nStatus: NOT FINISHED";
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, dt, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, dt, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByDateCreated(FilterMode::AfterDate, dt, true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByDateCreated(FilterMode::AfterDate, dt, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByDateCreated_WithOneEntry_AfterDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 27,5,2022, 10,27,30 };
	std::vector<int> date{ 27,5,2022, 10, 27, 40 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 4, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 4, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 0);


	std::map<int, ReminderEntry*> filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 0);

	std::map<int, ReminderEntry*> filtered3 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered3.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered4 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 1);
	it = filtered4.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByDateCreated_WithOneEntry_AfterDate_2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> date{ 27,5,2022, 10, 27, 30 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 4, 2050, 12, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 4, 2050, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, dt, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, dt, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByDateCreated(FilterMode::AfterDate, dt, true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByDateCreated(FilterMode::AfterDate, dt, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByDateCreated_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, new DateTime(), true);
	EXPECT_TRUE(filtered1.size() == 0);

	std::map<int, ReminderEntry*> filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, new DateTime(), false);
	EXPECT_TRUE(filtered2.size() == 0);

	std::map<int, ReminderEntry*> filtered3 = set.filterByDateCreated(FilterMode::AfterDate, new DateTime(), true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByDateCreated(FilterMode::AfterDate, new DateTime(), false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);

}

TEST(ReminderSet_Tests, FilterByExecDate) {
	DateTimeWorkerMock* filterDateMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 27, 5, 2022, 9, 56, 30 };
	EXPECT_CALL(*filterDateMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(filterDateMock);

	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	std::vector<std::string> paths;
	std::vector<std::string> outputs;
	std::vector<std::vector<std::string>> outputInLines;

	std::vector<std::pair<std::string, std::string>> paths_and_outputs;

	for (int i = 1; i <= 10; i++) {
		std::vector<int> dtVector{ 2 * i, i, 2022, 5 + i, 0, i };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(dtVector));
		DateTime* dt = new DateTime(dtMock);
		DateTime* execDate = new DateTime(27,5, 2023, 10,53, 25+i);

		std::string _path = "out/TITLE-" + std::to_string(i) + "_" +
			helper.getPath(dtVector[0], dtVector[1], dtVector[2], dtVector[3], dtVector[4], dtVector[5]) + "_" +
			helper.getPath(27,5,2023,10,53,25+i);

		std::string _output;
		std::string _title = "Title: TITLE-" + std::to_string(i);
		std::string _description = "Description: This is description for " + std::to_string(i);
		std::string _dateCreated = "Date created: " + dt->getFormat(true);
		std::string _execDate = "Execution date: " + execDate->getFormat(true);
		std::string _status = "Status: ";
		if (i % 2 == 0) {
			_status += "FINISHED";
		}
		else {
			_status += "NOT FINISHED";
		}

		_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;
		std::vector<std::string> lines;
		lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
		lines.push_back(_execDate); lines.push_back(_status);


		paths_and_outputs.push_back(std::pair(_path, _output));
		paths.push_back(_path);
		outputs.push_back(_output);
		outputInLines.push_back(lines);
	}

	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(paths_and_outputs.begin(), paths_and_outputs.end(), g);


	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	for (int i = 0; i < 10; i++) {
		EXPECT_CALL(*mock, fileExists(paths[i])).WillOnce(Return(true));
		EXPECT_CALL(*mock, readFromFileInLines(paths[i])).WillOnce(Return(outputInLines[i]));
	}

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 10);


	int i;
	std::map<int, ReminderEntry*> filtered_beforeDateDescending = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	std::map<int, ReminderEntry*>::iterator it;

	i = 5;
	for (it = filtered_beforeDateDescending.begin(); it != filtered_beforeDateDescending.end(); it++) {
		DateTime* dcTmp = new DateTime(2 * i, i, 2022, 5 + i, 0, i);
		DateTime* execTmp = new DateTime(27, 5, 2023, 10, 53, 25 + i);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 1;
	std::map<int, ReminderEntry*> filtered_beforeDateAscending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	for (it = filtered_beforeDateAscending.begin(); it != filtered_beforeDateAscending.end(); it++) {
		DateTime* dcTmp = new DateTime(2 * i, i, 2022, 5 + i, 0, i);
		DateTime* execTmp = new DateTime(27, 5, 2023, 10, 53, 25 + i);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	i = 10;
	std::map<int, ReminderEntry*> filtered_afterDateDescending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);

	for (it = filtered_afterDateDescending.begin(); it != filtered_afterDateDescending.end(); it++) {
		DateTime* dcTmp = new DateTime(2 * i, i, 2022, 5 + i, 0, i);
		DateTime* execTmp = new DateTime(27, 5, 2023, 10, 53, 25 + i);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i--;
	}

	i = 6;
	std::map<int, ReminderEntry*> filtered_afterDateAscending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
	for (it = filtered_afterDateAscending.begin(); it != filtered_afterDateAscending.end(); it++) {
		DateTime* dcTmp = new DateTime(2 * i, i, 2022, 5 + i, 0, i);
		DateTime* execTmp = new DateTime(27, 5, 2023, 10, 53, 25 + i);

		EXPECT_EQ(it->second->getTitle(), "TITLE-" + std::to_string(i));
		EXPECT_EQ(it->second->getDescription(), "This is description for " + std::to_string(i));
		EXPECT_TRUE(*it->second->getDateCreated() == *dcTmp);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execTmp);
		if (i % 2 == 0) {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
		}
		else {
			EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
		}

		i++;
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
	Mock::AllowLeak(filterDateMock);
	Mock::VerifyAndClear(filterDateMock);
}

TEST(ReminderSet_Tests, FilterByExecDate_WithOneEntry_BeforeDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 17,5,2023, 12,0,0 };
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17,5,2023,11,59,59);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17,5,2023,11,59,59);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByExecDate_WithOneEntry_BeforeDate_2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 17,5,2023, 12,0,0 };
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 5, 2023, 12, 0,0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 5, 2023, 12,0,0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByExecDate_WithOneEntry_AfterDate) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 17,5,2023, 12,0,0 };
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 5, 2023, 12, 0, 1);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 5, 2023, 12, 0, 1);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 0);


	std::map<int, ReminderEntry*> filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 0);

	std::map<int, ReminderEntry*> filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered3.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 1);
	it = filtered4.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByExecDate_WithOneEntry_AfterDate_2) {
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	std::vector<int> filterDateVector{ 17,5,2023, 12,0,0 };
	std::vector<int> date{ 27,5,2022, 10, 27, 29 };
	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(dtMock);
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(filterDateVector));
	DateTime* filterDate = new DateTime(dtMock);
	DateTime* execDate = new DateTime(17, 5, 2023, 12, 0, 0);
	std::string _path = "out/TITLE_" +
		helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" +
		helper.getPath(17, 5, 2023, 12, 0, 0);

	std::string _output;
	std::string _title = "Title: TITLE";
	std::string _description = "Description: This is description";
	std::string _dateCreated = "Date created: " + dt->getFormat(true);
	std::string _execDate = "Execution date: " + execDate->getFormat(true);
	std::string _status = "Status: NOT FINISHED";

	_output = _title + "\n" + _description + "\n" + _dateCreated + "\n" + _execDate + "\n" + _status;

	std::vector<std::string> lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	std::vector<std::string> paths{ _path };
	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(paths));
	EXPECT_CALL(*mock, fileExists(paths[0])).WillOnce(Return(true));
	EXPECT_CALL(*mock, readFromFileInLines(paths[0])).WillOnce(Return(lines));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	std::map<int, ReminderEntry*> filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	std::map<int, ReminderEntry*>::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	std::map<int, ReminderEntry*> filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
	Mock::AllowLeak(dtMock);
	Mock::VerifyAndClear(dtMock);
}

TEST(ReminderSet_Tests, FilterByExecDate_WithZeroEntries) {
	FileWorkerMock* mock = new FileWorkerMock();

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(std::vector<std::string>()));

	ReminderSet set(mock);
	std::map<int, ReminderEntry*> map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	std::map<int, ReminderEntry*> filtered1 = set.filterByExecDate(FilterMode::BeforeDate, new DateTime(), true);
	EXPECT_TRUE(filtered1.size() == 0);

	std::map<int, ReminderEntry*> filtered2 = set.filterByExecDate(FilterMode::BeforeDate, new DateTime(), false);
	EXPECT_TRUE(filtered2.size() == 0);

	std::map<int, ReminderEntry*> filtered3 = set.filterByExecDate(FilterMode::AfterDate, new DateTime(), true);
	EXPECT_TRUE(filtered3.size() == 0);

	std::map<int, ReminderEntry*> filtered4 = set.filterByExecDate(FilterMode::AfterDate, new DateTime(), false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}
