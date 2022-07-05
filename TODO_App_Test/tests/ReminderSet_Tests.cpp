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

		SetMap map = set->getAll();
		SetMap::iterator it;
		
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
		//std::cout << "TearDown()" << std::endl;
		Mock::AllowLeak(fwMock);
		Mock::VerifyAndClear(fwMock);
		Mock::AllowLeak(dtMock);
		Mock::VerifyAndClear(dtMock);
	}

	void print() {
		SetMap mapa = set->getAll();
		SetMap::iterator it;

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
		DateTime* execDate = new DateTime(28, 6, 2031, 10, 0);



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

		SetMap map = set->getAll();
		SetMap::iterator it = map.find(currentSize + 1);

		EXPECT_EQ(it->second->getTitle(), title);
		EXPECT_EQ(it->second->getDescription(), description);
		EXPECT_TRUE(*it->second->getDateCreated() == *tmpDateCreated);
		EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
		EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);
	}

	void deleteEntryAndTest(int index) {
		SetMap map = set->getAll();
		SetMap::iterator it;
		int currentSize = map.size();

		if ((it = map.find(index)) != map.end()) {
			std::string tmpPath = std::get<0>(paths_and_outputs[index - 1]);

			EXPECT_CALL(*fwMock, deleteFile(tmpPath)).WillOnce(Return(true));
			set->deleteEntry(index);

			map = set->getAll();
			EXPECT_EQ(map.size(), currentSize - 1);
		}
		else {
			try {
				set->deleteEntry(index);
			}
			catch (InvalidIndexException err) {
				throw err;
			}
		}
	}

	void deleteEntryAndTest(ReminderEntry* toDelete) {
		SetMap map = set->getAll();
		SetMap::iterator it;
		int currentSize = map.size();

		if (toDelete == nullptr) {
			try {
				set->deleteEntry(toDelete);
			}
			catch (NullObjectException err) {
				throw err;
			}

		}

		bool loopedThroughSetWithoutFindingOne = true;

		for (it = map.begin(); it != map.end(); it++) {
			if (*it->second == *toDelete) {
				EXPECT_CALL(*fwMock, deleteFile(toDelete->getFilePath())).WillOnce(Return(true));
				set->deleteEntry(toDelete);

				map = set->getAll();
				EXPECT_EQ(map.size(), currentSize - 1);
				for (it = map.begin(); it != map.end(); it++) {
					if (*it->second == *toDelete) {
						std::cout << "Element wasn't deleted" << std::endl;
						EXPECT_TRUE(false);
					}
				}
				loopedThroughSetWithoutFindingOne = false;
				break;
			}
		}

		if (loopedThroughSetWithoutFindingOne) {
			try {
				set->deleteEntry(toDelete);
			}
			catch (InvalidIndexException err) {
				throw err;
			}
		}


	}

	void editEntryAndTest(int index, ReminderEntry* edit) {
		SetMap map = set->getAll();
		SetMap::iterator it;
		if (edit == nullptr) {
			try {
				set->editEntry(index, edit);
			}
			catch (NullObjectException err) {
				throw err;
			}
		}

		if ((it = map.find(index)) == map.end()) {
			try {
				set->editEntry(index, edit);
			}
			catch (InvalidIndexException err) {
				throw err;
			}
		}
		
		EXPECT_CALL(*fwMock, deleteFile(it->second->getFilePath())).WillOnce(Return(true));
		set->editEntry(index, edit);	

		it = map.find(index);

		EXPECT_EQ(it->second->getTitle(), edit->getTitle());
		EXPECT_EQ(it->second->getDescription(), edit->getDescription());
		EXPECT_TRUE(*it->second->getExecutionDate() == *edit->getExecutionDate());
		EXPECT_EQ(it->second->getEntryStatus(), edit->getEntryStatus());
		EXPECT_EQ(it->second->getFilePath(), edit->getFilePath());

	}
};

class ReminderSet_SortAndFilterTest : public Test {
protected:
	int setSize; // default size

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

