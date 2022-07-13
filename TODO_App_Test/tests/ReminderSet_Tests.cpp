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
	int setSize;

	ReminderSet* set;

	// We will iterate and increment each of number in date so we would get different dates for every entry in set
	// We will also have DateTime for execDate and just like with previous one, we will iterate and increment it 
	// so we would get different execution dates for each entry in set
	std::vector<int> date{ 1, 1, 2030, 1, 1, 1 }; 
	
	//x<0> - path
	//x<1> - output
	//x<2> - output in lines
	std::vector<std::tuple<std::string, std::string, StringVector>> paths_and_outputs;
	
	//These will be expected output lines in our files
	const std::string _title = "Title: ";
	const std::string _description = "Description: ";
	const std::string _dateCreated = "Date created: ";
	const std::string _execDate = "Execution date: ";
	const std::string _status = "Status: ";

	FileWorkerMock* fwMock;
	DateTimeWorkerMock* dtMock;
	HelperClass helper;

	void SetUp(int n) {
		this->setSize = n;

		
		fwMock = new FileWorkerMock();
		dtMock = new DateTimeWorkerMock();

		for (int i = 1; i <= setSize; i++) {
			std::vector<int> tmp_DateCreated{  ((2 * i + date[0]) % MAX_DAY_VALUE_FEBRUARY_NOLEAP) + 1, 
				(((i + date[1]) % MAX_MONTH_VALUE)) + 1, 
				date[2],
				((i + date[3]) % MAX_HOURS_VALUE) + 1, 
				((2 * i + date[4]) % MAX_MINUTES_VALUE) + 1, 
				((2 * (i+date[5])) % MAX_SECONDS_VALUE) + 1
			};

			EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(tmp_DateCreated));
			DateTime* dt = new DateTime(dtMock);

			DateTime* tmp_ExecDate = new DateTime(
				(tmp_DateCreated[0] + 1) % MAX_DAY_VALUE_FEBRUARY_NOLEAP + 1, 
				tmp_DateCreated[1] , 
				tmp_DateCreated[2] + 1,												//Increment the year for exec date just to be sure that
																					// exec date is always later than date created
				(tmp_DateCreated[3] + 1) % MAX_HOURS_VALUE + 1, 
				(tmp_DateCreated[4] + 5) % MAX_MINUTES_VALUE + 1, 
				(tmp_DateCreated[5] + 5) % MAX_SECONDS_VALUE +1 );


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
			StringVector lines;
			lines.push_back(titleLine); lines.push_back(descriptionLine); 
			lines.push_back(dcLine); lines.push_back(edLine); lines.push_back(statusLine);

			paths_and_outputs.push_back(std::make_tuple(path, output, lines));

			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(paths_and_outputs.begin(), paths_and_outputs.end(), g);
		}


		StringVector tmp_paths;
		StringVector tmp_outputs;
		std::vector<StringVector> tmp_outputInLines;
		for (std::tuple<std::string, std::string, StringVector> x : paths_and_outputs) {
			tmp_paths.push_back(std::get<0>(x));
			tmp_outputs.push_back(std::get<1>(x));

			StringVector lines;
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

	void testConstructor() {
		StringVector paths;
		StringVector outputs;
		std::vector<StringVector> outputInLines;

		for (std::tuple<std::string, std::string, StringVector> x : paths_and_outputs) {
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

		std::string title = "NewEntryTitle_" + std::to_string(currentSize);						//	NewEntryTitle_N just to have different entries(or atleast something in them)
		std::string description = "New entry description_" + std::to_string(currentSize);		//	Because title is in filename, all the entries will have different filenames
		std::vector<int> dateCreated = { date[0], date[1], date[2] + 1, date[3], date[4], date[5] };
		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(dateCreated));
		DateTime* tmpDateCreated = new DateTime(1, 1, 2031, 1, 1, 1);
		DateTime* execDate = new DateTime(28, 6, 2031, 10, 0);

		std::string expectedPath = "out/" + title + "_" + helper.getPath(dateCreated[0], dateCreated[1], dateCreated[2], dateCreated[3], dateCreated[4], dateCreated[5]) +
			"_" + helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());

		std::string expectedOutput = _title + title + "\n" + _description + description + "\n" + _dateCreated + tmpDateCreated->getFormat(true) + "\n" +
			_execDate + execDate->getFormat(true) + "\n" + _status + "NOT FINISHED";

		StringVector lines;
		lines.push_back(_title + title); lines.push_back(_description + description); lines.push_back(_dateCreated + tmpDateCreated->getFormat(true));
		lines.push_back(_execDate + execDate->getFormat(true)); lines.push_back(_status + "NOT FINISHED");

		paths_and_outputs.push_back(std::tuple<std::string, std::string, StringVector>(expectedPath, expectedOutput, lines));

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

		currentSize++;
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
			currentSize--;
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
		
		std::string newPath = "out/" + edit->getTitle() + "_" +
			helper.getPath(it->second->getDateCreated()->getDay(), it->second->getDateCreated()->getMonth(), it->second->getDateCreated()->getYear(),
				it->second->getDateCreated()->getHours(), it->second->getDateCreated()->getMinutes(), it->second->getDateCreated()->getSeconds()) + "_" +
			helper.getPath(edit->getExecutionDate()->getDay(), edit->getExecutionDate()->getMonth(), edit->getExecutionDate()->getYear(),
				edit->getExecutionDate()->getHours(), edit->getExecutionDate()->getMinutes(), edit->getExecutionDate()->getSeconds());

		std::string newOutput = "Title: " + edit->getTitle() + "\n" +
			"Description: " + edit->getDescription() + "\n" +
			"Date created: " + it->second->getDateCreated()->getFormat(true) + "\n" +
			"Execution date: " + edit->getExecutionDate()->getFormat(true) + "\n" +
			"Status: NOT FINISHED";

		EXPECT_CALL(*fwMock, deleteFile(it->second->getFilePath())).WillOnce(Return(true));
		//EXPECT_CALL(*fwMock, writeInFile(newPath, newOutput)).Times(1);
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
	std::vector<std::tuple<std::string, std::string, StringVector>> paths_and_outputs;

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
			std::vector<int> tmp_DateCreated{ 
				((2 * i + date[0]) % MAX_DAY_VALUE_FEBRUARY_NOLEAP) + 1, 
				((i + date[1]) % MAX_MONTH_VALUE) + 1, 
				date[2],
				((i + date[3]) % MAX_HOURS_VALUE) + 1, 
				((2 * i + date[4]) % MAX_MINUTES_VALUE) + 1,
				((2 * (i + date[5])) % MAX_SECONDS_VALUE) + 1
			};
		
			
			EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillOnce(Return(tmp_DateCreated));
			DateTime* dt = new DateTime(dtMock);


			DateTime* tmp_ExecDate = new DateTime(((tmp_DateCreated[0] + 1) % MAX_DAY_VALUE_FEBRUARY_NOLEAP) +1, 
				tmp_DateCreated[1], 
				tmp_DateCreated[2],
				((tmp_DateCreated[3] + 1) % MAX_HOURS_VALUE) + 1,
				((tmp_DateCreated[4] + 5) % MAX_MINUTES_VALUE) + 1,
				((tmp_DateCreated[5] + 5) % MAX_SECONDS_VALUE) + 1);

			if (*tmp_ExecDate > *dt) {
				tmp_ExecDate->setYear(tmp_ExecDate->getYear() + 1);
			}


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
			StringVector lines;
			lines.push_back(titleLine); lines.push_back(descriptionLine);
			lines.push_back(dcLine); lines.push_back(edLine); lines.push_back(statusLine);

			paths_and_outputs.push_back(std::make_tuple(path, output, lines));

			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(paths_and_outputs.begin(), paths_and_outputs.end(), g);
		}


		StringVector tmp_paths;
		StringVector tmp_outputs;
		std::vector<StringVector> tmp_outputInLines;
		for (std::tuple<std::string, std::string, StringVector> x : paths_and_outputs) {
			tmp_paths.push_back(std::get<0>(x));
			tmp_outputs.push_back(std::get<1>(x));

			StringVector lines;
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
		StringVector paths;
		StringVector outputs;
		std::vector<StringVector> outputInLines;

		for (std::tuple<std::string, std::string, StringVector> x : paths_and_outputs) {
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
		std::vector<DateTime*> expectedDates;

		SetMap map = set->getAll();
		SetMap sorted = set->sortByDateCreated(descending);
		MapIterator it;
		
		for (it = map.begin(); it != map.end(); it++) {
			expectedDates.push_back(it->second->getDateCreated());
		}

		if (map.size() == 0) {
			EXPECT_TRUE(sorted.size() == 0);
			EXPECT_TRUE(expectedDates.size() == 0);
			return;
		}
		else if (map.size() == 1){
			EXPECT_TRUE(sorted.size() == 1);
			EXPECT_TRUE(expectedDates.size() == 1);
			EXPECT_TRUE(*expectedDates[0] == *sorted[1]->getDateCreated());
			return;
		}

		for (int i = 0; i < expectedDates.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedDates.size(); j++) {
				if (descending && *expectedDates[j] > *expectedDates[toSwapId]) {
					toSwapId = j;
				}
				else if (!descending && *expectedDates[j] < *expectedDates[toSwapId]) {
					toSwapId = j;
				}
			}

			std::swap(expectedDates[toSwapId], expectedDates[i]);
		}

		ASSERT_TRUE(sorted.size() == expectedDates.size());
		for (int i = 0; i != expectedDates.size(); i++) {
			ASSERT_TRUE(*sorted[i + 1]->getDateCreated() == *expectedDates[i]);
		}
	}

	void sortAndTestByExecDate(bool descending){
		std::vector<DateTime*> expectedDates;

		SetMap map = set->getAll();
		SetMap sorted = set->sortByExecDate(descending);
		MapIterator it;

		for (it = map.begin(); it != map.end(); it++) {
			expectedDates.push_back(it->second->getExecutionDate());
		}

		if (map.size() == 0) {
			EXPECT_TRUE(sorted.size() == 0);
			EXPECT_TRUE(expectedDates.size() == 0);
			return;
		}
		else if (map.size() == 1) {
			EXPECT_TRUE(sorted.size() == 1);
			EXPECT_TRUE(expectedDates.size() == 1);
			EXPECT_TRUE(*expectedDates[0] == *sorted[1]->getExecutionDate());
			return;
		}

		for (int i = 0; i < expectedDates.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedDates.size(); j++) {
				if (descending && *expectedDates[j] > *expectedDates[toSwapId]) {
					toSwapId = j;
				}
				else if (!descending && *expectedDates[j] < *expectedDates[toSwapId]) {
					toSwapId = j;
				}
			}

			std::swap(expectedDates[toSwapId], expectedDates[i]);
		}

		for (int i = 0; i != expectedDates.size(); i++) {
			ASSERT_TRUE(sorted.size() == expectedDates.size());
			ASSERT_TRUE(*sorted[i + 1]->getExecutionDate() == *expectedDates[i]);
		}
	}

	void sortAndTestByStatus(bool finishedFirst) {
		SetMap sorted = set->sortByStatus(finishedFirst);

		if (setSize == 0) {
			EXPECT_TRUE(sorted.size() == 0);
			return;
		}

		SetMap fullMap = set->getAll();
		MapIterator it;

		std::vector<std::pair<std::string, EntryStatus>> expectedValues; //It will compare this expected values(title and entryStatus) with was my real method returns
																		 // This vector is also in expected order


		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			if (finishedFirst && it->second->getEntryStatus() == EntryStatus::FINISHED) {
				expectedValues.push_back(std::pair<std::string, EntryStatus>(it->second->getTitle(), it->second->getEntryStatus()));
			}
			else if (!finishedFirst && it->second->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				expectedValues.push_back(std::pair<std::string, EntryStatus>(it->second->getTitle(), it->second->getEntryStatus()));
			}
		}

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			if (finishedFirst && it->second->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				expectedValues.push_back(std::pair<std::string, EntryStatus>(it->second->getTitle(), it->second->getEntryStatus()));
			}
			else if (!finishedFirst && it->second->getEntryStatus() == EntryStatus::FINISHED) {
				expectedValues.push_back(std::pair<std::string, EntryStatus>(it->second->getTitle(), it->second->getEntryStatus()));
			}
		}

		for (it = sorted.begin(); it != sorted.end(); it++) {
			EXPECT_EQ(it->second->getTitle(), expectedValues[it->first - 1].first);
			EXPECT_EQ(it->second->getEntryStatus(), expectedValues[it->first - 1].second);
		}
	}

	void sortAndTestByTitle(bool descending) {
		SetMap sorted = set->sortByTitle(descending);

		if (setSize == 0) {
			EXPECT_TRUE(sorted.size() == 0);
			return;
		}

		SetMap fullMap = set->getAll();
		MapIterator it;
		std::vector<std::string> expectedValues;	// expected order

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			expectedValues.push_back(it->second->getTitle());
		}

		for (int i = 0; i < expectedValues.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedValues.size(); j++) {
				if (descending) {
					if (expectedValues[j] > expectedValues[toSwapId]) {
						toSwapId = j;
					}
				}
				else {
					if (expectedValues[j] < expectedValues[toSwapId]) {
						toSwapId = j;
					}
				}
			}

			expectedValues[toSwapId].swap(expectedValues[i]);
		}

		for (it = sorted.begin(); it != sorted.end(); it++) {
			EXPECT_EQ(it->second->getTitle(), expectedValues[it->first - 1]);
		}
	}

	void sortAndTestByDescription(bool descending) {
		SetMap sorted = set->sortByDescription(descending);

		if (setSize == 0) {
			EXPECT_TRUE(sorted.size() == 0);
			return;
		}

		SetMap fullMap = set->getAll();
		MapIterator it;
		std::vector<std::string> expectedValues;	// expected order

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			expectedValues.push_back(it->second->getDescription());
		}

		for (int i = 0; i < expectedValues.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedValues.size(); j++) {
				if (descending) {
					if (expectedValues[j] > expectedValues[toSwapId]) {
						toSwapId = j;
					}
				}
				else {
					if (expectedValues[j] < expectedValues[toSwapId]) {
						toSwapId = j;
					}
				}
			}

			expectedValues[toSwapId].swap(expectedValues[i]);
		}

		for (it = sorted.begin(); it != sorted.end(); it++) {
			EXPECT_EQ(it->second->getDescription(), expectedValues[it->first - 1]);
		}
	}

	void filterAndTestByFilter(EntryStatus filterStatus) {
		SetMap filtered = set->filterByStatus(filterStatus);

		if (setSize == 0) {
			EXPECT_TRUE(setSize == 0);
			return;
		}

		SetMap fullMap = set->getAll();
		MapIterator it;
		std::vector<std::pair<std::string, EntryStatus>> expectedValues;

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			if (it->second->getEntryStatus()  == filterStatus) {
				expectedValues.push_back(std::pair<std::string, EntryStatus>(it->second->getTitle(), it->second->getEntryStatus()));
			}
		}


		for (it = filtered.begin(); it != filtered.end(); it++) {
			EXPECT_EQ(it->second->getTitle(), expectedValues[it->first - 1].first);
			EXPECT_EQ(it->second->getEntryStatus(), expectedValues[it->first - 1].second);
		}
	}

	void filterAndTest_byDateCreated(FilterMode filterMode, DateTime* filterDate, bool descending) {

		SetMap filtered = set->filterByDateCreated(filterMode, filterDate, descending);
		SetMap fullMap = set->getAll();
		MapIterator it;

		std::vector <DateTime*> expectedDates;

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			if (filterMode == FilterMode::BeforeDate && (*it->second->getDateCreated() > *filterDate || *it->second->getDateCreated() == *filterDate)) {
				expectedDates.push_back(it->second->getDateCreated());
			}
			else if (filterMode == FilterMode::AfterDate && *it->second->getDateCreated() < *filterDate) {
				expectedDates.push_back(it->second->getDateCreated());
			}
		}

		if (expectedDates.size() == 0) {
			EXPECT_TRUE(filtered.size() == 0);
			return;
		}
		else if (expectedDates.size() == 1) {
			EXPECT_TRUE(filtered.size() == 1);
			EXPECT_TRUE(*filtered[1]->getDateCreated() == *expectedDates[0]);
			return;
		}

		for (int i = 0; i < expectedDates.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedDates.size(); j++) {
				if (descending && *expectedDates[j] > *expectedDates[toSwapId]) {
					toSwapId = j;
				}
				else if (!descending && *expectedDates[j] < *expectedDates[toSwapId]) {
					toSwapId = j;
				}
			}

			std::swap(expectedDates[toSwapId], expectedDates[i]);
		}


		ASSERT_TRUE(filtered.size() == expectedDates.size());

		int i = 0;
		for (it = filtered.begin(); it != filtered.end(); it++) {
			ASSERT_TRUE(*it->second->getDateCreated() == *expectedDates[i]);
			i++;
		}
	}

	void filterAndTestByExecDate(FilterMode filterMode, DateTime* filterDate, bool descending) {
		SetMap filtered = set->filterByExecDate(filterMode, filterDate, descending);
		SetMap fullMap = set->getAll();
		MapIterator it;

		std::vector <DateTime*> expectedDates;

		for (it = fullMap.begin(); it != fullMap.end(); it++) {
			if (filterMode == FilterMode::BeforeDate && (*it->second->getExecutionDate() > *filterDate || *it->second->getExecutionDate() == *filterDate)) {
				expectedDates.push_back(it->second->getExecutionDate());
			}
			else if (filterMode == FilterMode::AfterDate && *it->second->getExecutionDate() < *filterDate) {
				expectedDates.push_back(it->second->getExecutionDate());
			}
		}

		if (expectedDates.size() == 0) {
			EXPECT_TRUE(filtered.size() == 0);
			return;
		}
		else if (expectedDates.size() == 1) {
			EXPECT_TRUE(filtered.size() == 1);
			EXPECT_TRUE(*filtered[1]->getExecutionDate() == *expectedDates[0]);
			return;
		}

		for (int i = 0; i < expectedDates.size() - 1; i++) {
			int toSwapId = i;

			for (int j = i + 1; j < expectedDates.size(); j++) {
				if (descending && *expectedDates[j] > *expectedDates[toSwapId]) {
					toSwapId = j;
				}
				else if (!descending && *expectedDates[j] < *expectedDates[toSwapId]) {
					toSwapId = j;
				}
			}

			std::swap(expectedDates[toSwapId], expectedDates[i]);
		}

		ASSERT_TRUE(filtered.size() == expectedDates.size());

		int i = 0;
		for (it = filtered.begin(); it != filtered.end(); it++) {
			ASSERT_TRUE(*it->second->getExecutionDate() == *expectedDates[i]);
			i++;
		}
	}

	void TearDown() {
		Mock::AllowLeak(dtMock);
		Mock::VerifyAndClear(dtMock);
		Mock::AllowLeak(fwMock);
		Mock::VerifyAndClear(fwMock);
	}

	void print(SetMap map) {
		SetMap::iterator it;

		for (it = map.begin(); it != map.end(); it++) {
			std::cout << it->first << " : " << it->second->getDateCreated()->getFormat(true) << std::endl;
		}
	}
};

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_0) {
	SetUp(0);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_1) {
	SetUp(2);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_2) {
	SetUp(5);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_3) {
	SetUp(10);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_4) {
	SetUp(50);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSetTest_Constructor, ConstructorWithMock_5) {
	SetUp(1000);
	testConstructor();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_0) {
	SetUp(0);
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_01) {
	SetUp(0);
	makeNewEntryAndTest();
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_02) {
	SetUp(0);

	for (int i = 0; i < 50; i++) {
		makeNewEntryAndTest();
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_1) {
	SetUp(2);
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_11) {
	SetUp(2);
	
	makeNewEntryAndTest();
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_12) {
	SetUp(2);
	

	for (int i = 0; i < 50; i++)
	{
		makeNewEntryAndTest();
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_2) {
	SetUp(5);
	
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_21) {
	SetUp(5);
	
	makeNewEntryAndTest();
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_22) {
	SetUp(5);
	

	for (int i = 0; i < 100;i++) {
		makeNewEntryAndTest();
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_3) {
	SetUp(10);
	
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_4) {
	SetUp(50);
	
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_5) {
	SetUp(1000);
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_51) {
	SetUp(1000);
	
	makeNewEntryAndTest();
	makeNewEntryAndTest();
	TearDown();
}

TEST_F(ReminderSet_MethodTest, MakeNewEntryCase_52) {
	SetUp(1000);
	

	for (int i = 0; i < 2000; i++) {
		makeNewEntryAndTest();
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_1) {
	SetUp(2);
	
	deleteEntryAndTest(1);
	deleteEntryAndTest(2);

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_2) {
	SetUp(5);
	
	deleteEntryAndTest(1);
	deleteEntryAndTest(5);
	deleteEntryAndTest(3);

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_22) {
	SetUp(5);
	

	for(int i=1;i<=5;i++){
		deleteEntryAndTest(i);
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_3) {
	SetUp(10);
	

	deleteEntryAndTest(1);
	deleteEntryAndTest(10);
	deleteEntryAndTest(5);

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_33) {
	SetUp(10);
	

	for (int i = 1; i <= 10; i++) {
		deleteEntryAndTest(i);
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_4) {
	SetUp(50);
	

	deleteEntryAndTest(1);
	deleteEntryAndTest(50);
	deleteEntryAndTest(25);

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_44) {
	SetUp(50);
	

	for (int i = 1; i <= 50; i++) {
		deleteEntryAndTest(i);
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_5) {
	SetUp(1000);
	

	deleteEntryAndTest(1);
	deleteEntryAndTest(1000);
	deleteEntryAndTest(500);

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_ValidIndex_55) {
	SetUp(1000);
	

	for (int i = 1; i <= 10; i++) {
		deleteEntryAndTest(i);
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_01) {
	try {
		SetUp(0);
		deleteEntryAndTest(0);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_02) {
	try {
		SetUp(0);
		deleteEntryAndTest(1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_03) {
	try {
		SetUp(0);
		deleteEntryAndTest(7);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_04) {
	try {
		SetUp(0);
		deleteEntryAndTest(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_11) {
	try {
		SetUp(2);
		deleteEntryAndTest(0);
		
		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_12) {
	try {
		SetUp(2);
		deleteEntryAndTest(3);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_13) {
	try {
		SetUp(2);
		deleteEntryAndTest(1);
		deleteEntryAndTest(1);									// it should throw exception here because we're trying to delete the same entry twice

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_14) {
	try {
		SetUp(2);
		deleteEntryAndTest(10);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_15) {
	try {
		SetUp(2);
		deleteEntryAndTest(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_21) {
	try {
		SetUp(10);
		deleteEntryAndTest(0);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_22) {
	try {
		SetUp(10);
		deleteEntryAndTest(11);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_23) {
	try {
		SetUp(10);
		deleteEntryAndTest(1);
		deleteEntryAndTest(1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_24) {
	try {
		SetUp(10);
		deleteEntryAndTest(50);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_31) {
	try {
		SetUp(50);
		deleteEntryAndTest(0);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_32) {
	try {
		SetUp(50);
		deleteEntryAndTest(51);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_33) {
	try {
		SetUp(50);
		deleteEntryAndTest(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_34) {
	try {
		SetUp(50);
		deleteEntryAndTest(5);
		deleteEntryAndTest(5);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_35) {
	try {
		SetUp(50);
		deleteEntryAndTest(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_41) {
	try {
		SetUp(1000);
		deleteEntryAndTest(0);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_42) {
	try {
		SetUp(1000);
		deleteEntryAndTest(1001);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_43) {
	try {
		SetUp(1000);
		deleteEntryAndTest(150);
		deleteEntryAndTest(150);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_44) {
	try {
		SetUp(1000);
		deleteEntryAndTest(1150);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_InvalidIndex_45) {
	try {
		SetUp(1000);
		deleteEntryAndTest(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), std::string(ERR_MSG_INVALID_INDEX));
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithNullObject_0) {
	SetUp(0);
	
	try {
		deleteEntryAndTest(nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithNullObject_1) {
	SetUp(2);
	
	try {
		deleteEntryAndTest(nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithNullObject_2) {
	SetUp(10);
	
	try {
		deleteEntryAndTest(nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithNullObject_3) {
	SetUp(500);
	
	try {
		deleteEntryAndTest(nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithInvalidObject_0) {
		SetUp(0);

		std::string newEntry_title = "Title: title";
		std::string newEntry_description = "Description: description";
		std::string newEntry_dateCreated = "Date created: 01/07/2022 11:20:00";
		std::string newEntry_execDate = "Execution date: 00/00/0000 00:00:00";
		std::string newEntry_status = "Status: NOT FINISHED";

		std::string newEntry_path = "out/title_" + helper.getPath(1, 7, 2022, 11, 20, 0) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
		std::string newEntry_output = newEntry_title + "\n" + newEntry_description + "\n" + newEntry_dateCreated + "\n" + newEntry_execDate + "\n" + newEntry_status;

		std::vector<int> newEntry_date{ 1,7,2022,11,20,0 };
		
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

		TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithInvalidObject_1) {
	SetUp(2);

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

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithInvalidObject_2) {
	SetUp(100);

	std::string newEntry_title = "Title: title";
	std::string newEntry_description = "Description: description";
	std::string newEntry_dateCreated = "Date created: 01/07/2022 11:20:00";
	std::string newEntry_execDate = "Execution date: 00/00/0000 00:00:00";
	std::string newEntry_status = "Status: NOT FINISHED";

	std::string newEntry_path = "out/title_" + helper.getPath(1, 7, 2022, 11, 20, 0) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string newEntry_output = newEntry_title + "\n" + newEntry_description + "\n" + newEntry_dateCreated + "\n" + newEntry_execDate + "\n" + newEntry_status;

	std::vector<int> newEntry_date{ 1,7,2022,11,20,0 };
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

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithInvalidObject_3) {
	SetUp(1000);

	std::string newEntry_title = "Title: title";
	std::string newEntry_description = "Description: description";
	std::string newEntry_dateCreated = "Date created: 01/07/2022 11:20:00";
	std::string newEntry_execDate = "Execution date: 00/00/0000 00:00:00";
	std::string newEntry_status = "Status: NOT FINISHED";

	std::string newEntry_path = "out/title_" + helper.getPath(1, 7, 2022, 11, 20, 0) + "_" + helper.getPath(0, 0, 0, 0, 0, 0);
	std::string newEntry_output = newEntry_title + "\n" + newEntry_description + "\n" + newEntry_dateCreated + "\n" + newEntry_execDate + "\n" + newEntry_status;

	std::vector<int> newEntry_date{ 1,7,2022,11,20,0 };
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

	TearDown();
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_1) {
	SetUp(2);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	it = map.find(1);
	deleteEntryAndTest(it->second);

	it = map.find(2);
	deleteEntryAndTest(it->second);
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_2) {
	SetUp(10);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	it = map.find(1);
	deleteEntryAndTest(it->second);

	it = map.find(10);
	deleteEntryAndTest(it->second);
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_22) {
	SetUp(10);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	for (int i = 1; i <= 10; i++) {
		it = map.find(i);
		deleteEntryAndTest(it->second);
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_3) {
	SetUp(100);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	it = map.find(1);
	deleteEntryAndTest(it->second);

	it = map.find(100);
	deleteEntryAndTest(it->second);
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_33) {
	SetUp(100);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	for (int i = 1; i <= 10; i++) {
		it = map.find(i);
		deleteEntryAndTest(it->second);
	}
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_4) {
	SetUp(1000);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	it = map.find(1);
	deleteEntryAndTest(it->second);

	it = map.find(1000);
	deleteEntryAndTest(it->second);
}

TEST_F(ReminderSet_MethodTest, DeleteEntry_WithConcreteObject_44) {
	SetUp(1000);
	

	SetMap map = set->getAll();
	SetMap::iterator it;

	for (int i = 1; i <= 1000; i++) {
		it = map.find(i);
		deleteEntryAndTest(it->second);
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_1){
	SetUp(1);
	

	SetMap map = set->getAll();
	int size = map.size();

	
	try {
		editEntryAndTest(1, nullptr);
		
		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
	
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_21) {
	SetUp(10);
	

	try {
		editEntryAndTest(1, nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_22) {
	SetUp(10);
	

	try {
		editEntryAndTest(10, nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_23) {
	SetUp(10);
	

	SetMap map = set->getAll();
	int size = map.size();

	for (int i = 1; i <= 10; i++) {
		try {
			editEntryAndTest(i, nullptr);

			FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
		}
		catch (NullObjectException err) {
			EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
		}
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_31) {
	SetUp(500);
	

	try {
		editEntryAndTest(1, nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_32) {
	SetUp(500);
	

	try {
		editEntryAndTest(500, nullptr);

		FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
	}
	catch (NullObjectException err) {
		EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithNullObject_33) {
	SetUp(500);
	

	SetMap map = set->getAll();
	int size = map.size();

	for (int i = 1; i <= 500; i++) {
		try {
			editEntryAndTest(i, nullptr);

			FAIL() << "Expected: " + std::string(ERR_MSG_NULL_OBJ);
		}
		catch (NullObjectException err) {
			EXPECT_EQ(err.what(), ERR_MSG_NULL_OBJ);
		}
	}
	TearDown();
}


// This method is just to have more readable code in test case
// nameExtension => in case we call this method more than once
//					we will have different filenames and actual different entries in set
ReminderEntry* getReminderEntry(int nameExtension, DateTimeWorkerMock* dtMock, FileWorkerMock* fwMock) {
	HelperClass helper;
	std::vector<int> date{ 1, 1, 2030, 1, 1, 1 };
	DateTime* dt;
	EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	dt = new DateTime(dtMock);

	DateTime* execDate = new DateTime(4, 7, 2100, 12, 0, 0);

	std::string new_title = "NewTitle_" + std::to_string(nameExtension);
	std::string new_description = "NewDescription_" + std::to_string(nameExtension);
	std::string new_dc = dt->getFormat(true);

	std::string titleOutput = "Title: " + new_title;
	std::string descriptionOutput = "Description: " + new_description;
	std::string dcOutput = "Date created: " + new_dc;
	
	std::string edOutput = "Execution date: " + execDate->getFormat(true);
	std::string statusOutput = "Status: NOT FINISHED";

	std::string newPath = "out/" + new_title + "_" + helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" +
		helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(), execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
	std::string newOutput = titleOutput + "\n" + descriptionOutput + "\n" + dcOutput + "\n" + edOutput + "\n" + statusOutput;

	StringVector reading;
	reading.push_back(titleOutput); reading.push_back(descriptionOutput); reading.push_back(dcOutput); reading.push_back(edOutput); reading.push_back(statusOutput);


	EXPECT_CALL(*fwMock, writeInFile(_, _)).Times(AnyNumber());
	return new ReminderEntry(execDate, new_title, new_description, dtMock, fwMock);
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_1) {
	SetUp(1);
	

	editEntryAndTest(1, getReminderEntry(setSize + 1, this->dtMock, this->fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_11) {
	SetUp(1);
	

	for (int i = 1; i < 10; i++) {
		editEntryAndTest(1, getReminderEntry(setSize + i, dtMock, fwMock));
	}

	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_21) {
	SetUp(10);
	

	editEntryAndTest(1, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_22) {
	SetUp(10);
	

	editEntryAndTest(10, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_23) {
	SetUp(10);
	

	editEntryAndTest(5, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_24) {
	SetUp(10);
	

	for (int i = 1; i <= 100; i++) {
		editEntryAndTest( i%10 +1, getReminderEntry(setSize + i, dtMock, fwMock)); //edit every entry 10 times
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_31) {
	SetUp(500);
	
	editEntryAndTest(1, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_32) {
	SetUp(500);
	
	editEntryAndTest(500, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_33) {
	SetUp(500);
	
	editEntryAndTest(250, getReminderEntry(setSize + 1, dtMock, fwMock));
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithValidIndex_34) {
	SetUp(500);
	

	for (int i = 1; i <= 2500; i++) {
		editEntryAndTest(i%500 +1, getReminderEntry(setSize + i, dtMock, fwMock)); //edit every entry 5 times
	}
	TearDown();
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_01) {
	SetUp(0);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(0, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_02) {
	SetUp(0);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_03) {
	SetUp(0);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(-1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_04) {
	SetUp(0);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(2, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_11) {
	SetUp(1);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(-1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_12) {
	SetUp(1);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(2, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_13) {
	SetUp(1);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(10, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_21) {
	SetUp(10);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(-1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_22) {
	SetUp(10);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(0, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_23) {
	SetUp(10);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(11, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_24) {
	SetUp(10);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(12, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_25) {
	SetUp(1);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(50, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_31) {
	SetUp(100);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(-1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_32) {
	SetUp(100);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(0, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_33) {
	SetUp(100);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(101, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_34) {
	SetUp(100);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(200, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_41) {
	SetUp(2500);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(-1, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_42) {
	SetUp(2500);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(0, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_43) {
	SetUp(2500);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(5001, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
	}
}

TEST_F(ReminderSet_MethodTest, EditEntry_WithInvalidIndex_44) {
	SetUp(2500);

	try {
		ReminderEntry* r = getReminderEntry(0, dtMock, fwMock);

		editEntryAndTest(5002, r);

		FAIL() << "Expected: " + std::string(ERR_MSG_INVALID_INDEX);
	}
	catch (InvalidIndexException err) {
		EXPECT_EQ(err.what(), ERR_MSG_INVALID_INDEX);
		TearDown();
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

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_4) {
	SetUp(50);
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

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_ascending_4) {
	SetUp(50);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithOneEntry) {
	SetUp(1);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithOneEntry_asc) {
	SetUp(1);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithZeroEntries) {
	SetUp(0);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDateCreated_WithZeroEntries_asc) {
	SetUp(0);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_4) {
	SetUp(50);
	initialTest();
	sortAndTestByDateCreated(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_5) {
	SetUp(1000);
	initialTest();
	sortAndTestByExecDate(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_1) {
	SetUp(2);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_2) {
	SetUp(5);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_asc_3) {
	SetUp(10);
	initialTest();
	sortAndTestByDateCreated(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withOneEntry) {
	SetUp(1);
	initialTest();
	sortAndTestByExecDate(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withOneEntry_asc) {
	SetUp(1);
	initialTest();
	sortAndTestByExecDate(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withZeroEntries) {
	SetUp(0);
	initialTest();
	sortAndTestByExecDate(true);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByExecDate_withZeroEntries_asc) {
	SetUp(0);
	initialTest();
	sortAndTestByExecDate(false);
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_finishedFirst_1) {
	SetUp(2);
	sortAndTestByStatus(true);
	
	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_finishedFirst_2) {
	SetUp(5);
	sortAndTestByStatus(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_finishedFirst_3) {
	SetUp(10);
	sortAndTestByStatus(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_notFinishedFirst_1) {
	SetUp(2);
	sortAndTestByStatus(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_notFinishedFirst_2) {
	SetUp(5);
	sortAndTestByStatus(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_notFinishedFirst_3) {
	SetUp(10);
	sortAndTestByStatus(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_withZeroEntries_finishedFirst) {
	SetUp(0);
	sortAndTestByStatus(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_withZeroEntries_notFinishedFirst) {
	SetUp(0);
	sortAndTestByStatus(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_withOneEntry_finishedFirst) {
	SetUp(1);
	sortAndTestByStatus(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByStatus_withOneEntry_notFinishedFirst) {
	SetUp(1);
	sortAndTestByStatus(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_1) {
	SetUp(2);
	sortAndTestByTitle(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_2) {
	SetUp(5);
	sortAndTestByTitle(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_3) {
	SetUp(10);
	sortAndTestByTitle(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_asc_1) {
	SetUp(2);
	sortAndTestByTitle(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_asc_2) {
	SetUp(5);
	sortAndTestByTitle(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_asc_3) {
	SetUp(10);
	sortAndTestByTitle(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_withOneEntry) {
	SetUp(1);
	sortAndTestByTitle(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_withOneEntry_asc) {
	SetUp(1);
	sortAndTestByTitle(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_withZeroEntries) {
	SetUp(0);
	sortAndTestByTitle(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByTitle_withZeroEntries_asc) {
	SetUp(0);
	sortAndTestByTitle(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_1) {
	SetUp(2);
	sortAndTestByDescription(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_2) {
	SetUp(5);
	sortAndTestByDescription(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_3){
	SetUp(10);
	sortAndTestByDescription(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_asc_1) {
	SetUp(2);
	sortAndTestByDescription(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_asc_2) {
	SetUp(5);
	sortAndTestByDescription(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_asc_3) {
	SetUp(10);
	sortAndTestByDescription(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_withOneEntry) {
	SetUp(1);
	sortAndTestByDescription(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_withOneEntry_asc) {
	SetUp(1);
	sortAndTestByDescription(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_withZeroEntries) {
	SetUp(0);
	sortAndTestByDescription(true);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, SortByDescription_withZeroEntries_asc) {
	SetUp(0);
	sortAndTestByDescription(false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_1) {
	SetUp(2);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_2) {
	SetUp(5);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_3) {
	SetUp(10);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_4) {
	SetUp(100);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_5) {
	SetUp(2500);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_notFinished_1) {
	SetUp(2);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_notFinished_2) {
	SetUp(5);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_notFinished_3) {
	SetUp(10);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_notFinished_4) {
	SetUp(100);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_notFinished_5) {
	SetUp(2500);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_withOneEntry_finished) {
	SetUp(1);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_withOneEntry_notFinished) {
	SetUp(1);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_withZeroEntries) {
	SetUp(0);
	filterAndTestByFilter(EntryStatus::FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByStatus_withZeroEntries_notFinished) {
	SetUp(0);
	filterAndTestByFilter(EntryStatus::NOT_FINISHED);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_0) {
	SetUp(0);

	// Date should not matter as long as it is a correct date
	// We won't test for correct date as this is covered in DateTime_Tests
	filterAndTest_byDateCreated(FilterMode::BeforeDate, new DateTime(12, 7, 2022, 13, 0, 0), true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, new DateTime(12, 7, 2022, 13, 0, 0), false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, new DateTime(12, 7, 2022, 13, 0, 0), true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, new DateTime(12, 7, 2022, 13, 0, 0), false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_1) {
	SetUp(1);

	DateTime* filterDate = new DateTime(std::string("04/03/2030 03:04:05")); // first date created that is assigned in set

	// Testing with exact date
	// Should include the one because BeforeDate takes every date that is either before or on that date
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, false);

	// With previous reasoning, this one should be empty
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, false);

	//Let's increment filterDate's seconds by 1
	DateTime* filterDate2 = new DateTime(std::string("04/03/2030 03:04:06"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, false);

	//Let's now decrease original filterDate's seconds by 1
	DateTime* filterDate3 = new DateTime(std::string("04/03/2030 03:04:04"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate, false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_2_1) {
	SetUp(2);
	// 1 06/04/2030 04:06:07
	// 2 04/03/2030 03:04:05

	DateTime* filterDate1 = new DateTime(4,3,2030, 3,4,5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);

	
	DateTime* filterDate2 = new DateTime(4, 3, 2030, 3,4,6);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, false);

	DateTime* filterDate3 = new DateTime(4, 3, 2030, 3, 4, 4);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, false);

	DateTime* filterDate4 = new DateTime(6, 4, 2030, 4, 6, 6);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, false);

	DateTime* filterDate5 = new DateTime(6, 4, 2030, 4, 6, 7);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, false);

	DateTime* filterDate6 = new DateTime(6, 4, 2030, 4, 6, 8);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_2_2) {
	SetUp(2);
	// 1 06/04/2030 04:06:07
	// 2 04/03/2030 03:04:05

	DateTime* filterDate1 = new DateTime(4, 3, 2030, 3, 3, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);


	DateTime* filterDate2 = new DateTime(4, 3, 2030, 3, 5, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, false);

	DateTime* filterDate3 = new DateTime(4, 3, 2030, 2, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, false);

	DateTime* filterDate4 = new DateTime(4, 3, 2030, 4, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, false);

	DateTime* filterDate5 = new DateTime(4, 3, 2029, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, false);

	DateTime* filterDate6 = new DateTime(4, 3, 2031, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, false);

	DateTime* filterDate7 = new DateTime(4, 2, 2030, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate7, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate7, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate7, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate7, false);

	DateTime* filterDate8 = new DateTime(3, 4, 2030, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate8, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate8, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate8, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate8, false);

	DateTime* filterDate9 = new DateTime(5, 3, 2030, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate9, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate9, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate9, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate9, false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_3_1) {
	SetUp(10);
	// 1  04/03/2030 03:04:05
	//10. 22/12/2030 12:22:23

	DateTime* filterDate1 = new DateTime(4, 3, 2030, 3, 4, 4);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);

	DateTime* filterDate2 = new DateTime(4, 3, 2030, 3, 4, 5);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, false);

	DateTime* filterDate3 = new DateTime(4, 3, 2030, 3, 4, 6);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);

	DateTime* filterDate4 = new DateTime(22,12,2030,12,22,22);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, false);

	DateTime* filterDate5 = new DateTime(22,12,2030,12,22,23);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, false);

	DateTime* filterDate6 = new DateTime(22, 12, 2030, 12, 22, 24);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, false);

	TearDown();
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_4_1) {
	SetUp(1000);
	// 1    :  04/01/2030 01:04:05
	// 499  :  26/06/2030 17:28:29
	// 500  :  26/06/2030 18:55:56
	// 1000 :  26/12/2030 22:30:31

	DateTime* filterDate1 = new DateTime(std::string("26/06/2030 17:28:29"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);
	
	DateTime* filterDate2 = new DateTime(std::string("26/06/2030 17:28:30"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, false);

	DateTime* filterDate3 = new DateTime(std::string("26/06/2030 17:28:28"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, false);

	DateTime* filterDate4 = new DateTime(std::string("26/06/2030 18:55:55"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, false);

	DateTime* filterDate5 = new DateTime(std::string("26/06/2030 18:55:56"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, false);

	DateTime* filterDate6 = new DateTime(std::string("26/06/2030 18:55:57"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, false);
}

TEST_F(ReminderSet_SortAndFilterTest, FilterByDateCreated_4_2) {
	SetUp(1000);
	// 1    :  04/01/2030 01:04:05
	// 499  :  26/06/2030 17:28:29
	// 500  :  26/06/2030 18:55:56
	// 1000 :  26/12/2030 22:30:31

	DateTime* filterDate1 = new DateTime(std::string("04/01/2030 01:04:04"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate1, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate1, false);

	DateTime* filterDate2 = new DateTime(std::string("04/01/2030 01:04:05"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate2, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate2, false);

	DateTime* filterDate3 = new DateTime(std::string("04/01/2030 01:04:06"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate3, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate3, false);

	DateTime* filterDate4 = new DateTime(std::string("26/12/2030 22:30:31"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate4, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate4, false);

	DateTime* filterDate5 = new DateTime(std::string("26/12/2030 22:30:31"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate5, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate5, false);

	DateTime* filterDate6 = new DateTime(std::string("26/12/2030 22:30:31"));
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::BeforeDate, filterDate6, false);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, true);
	filterAndTest_byDateCreated(FilterMode::AfterDate, filterDate6, false);
}

TEST_F(ReminderSet_SortAndFilterTest, FitlerByDateCreated_5) {
	SetUp(50);

	//We'll go through every date in set and test filtering

	SetMap m = set->getAll();
	MapIterator it;


	for (it = m.begin(); it != m.end(); it++) {
		DateTime* tmp = it->second->getDateCreated();

		filterAndTest_byDateCreated(FilterMode::BeforeDate, tmp, true);
		filterAndTest_byDateCreated(FilterMode::BeforeDate, tmp, false);
		filterAndTest_byDateCreated(FilterMode::AfterDate, tmp, true);
		filterAndTest_byDateCreated(FilterMode::AfterDate, tmp, false);
	}
}

/*
/*
TEST(ReminderSet_Tests, FilterByDateCreated) {
	DateTimeWorkerMock* filterDateMock = new DateTimeWorkerMock();
	std::vector<int> date{ 27, 5, 2022, 9, 56, 31 };
	EXPECT_CALL(*filterDateMock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* filterDate = new DateTime(filterDateMock);

	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();

	FileWorkerMock* mock = new FileWorkerMock();
	HelperClass helper;

	StringVector paths;
	StringVector outputs;
	std::vector<StringVector> outputInLines;

	std::vector<std::tuple<std::string, std::string, StringVector>> paths_and_outputs; // first=path, second=
	

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
		StringVector lines;
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(StringVector()));

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

	StringVector paths;
	StringVector outputs;
	std::vector<StringVector> outputInLines;

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
		StringVector lines;
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	StringVector lines;
	lines.push_back(_title); lines.push_back(_description); lines.push_back(_dateCreated);
	lines.push_back(_execDate); lines.push_back(_status);

	StringVector paths{ _path };
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

	EXPECT_CALL(*mock, getAllFromDirectory("out/")).WillOnce(Return(StringVector()));

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

