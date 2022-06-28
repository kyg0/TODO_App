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

class ReminderSetTest_Constructor : public Test {
protected:
	ReminderSet* set;

	FileWorkerMock* fwMock;
	DateTimeWorkerMock* dtMock;
	HelperClass helper;
	std::vector<int> date{ 1, 1, 2030, 1, 1, 1 }; // We will iterate and increment each of number in date so we would get different dates for every entry in set
														// We will also have DateTime for execDate and just like with previous one, we will iterate and increment it 
														// so we would get different execution dates for each entry in set
	
	//x<0> - path
	//x<1> - output
	//x<2> - output in lines
	std::vector<std::tuple<std::string, std::string, std::vector<std::string>>> paths_and_outputs;
	

	const std::string _title = "Title: ";
	const std::string _description = "Description: ";
	const std::string _dateCreated = "Date created: ";
	const std::string _execDate = "Execution date: ";
	const std::string _status = "Status: ";


	void SetUp() {
		fwMock = new FileWorkerMock();
		dtMock = new DateTimeWorkerMock();

		for (int i = 1; i <= 10; i++) {
			std::vector<int> tmp_DateCreated{ 2 * i + date[0] , i + date[1], date[2],
				i - date[3] , 2 * i + date[4], 2 * (i+date[5])};
			EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(tmp_DateCreated));
			DateTime* dt = new DateTime(dtMock);

			DateTime* tmp_ExecDate = new DateTime(tmp_DateCreated[0] + 1, tmp_DateCreated[1], tmp_DateCreated[2],
				tmp_DateCreated[3] + 1, tmp_DateCreated[4] + 5, tmp_DateCreated[5] + 5);

			std::string path = "out/title-" + std::to_string(i) + "_" +
				helper.getPath(tmp_DateCreated[0], tmp_DateCreated[1], tmp_DateCreated[2],
					tmp_DateCreated[3], tmp_DateCreated[4], tmp_DateCreated[5]) + "_" +
				helper.getPath(tmp_ExecDate->getDay(), tmp_ExecDate->getMonth(), tmp_ExecDate->getYear(),
					tmp_ExecDate->getHours(), tmp_ExecDate->getMinutes(), tmp_ExecDate->getSeconds());

			std::string tmp_status = "NOT FINISHED";
			if (i % 2 == 0) {
				tmp_status = "FINISHED";
			}
			std::string titleLine = this->_title + "title-" + std::to_string(i);
			std::string descriptionLine = this->_description + "description for <" + std::to_string(i) + ">";
			std::string dcLine = this->_dateCreated + dt->getFormat(true);
			std::string edLine = this->_execDate + tmp_ExecDate->getFormat(true);
			std::string statusLine = this->_status + tmp_status;


			std::string output = titleLine + "\n" + descriptionLine + "\n" + dcLine + "\n" + edLine + "\n" + statusLine;
			std::vector<std::string> lines;
			lines.push_back(titleLine); lines.push_back(descriptionLine); 
			lines.push_back(dcLine); lines.push_back(edLine); lines.push_back(statusLine);

			paths_and_outputs.push_back(std::make_tuple(path, output, lines));

			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(paths_and_outputs.begin(), paths_and_outputs.end(), g);
		}


		std::vector<std::string> tmp_paths;
		std::vector<std::string> tmp_outputs;
		std::vector<std::vector<std::string>> tmp_outputInLines;
		for (std::tuple<std::string, std::string, std::vector<std::string>> x : paths_and_outputs) {
			tmp_paths.push_back(std::get<0>(x));
			tmp_outputs.push_back(std::get<1>(x));

			std::vector<std::string> lines;
			for (std::string s : std::get<2>(x)) {
				lines.push_back(s);
			}
			tmp_outputInLines.push_back(lines);
		}

		EXPECT_CALL(*fwMock, getAllFromDirectory("out/")).WillOnce(Return(tmp_paths));
		for (int i = 0; i < 10; i++) {
			EXPECT_CALL(*fwMock, fileExists(tmp_paths[i])).WillOnce(Return(true));
			EXPECT_CALL(*fwMock, readFromFileInLines(tmp_paths[i])).WillOnce(Return(tmp_outputInLines[i]));
		}