	void SetUp(int n = 10) {
		this->setSize = n;


		fwMock = new FileWorkerMock();
		dtMock = new DateTimeWorkerMock();

		for (int i = 1; i != setSize + 1; i++) {
			std::vector<int> tmp_DateCreated{ (2 * i + date[0])%MAX_DAY_VALUE_FEBRUARY_NOLEAP , (i + date[1])%MAX_MONTH_VALUE, date[2],
				(i + date[3])%MAX_HOURS_VALUE , (2 * i + date[4])%MAX_MINUTES_VALUE, (2 * (i + date[5]))%MAX_SECONDS_VALUE};
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
		for (int i = 0; i < setSize; i++) {
			EXPECT_CALL(*fwMock, fileExists(tmp_paths[i])).WillOnce(Return(true));
			EXPECT_CALL(*fwMock, readFromFileInLines(tmp_paths[i])).WillOnce(Return(tmp_outputInLines[i]));
		}

		set = new ReminderSet(fwMock);
	}

	void initialTest() {
		std::vector<std::string> paths;
		std::vector<std::string> outputs;
		std::vector<std::vector<std::string>> outputInLines;

		for (std::tuple<std::string, std::string, std::vector<std::string>> x : paths_and_outputs) {
			paths.push_back(std::get<0>(x));
			outputs.push_back(std::get<1>(x));
			outputInLines.push_back(std::get<2>(x));
		}

		SetMap map = set->getAll();
		SetMap::iterator it;

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

	void sortAndTestByDateCreated(bool descending) {
		std::vector<std::pair<int, DateTime*>> dateCreated_array;
		SetMap map = set->getAll();
		SetMap::iterator it;

		if (setSize == 0) {
			map = set->sortByDateCreated(descending);
			EXPECT_TRUE(map.size() == 0);
			return;
		}

		for (it = map.begin(); it != map.end(); it++) {
			dateCreated_array.push_back(std::pair<int, DateTime*>(it->first, it->second->getDateCreated()));
		}

		int i, j;
		int idToSwap; //its index of min if descending=true or index of max if descending=false

		for (i = 0; i < dateCreated_array.size() - 1; i++) {
			idToSwap = i;
			for (j = i + 1; j < dateCreated_array.size(); j++) {
				if (descending) {
					if (*dateCreated_array[j].second < *dateCreated_array[idToSwap].second) {
						idToSwap = j;
					}
				}
				else {
					if (*dateCreated_array[j].second > *dateCreated_array[idToSwap].second) {
						idToSwap = j;
					}
				}
			}

			//std::vector<int, DateTime*>::swap(&dateCreated_array[idToSwap], &dateCreated_array[i]);
			dateCreated_array[idToSwap].swap(dateCreated_array[i]);
		}

		for (int i = 1; i != dateCreated_array.size(); i++) {
			dateCreated_array[i].first = i;
		}

		map = set->sortByDateCreated(descending);
		for (int i = 1; i != map.size(); i++) {
			EXPECT_TRUE(map.size() == setSize);
			EXPECT_TRUE(*map[i]->getDateCreated() == *dateCreated_array[i-1].second);
		}
	}

	void sortAndTestByExecDate(bool descending){
		std::vector<std::pair<int, DateTime*>> execDate_array;
		SetMap map = set->getAll();
		SetMap::iterator it;

		if (setSize == 0) {
			map = set->sortByExecDate(descending);
			EXPECT_TRUE(map.size() == 0);
			return;
		}

		for (it = map.begin(); it != map.end(); it++) {
			execDate_array.push_back(std::pair<int, DateTime*>(it->first, it->second->getExecutionDate()));
		}

		int i, j;
		int idToSwap; //its index of min if descending=true or index of max if descending=false

		for (i = 0; i < execDate_array.size() - 1; i++) {
			idToSwap = i;
			for (j = i + 1; j < execDate_array.size(); j++) {
				if (descending) {
					if (*execDate_array[j].second < *execDate_array[idToSwap].second) {
						idToSwap = j;
					}
				}
				else {
					if (*execDate_array[j].second > *execDate_array[idToSwap].second) {
						idToSwap = j;
					}
				}
			}

			execDate_array[idToSwap].swap(execDate_array[i]);
		}

		for (int i = 1; i != execDate_array.size(); i++) {
			execDate_array[i].first = i;
		}

		map = set->sortByExecDate(descending);
		for (int i = 1; i != map.size(); i++) {
			EXPECT_TRUE(map.size() == setSize);
			EXPECT_TRUE(*map[i]->getExecutionDate() == *execDate_array[i - 1].second);
		}
	}

	void sortAndTestByStatus(bool finishedFirst) {

	}

	void print(SetMap map) {
		SetMap::iterator it;

		for (it = map.begin(); it != map.end(); it++) {
			std::cout << it->first << " : " << it->second->getDateCreated()->getFormat(true) << std::endl;
		}
	}
};


TEST_F(ReminderSetTest_Constructor, ConstructorWithMock) {
	testConstructor();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase) {
	testConstructor();
	makeNewEntryAndTest();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex) {
	deleteEntryAndTest(1);
	deleteEntryAndTest(5);
	deleteEntryAndTest(10);
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex) {
	try {
		deleteEntryAndTest(0);
		
		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
	}

	try {
		deleteEntryAndTest(11);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
	}

	try {
		deleteEntryAndTest(1);
		deleteEntryAndTest(1); // should throw exception because of multiple tries of deleting the same

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithNullObject) {
	testConstructor();

	try {
		deleteEntryAndTest(nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
}

//Entry doesn't exist in set
TEST_F(ReminderSet_MethodTest, DeleteEntry_WithInvalidObject) {
	std::string newEntry_title = "Title: title";
	std::string newEntry_description = "Description: description";
	std::string newEntry_dateCreated = "Date created: 01/07/2022 11:20:00";
	std::string newEntry_execDate = "Execution date: 00/00/0000 00:00:00";
	std::string newEntry_status = "Status: NOT FINISHED";

	std::string newEntry_path = "out/title_" + helper.getPath(1, 7, 2022, 11, 20, 0) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string newEntry_output = newEntry_title + "\n" + newEntry_description + "\n" + newEntry_dateCreated + "\n" + newEntry_execDate + "\n" + newEntry_status;

	std::vector<int> newEntry_date{1,7,2022,11,20,0};
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(newEntry_date));
	EXPECT_CALL(*fwMock, writeInFile(newEntry_path, newEntry_output)).Times(1);
	ReminderEntry* newEntry = new ReminderEntry("title", "description", dtMock, fwMock);

	try {
		deleteEntryAndTest(newEntry);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_OBJECT);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_OBJECT);
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject) {
	testConstructor();

	SetMap map = set->getAll();
	SetMap::iterator it;

	it = map.find(1);
	deleteEntryAndTest(it->second);

	it = map.find(10);
	deleteEntryAndTest(it->second);

	it = map.find(5);
	deleteEntryAndTest(it->second);
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject){
	testConstructor();

	SetMap map = set->getAll();
	int size = map.size();

	for (int i = 1; i <= size; i++) {
		try {
			editEntryAndTest(i, nullptr);

			FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
		}
		catch (NullObjectException err) {
			EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
		}
	}
}


TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_1) {
	DateTime* dt;
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	DateTime* execDate = new DateTime(4,7,2100,12,0,0);
	
	std::string new_title = "Title: new title";
	std::string new_description = "Description: new description";
	std::string new_dc = "Date created: " + dt->getFormat(true);
	std::string new_ed = "Execution date: " + execDate->getFormat(true);
	std::string new_status = "Status: NOT FINISHED";

	std::string newPath = "out/" + new_title + "_"+ helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" +
		helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	std::string newOutput = new_title + "\n" + new_description + "\n" + new_dc + "\n" + new_ed + "\n" + new_status;

	std::vector<std::string> reading;
	reading.push_back(new_title); reading.push_back(new_description); reading.push_back(new_dc); reading.push_back(new_ed); reading.push_back(new_status);

	ReminderEntry* edit = new ReminderEntry(reading, dtMock, fwMock);
	
	editEntryAndTest(1, edit);
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_2) {
	DateTime* dt;
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	DateTime* execDate = new DateTime(4, 7, 2100, 12, 0, 0);

	std::string new_title = "Title: new title";
	std::string new_description = "Description: new description";
	std::string new_dc = "Date created: " + dt->getFormat(true);
	std::string new_ed = "Execution date: " + execDate->getFormat(true);
	std::string new_status = "Status: NOT FINISHED";

	std::string newPath = "out/" + new_title + "_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" +
		helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	std::string newOutput = new_title + "\n" + new_description + "\n" + new_dc + "\n" + new_ed + "\n" + new_status;

	std::vector<std::string> reading;
	reading.push_back(new_title); reading.push_back(new_description); reading.push_back(new_dc); reading.push_back(new_ed); reading.push_back(new_status);

	ReminderEntry* edit = new ReminderEntry(reading, dtMock, fwMock);

	editEntryAndTest(10, edit);
}


TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_3) {
	DateTime* dt;
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	DateTime* execDate = new DateTime(4, 7, 2100, 12, 0, 0);

	std::string new_title = "Title: new title";
	std::string new_description = "Description: new description";
	std::string new_dc = "Date created: " + dt->getFormat(true);
	std::string new_ed = "Execution date: " + execDate->getFormat(true);
	std::string new_status = "Status: NOT FINISHED";

	std::string newPath = "out/" + new_title + "_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" +
		helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	std::string newOutput = new_title + "\n" + new_description + "\n" + new_dc + "\n" + new_ed + "\n" + new_status;

	std::vector<std::string> reading;
	reading.push_back(new_title); reading.push_back(new_description); reading.push_back(new_dc); reading.push_back(new_ed); reading.push_back(new_status);

	ReminderEntry* edit = new ReminderEntry(reading, dtMock, fwMock);

	editEntryAndTest(5, edit);
}


TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex) {
	DateTime* dt;
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	DateTime* execDate = new DateTime(4, 7, 2100, 12, 0, 0);

	std::string new_title = "Title: new title";
	std::string new_description = "Description: new description";
	std::string new_dc = "Date created: " + dt->getFormat(true);
	std::string new_ed = "Execution date: " + execDate->getFormat(true);
	std::string new_status = "Status: NOT FINISHED";

	std::string newPath = "out/" + new_title + "_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" +
		helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	std::string newOutput = new_title + "\n" + new_description + "\n" + new_dc + "\n" + new_ed + "\n" + new_status;

	std::vector<std::string> reading;
	reading.push_back(new_title); reading.push_back(new_description); reading.push_back(new_dc); reading.push_back(new_ed); reading.push_back(new_status);

	ReminderEntry* edit = new ReminderEntry(reading, dtMock, fwMock);

	try {
		editEntryAndTest(0, edit);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
	}

	try {
		editEntryAndTest(-1, edit);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
	}

	try {
		editEntryAndTest(11, edit);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
	}

	try {
		editEntryAndTest(100, edit);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
	}
}


TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_ascending_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_ascending_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_ascending_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithOneEntry) {
	SetUp(1);
	initialTest();
	sortAndTestByDateCreated(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithOneEntry_asc) {
	SetUp(1);
	initialTest();
	sortAndTestByDateCreated(false);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithZeroEntries) {
	SetUp(0);
	initialTest();
	sortAndTestByDateCreated(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithZeroEntries_asc) {
	SetUp(0);
	initialTest();
	sortAndTestByDateCreated(false);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(false);
}


TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(false);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(false);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withOneEntry) {
	SetUp(1);
	initialTest();
	sortAndTestByExecDate(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withOneEntry_asc) {
	SetUp(1);
	initialTest();
	sortAndTestByExecDate(false);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withZeroEntries) {
	SetUp(0);
	initialTest();
	sortAndTestByExecDate(true);
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withZeroEntries_asc) {
	SetUp(0);
	initialTest();
	sortAndTestByExecDate(false);
}

/*
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap sortedByStatus_finishedFirst = set.sortByStatus(true);
	EXPECT_TRUE(sortedByStatus_finishedFirst.size() == 1);
	SetMap::iterator it = sortedByStatus_finishedFirst.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	SetMap sortedByStatus_notFinishedFirst = set.sortByStatus(false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap sortedByStatus_1 = set.sortByStatus(true);
	EXPECT_TRUE(sortedByStatus_1.size() == 0);

	SetMap sortedByStatus_2 = set.sortByStatus(true);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 9);

	int i = 1;
	SetMap::iterator it;
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
	SetMap sortedByTitle_descending = set.sortByTitle(true);


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
	SetMap sortedByTitle_ascending = set.sortByTitle(false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap sortedByTitle_1 = set.sortByTitle(true);
	EXPECT_TRUE(sortedByTitle_1.size() == 1);
	SetMap::iterator it = sortedByTitle_1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	SetMap sortedByTitle_2 = set.sortByTitle(false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap sorted_1 = set.sortByTitle(true);
	EXPECT_TRUE(sorted_1.size() == 0);

	SetMap sorted_2 = set.sortByTitle(true);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 9);

	int i = 1;
	SetMap::iterator it;
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
	SetMap sortedByDescription_descending = set.sortByTitle(true);


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
	SetMap sortedByDescription_ascending = set.sortByTitle(false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap sortedByDescription_1 = set.sortByDescription(true);
	EXPECT_TRUE(sortedByDescription_1.size() == 1);
	SetMap::iterator it = sortedByDescription_1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == DateTime(dtMock));
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);



	SetMap sortedByDescription_2 = set.sortByDescription(false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap sorted_1 = set.sortByDescription(true);
	EXPECT_TRUE(sorted_1.size() == 0);

	SetMap sorted_2 = set.sortByDescription(true);
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
	SetMap map = set.getAll();
	SetMap::iterator it;
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

	
	SetMap filteredByStatus1 = set.filterByStatus(EntryStatus::FINISHED);

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
	SetMap filteredByStatus0 = set.filterByStatus(EntryStatus::NOT_FINISHED);
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
	SetMap map = set.getAll();
	SetMap::iterator it = map.begin();
	EXPECT_TRUE(map.size() == 1);
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);


	SetMap filtered1 = set.filterByStatus(EntryStatus::NOT_FINISHED);
	EXPECT_TRUE(filtered1.size() == 1);
	it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::NOT_FINISHED);
	

	SetMap filtered2 = set.filterByStatus(EntryStatus::FINISHED);
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
	SetMap map = set.getAll();
	SetMap::iterator it = map.begin();
	EXPECT_TRUE(map.size() == 1);
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);


	SetMap filtered1 = set.filterByStatus(EntryStatus::FINISHED);
	EXPECT_TRUE(filtered1.size() == 1);
	it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "DESCRIPTION");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_TRUE(it->second->getEntryStatus() == EntryStatus::FINISHED);


	SetMap filtered2 = set.filterByStatus(EntryStatus::NOT_FINISHED);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap filtered1 = set.filterByStatus(EntryStatus::FINISHED);
	EXPECT_TRUE(filtered1.size() == 0);

	SetMap filtered2 = set.filterByStatus(EntryStatus::NOT_FINISHED);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 10);


	int i;
	SetMap filtered_beforeDateDescending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	SetMap::iterator it;

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
	SetMap filtered_beforeDateAscending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
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
	SetMap filtered_afterDateDescending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);

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
	SetMap filtered_afterDateAscending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);
	
	SetMap filtered4 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, dt, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, dt, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByDateCreated(FilterMode::AfterDate, dt, true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByDateCreated(FilterMode::AfterDate, dt, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 0);


	SetMap filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 0);

	SetMap filtered3 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 1);
	SetMap::iterator it = filtered3.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered4 = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, dt, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, dt, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByDateCreated(FilterMode::AfterDate, dt, true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByDateCreated(FilterMode::AfterDate, dt, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap filtered1 = set.filterByDateCreated(FilterMode::BeforeDate, new DateTime(), true);
	EXPECT_TRUE(filtered1.size() == 0);

	SetMap filtered2 = set.filterByDateCreated(FilterMode::BeforeDate, new DateTime(), false);
	EXPECT_TRUE(filtered2.size() == 0);

	SetMap filtered3 = set.filterByDateCreated(FilterMode::AfterDate, new DateTime(), true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByDateCreated(FilterMode::AfterDate, new DateTime(), false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 10);


	int i;
	SetMap filtered_beforeDateDescending = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	SetMap::iterator it;

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
	SetMap filtered_beforeDateAscending = set.filterByDateCreated(FilterMode::BeforeDate, filterDate, false);
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
	SetMap filtered_afterDateDescending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, true);

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
	SetMap filtered_afterDateAscending = set.filterByDateCreated(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 0);


	SetMap filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 0);

	SetMap filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 1);
	SetMap::iterator it = filtered3.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 1);

	SetMap filtered1 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, true);
	EXPECT_TRUE(filtered1.size() == 1);
	SetMap::iterator it = filtered1.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);


	SetMap filtered2 = set.filterByExecDate(FilterMode::BeforeDate, filterDate, false);
	EXPECT_TRUE(filtered2.size() == 1);
	it = filtered2.begin();
	EXPECT_EQ(it->second->getTitle(), "TITLE");
	EXPECT_EQ(it->second->getDescription(), "This is description");
	EXPECT_TRUE(*it->second->getDateCreated() == *dt);
	EXPECT_TRUE(*it->second->getExecutionDate() == *execDate);
	EXPECT_EQ(it->second->getEntryStatus(), EntryStatus::NOT_FINISHED);

	SetMap filtered3 = set.filterByExecDate(FilterMode::AfterDate, filterDate, true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByExecDate(FilterMode::AfterDate, filterDate, false);
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
	SetMap map = set.getAll();
	EXPECT_TRUE(map.size() == 0);

	SetMap filtered1 = set.filterByExecDate(FilterMode::BeforeDate, new DateTime(), true);
	EXPECT_TRUE(filtered1.size() == 0);

	SetMap filtered2 = set.filterByExecDate(FilterMode::BeforeDate, new DateTime(), false);
	EXPECT_TRUE(filtered2.size() == 0);

	SetMap filtered3 = set.filterByExecDate(FilterMode::AfterDate, new DateTime(), true);
	EXPECT_TRUE(filtered3.size() == 0);

	SetMap filtered4 = set.filterByExecDate(FilterMode::AfterDate, new DateTime(), false);
	EXPECT_TRUE(filtered4.size() == 0);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}
*/