		set = new ReminderSet(fwMock);
		
	}

	void testConstructor() {
		std::vector<std::string> paths;
		std::vector<std::string> outputs;
		std::vector<std::vector<std::string>> outputInLines;

		for (std::tuple<std::string, std::string, std::vector<std::string>> x : paths_and_outputs) {
			paths.push_back(std::get<0>(x));
			outputs.push_back(std::get<1>(x));
			outputInLines.push_back(std::get<2>(x));
		}

		std::map<int, ReminderEntry*> map = set->getAll();
		std::map<int, ReminderEntry*>::iterator it;
		
		for (int i = 0; i < map.size(); i++) {
			std::string _expectedTitle = outputInLines[i][0].substr(7, outputInLines[i][0].size() - 7);
			std::string _expectedDescription = outputInLines[i][1].substr(13, outputInLines[i][1].size() - 13);
			std::string _expectedDateCreated = outputInLines[i][2].substr(14, outputInLines[i][2].size() - 14);
			std::string _expectedExecDate = outputInLines[i][3].substr(16, outputInLines[i][3].size() - 16);
			std::string _status = outputInLines[i][4].substr(8, outputInLines[i][4].size() - 8);

			it = map.find(i + 1);

			EXPECT_EQ(it->second->getTitle(), _expectedTitle);
			EXPECT_EQ(it->second->getDescription(), _expectedDescription);
			EXPECT_EQ(it->second->getDateCreated()->getFormat(true), _expectedDateCreated);
			if (it->second->getExecutionDate() != nullptr) {
				EXPECT_EQ(it->second->getExecutionDate()->getFormat(true), _expectedExecDate);
			}
			else {
				EXPECT_TRUE(it->second->getExecutionDate() == nullptr);
			}
			if (_status == "FINISHED") {
				EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::FINISHED);
			}
			else {
				EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
			}
		}
	}


	void TearDown() {
		std::cout << "TearDown()" << std::endl;
		Mock::AllowLeak(fwMock);
		Mock::VerifyAndClear(fwMock);
		Mock::AllowLeak(dtMock);
		Mock::VerifyAndClear(dtMock);
	}

	void print() {
		std::map<int, ReminderEntry*> mapa = set->getAll();
		std::map<int, ReminderEntry*>::iterator it;

		for (it = mapa.begin(); it != mapa.end(); it++) {
			std::cout << "ENTRY _ " << it->first << std::endl;
			std::cout << it->second->getFileOutput() << std::endl;
			std::cout << "===================================" << std::endl;
		}
	}
};

class ReminderSet_MethodTest : public ReminderSetTest_Constructor {
protected:
	void makeNewEntryAndTest() {
		int currentSize = set->getAll().size();

		std::string title = "NewEntryTitle";
		std::string description = "New entry description";
		std::vector<int> dateCreated = { date[0], date[1], date[2] + 1, date[3], date[4], date[5] };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(dateCreated));
		DateTime* tmpDateCreated = new DateTime(1, 1, 2031, 1, 1, 1);
		DateTime* execDate = new DateTime(28,6,2031, 10,0);



		std::string expectedPath = "out/" + title + "_" + helper.getPath(dateCreated[0], dateCreated[1], dateCreated[2], dateCreated[3], dateCreated[4], dateCreated[5]) +
			"_" + helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	
		std::string expectedOutput = _title + title + "\n" + _description + description + "\n" + _dateCreated + tmpDateCreated->getFormat(true) + "\n" +
			_execDate + execDate->getFormat(true) + "\n" + _status + "NOT FINISHED";

		std::vector<std::string> lines;
		lines.push_back(_title + title); lines.push_back(_description + description); lines.push_back(_dateCreated + tmpDateCreated->getFormat(true));
		lines.push_back(_execDate + execDate->getFormat(true)); lines.push_back(_status + "NOT FINISHED");

		paths_and_outputs.push_back(std::tuple<std::string, std::string, std::vector<std::string>>(expectedPath, expectedOutput, lines));

		EXPECT_EQ(paths_and_outputs.size(), currentSize + 1);
		
		EXPECT_CALL(*fwMock, writeInFile(expectedPath, expectedOutput)).Times(1);
		ReminderEntry* newEntry = new ReminderEntry(execDate, title, description, dtMock, fwMock);

		EXPECT_CALL(*fwMock, fileExists(expectedPath)).WillOnce(Return(true));
		set->makeNewEntry(newEntry);

		std::map<int, ReminderEntry*> map = set->getAll();
		std::map<int, ReminderEntry*>::iterator it = map.find(currentSize + 1);

		EXPECT_EQ(it->second->getTitle(), title);
		EXPECT_EQ(it->second->getDescription(), description);
		EXPECT_TRUE(*it->second->getDateCreated() == *tmpDateCreated);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
		EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
	}
};

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock) {
	testConstructor();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase) {
	testConstructor();
	makeNewEntryAndTest();
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
/*
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

	std::vector<std::tuple<std::string, std::string, std::vector<std::string>>> paths_and_outputs; // first=path, second=
	

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
*/

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
