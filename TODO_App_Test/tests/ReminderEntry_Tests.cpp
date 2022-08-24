#include "../pch.h"
#include "../../TODO_App/include/ReminderEntry.h"
#include "../mock/DateTimeWorkerMock.h"
#include "../mock/FileWorkerMock.h"
#include "../src/HelperClass.h"

using namespace ::testing;

class ReminderEntryTest_Constructor : public Test {
protected:
	ReminderEntry* entry;
	DateTimeWorkerMock* dtMock = new DateTimeWorkerMock();
	FileWorkerMock* fwMock = new FileWorkerMock();

	DateTime* dt;
	HelperClass helper;
	const std::vector<int> date{ 15, 6 , 2030, 12, 30, 30 }; // random initial date and time
	const std::string title = "Title: ";
	const std::string description = "Description: ";
	const std::string dateCreated = "Date created: ";
	const std::string execDate = "Execution date: ";
	const std::string status = "Status: ";
	std::string output;
	std::string path;
public:
	void SetUp() {

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		dt = new DateTime(dtMock);

		this->path = "out/_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" + NULL_DATETIME_FORMAT_FILENAME;
		this->output = this->title + "\n" + this->description + "\n" + this->dateCreated + dt->getFormat(true) + "\n" 
			+ this->execDate + NULL_DATETIME_FORMAT_OUTPUT + "\n" + this->status + "NOT FINISHED";

		
		EXPECT_CALL(*fwMock, writeInFile(this->path, this->output)).Times(1);
		this->entry = new ReminderEntry(this->dtMock, this->fwMock);
	}
	void SetUp(std::string title, std::string description) {
		this->dtMock = dtMock;
		this->fwMock = fwMock;

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		dt = new DateTime(dtMock);

		std::string correctTitle;
		std::string correctDescription;

		if (title.size() > MAX_TITLE_LEN) {
			correctTitle = title.substr(0, MAX_TITLE_LEN);
		}
		else {
			correctTitle = title;
		}

		if (description.size() > MAX_DESCRIPTION_LEN) {
			correctDescription = description.substr(0, MAX_DESCRIPTION_LEN);
		}
		else {
			correctDescription = description;
		}

		path = "out/" + correctTitle + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_" + NULL_DATETIME_FORMAT_FILENAME;
		output = this->title + correctTitle + "\n" + this->description + correctDescription + "\n" +
			this->dateCreated + dt->getFormat(true) + "\n" + this->execDate + NULL_DATETIME_FORMAT_OUTPUT + "\n" + this->status + "NOT FINISHED";

		EXPECT_CALL(*fwMock, writeInFile(this->path, this->output)).Times(1);
		this->entry = new ReminderEntry(title, description, dtMock, fwMock);
	}
	void SetUp(DateTime* execDate, std::string title, std::string description) {
		this->dtMock = dtMock;
		this->fwMock = fwMock;

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		dt = new DateTime(dtMock);

		if (*execDate > *dt) {
			throw InvalidExecutionDateException(ERR_MSG_INVALID_EXEC_DATE);
		}

		std::string correctTitle;
		std::string correctDescription;
		if (title.size() > MAX_TITLE_LEN) {
			correctTitle = title.substr(0, MAX_TITLE_LEN);
		}
		else {
			correctTitle = title;
		}

		if (description.size() > MAX_DESCRIPTION_LEN) {
			correctDescription = description.substr(0, MAX_DESCRIPTION_LEN);
		}
		else {
			correctDescription = description;
		}

		path = "out/" + correctTitle + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) + "_"
			+ helper.getPath(execDate->getDay(), execDate->getMonth(), execDate->getYear(),
				execDate->getHours(), execDate->getMinutes(), execDate->getSeconds());
		output = this->title + correctTitle + "\n" + this->description + correctDescription + "\n" +
			this->dateCreated + dt->getFormat(true) + "\n" + this->execDate + execDate->getFormat(true) + "\n" + this->status + "NOT FINISHED";
		EXPECT_CALL(*fwMock, writeInFile(this->path, this->output)).Times(1);
		this->entry = new ReminderEntry(execDate, title, description, dtMock, fwMock);
		
	}

	void testConstructor(DateTime* expectedExecDate, std::string expectedTitle, std::string expectedDescription) {
		if (expectedTitle.size() > MAX_TITLE_LEN) {
			EXPECT_NE(entry->getTitle(), expectedTitle);
			EXPECT_EQ(entry->getTitle(), expectedTitle.substr(0, MAX_TITLE_LEN));
		}
		else{
			EXPECT_EQ(entry->getTitle(), expectedTitle);
		}
		if (expectedDescription.size() > MAX_DESCRIPTION_LEN) {
			EXPECT_NE(entry->getDescription(), expectedDescription);
			EXPECT_EQ(entry->getDescription(), expectedDescription.substr(0, MAX_DESCRIPTION_LEN));
		}
		else {
			EXPECT_EQ(entry->getDescription(), expectedDescription);
		}
		if (expectedExecDate == nullptr) {
			EXPECT_TRUE(entry->getExecutionDate() == nullptr);
		}
		else {
			EXPECT_TRUE(*entry->getExecutionDate() == *expectedExecDate);
		}

		EXPECT_EQ(EntryStatus::NOT_FINISHED, entry->getEntryStatus());
		EXPECT_TRUE(*entry->getDateCreated() == *this->dt);
		EXPECT_TRUE(entry->getFilePath() == this->path);
	}

	void TearDown() {
		Mock::AllowLeak(this->fwMock);
		Mock::VerifyAndClear(this->fwMock);
		Mock::AllowLeak(this->dtMock);
		Mock::VerifyAndClear(this->dtMock);
	}
};

class ReminderEntryTest_Setter : public ReminderEntryTest_Constructor {
public:
	void setAndTestTitle(std::string title) {
		std::string correctTitle;
		if (title.size() > MAX_TITLE_LEN) {
			correctTitle = title.substr(0, MAX_TITLE_LEN);
		}
		else {
			correctTitle = title;
		}

		std::string execDate_FileNameFormat;
		std::string execDate_OutputFormat;
		if (entry->getExecutionDate() == nullptr) {
			execDate_FileNameFormat = NULL_DATETIME_FORMAT_FILENAME;
			execDate_OutputFormat = NULL_DATETIME_FORMAT_OUTPUT;
		}
		else {
			execDate_FileNameFormat = helper.getPath(entry->getExecutionDate()->getDay(), entry->getExecutionDate()->getMonth(), entry->getExecutionDate()->getYear(),
				entry->getExecutionDate()->getHours(), entry->getExecutionDate()->getMinutes(), entry->getExecutionDate()->getSeconds());
			execDate_OutputFormat = entry->getExecutionDate()->getFormat(true);
		}
		std::string entryStatusString;
		if (entry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			entryStatusString = "NOT FINISHED";
		}
		else {
			entryStatusString = "FINISHED";
		}

		std::string newPath = "out/" + correctTitle + "_" +
			helper.getPath(entry->getDateCreated()->getDay(), entry->getDateCreated()->getMonth(), entry->getDateCreated()->getYear(),
				entry->getDateCreated()->getHours(), entry->getDateCreated()->getMinutes(), entry->getDateCreated()->getSeconds()) + "_" +
			execDate_FileNameFormat;
		std::string newOutput = this->title + correctTitle + "\n" + this->description + entry->getDescription() + "\n" +
			this->dateCreated + entry->getDateCreated()->getFormat(true) + "\n" + this->execDate + execDate_OutputFormat + "\n" +
			this->status + entryStatusString;

		EXPECT_CALL(*fwMock, deleteFile(this->path)).WillOnce(Return(true));
		EXPECT_CALL(*fwMock, writeInFile(newPath, newOutput)).Times(1);

		entry->setTitle(title);

		if (title.size() > MAX_TITLE_LEN) {
			EXPECT_NE(entry->getTitle(), title);
			EXPECT_EQ(entry->getTitle(), correctTitle);
		}
		else {
			EXPECT_EQ(entry->getTitle(), title);
		}

		this->path = newPath;
		this->output = newOutput;
	}
	void setAndTestDescription(std::string description) {
		std::string correctDescription;
		if (description.size() > MAX_DESCRIPTION_LEN) {
			correctDescription = description.substr(0, MAX_DESCRIPTION_LEN);
		}
		else {
			correctDescription = description;
		}

		std::string execDate_FileNameFormat;
		std::string execDate_OutputFormat;
		if (entry->getExecutionDate() == nullptr) {
			execDate_FileNameFormat = NULL_DATETIME_FORMAT_FILENAME;
			execDate_OutputFormat = NULL_DATETIME_FORMAT_OUTPUT;
		}
		else {
			execDate_FileNameFormat = helper.getPath(entry->getExecutionDate()->getDay(), entry->getExecutionDate()->getMonth(), entry->getExecutionDate()->getYear(),
				entry->getExecutionDate()->getHours(), entry->getExecutionDate()->getMinutes(), entry->getExecutionDate()->getSeconds());
			execDate_OutputFormat = entry->getExecutionDate()->getFormat(true);
		}
		std::string entryStatusString;
		if (entry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			entryStatusString = "NOT FINISHED";
		}
		else {
			entryStatusString = "FINISHED";
		}

		std::string newOutput = this->title + entry->getTitle() + "\n" + this->description + correctDescription + "\n" +
			this->dateCreated + entry->getDateCreated()->getFormat(true) + "\n" + this->execDate + execDate_OutputFormat + "\n" +
			this->status + entryStatusString;

		EXPECT_CALL(*fwMock, deleteFile(this->path)).WillOnce(Return(true));
		EXPECT_CALL(*fwMock, writeInFile(this->path, newOutput)).Times(1);

		entry->setDescription(description);

		if (description.size() > MAX_DESCRIPTION_LEN) {
			EXPECT_NE(entry->getDescription(), description);
			EXPECT_EQ(entry->getDescription(), description.substr(0,MAX_DESCRIPTION_LEN));
		}
		else {
			EXPECT_EQ(entry->getDescription(), description);
		}

		
		this->output = newOutput;
	}
	void setAndTestExecDate(DateTime* newExecDate) {
		if (*newExecDate > *entry->getDateCreated()) {
			throw InvalidExecutionDateException(ERR_MSG_INVALID_EXEC_DATE);
		}

		std::string newPath;
		std::string newOutput;

		if (newExecDate == nullptr) {
			newPath = "out/" + entry->getTitle() + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5]) 
				+ "_" + NULL_DATETIME_FORMAT_FILENAME;
		}
		else {
			newPath = "out/" + entry->getTitle() + "_" + helper.getPath(date[0], date[1], date[2], date[3], date[4], date[5])
				+ "_" + helper.getPath(newExecDate->getDay(), newExecDate->getMonth(), newExecDate->getYear(),
					newExecDate->getHours(), newExecDate->getMinutes(), newExecDate->getSeconds());
		}

		newOutput = this->title + entry->getTitle() + "\n" + this->description + entry->getDescription() + "\n" +
			this->dateCreated + entry->getDateCreated()->getFormat(true) + "\n" + this->execDate + newExecDate->getFormat(true) + "\n" +
			this->status;
		if (entry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			newOutput += "NOT FINISHED";
		}
		else {
			newOutput += "FINISHED";
		}

		EXPECT_CALL(*fwMock, deleteFile(this->path)).WillOnce(Return(true));
		EXPECT_CALL(*fwMock, writeInFile(newPath, newOutput)).Times(1);

		entry->setExecutionDate(*newExecDate);
		if (newExecDate == nullptr) {
			EXPECT_TRUE(entry->getExecutionDate() == nullptr);
		}
		else {
			EXPECT_TRUE(*entry->getExecutionDate() == *newExecDate);
		}
		this->path = newPath;
		this->output = newOutput;
	}
	void setAndTestStatus(EntryStatus status) {
		std::string newOutput = this->title + entry->getTitle() + "\n" + this->description + entry->getDescription() + "\n" +
			this->dateCreated + entry->getDateCreated()->getFormat(true) + "\n" + this->execDate;
		if (entry->getExecutionDate() == nullptr) {
			newOutput += std::string(NULL_DATETIME_FORMAT_OUTPUT) + "\n";
		}
		else {
			newOutput += entry->getExecutionDate()->getFormat(true) +"\n";
		}
		if (status == EntryStatus::NOT_FINISHED) {
			newOutput += this->status + "NOT FINISHED";
		}
		else {
			newOutput += this->status + "FINISHED";
		}
		EXPECT_CALL(*fwMock, deleteFile(this->path)).WillOnce(Return(true));
		EXPECT_CALL(*fwMock, writeInFile(this->path, newOutput)).Times(1);
		this->output = newOutput;
		this->entry->setStatus(status);

		EXPECT_EQ(this->entry->getEntryStatus(), status);
		this->output = newOutput;
	}
	void changeAndTestStatus() {
		std::string execDate_output;
		std::string status_output;
		EntryStatus prevStatus = this->entry->getEntryStatus();

		if (this->entry->getExecutionDate() == nullptr) {
			execDate_output = NULL_DATETIME_FORMAT_OUTPUT;
		}
		else {
			execDate_output = this->entry->getExecutionDate()->getFormat(true);
		}
		if (this->entry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			status_output = "FINISHED";
		}
		else {
			status_output = "NOT FINISHED";
		}

		std::string newOutput = this->title + this->entry->getTitle() + "\n" +
			this->description + this->entry->getDescription() + "\n" +
			this->dateCreated + this->entry->getDateCreated()->getFormat(true) + "\n" +
			this->execDate + execDate_output + "\n" +
			this->status + status_output;

		EXPECT_CALL(*fwMock, deleteFile(this->path)).WillOnce(Return(true));
		EXPECT_CALL(*fwMock, writeInFile(this->path, newOutput)).Times(1);

		this->entry->changeEntryStatus();

		EXPECT_NE(this->entry->getEntryStatus(), prevStatus);

		this->output = newOutput;


	}
};


class ReminderEntryTest_Comparison : public Test {
protected:
	DateTimeWorkerMock* dtMock;
	FileWorkerMock* fwMock;
	HelperClass helper;
	std::vector<int> date{ 15,6,2030, 12,30, 56 };
public:
	ReminderEntry& getReference() {
		dtMock = new DateTimeWorkerMock();
		fwMock = new FileWorkerMock();

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime* dt = new DateTime(dtMock);

		std::string path = "out/_"
			+ helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_" 
			+ NULL_DATETIME_FORMAT_FILENAME;
		std::string output = "Title: \nDescription: \nDate created: " + dt->getFormat(true) + "\nExecution date: " + NULL_DATETIME_FORMAT_OUTPUT +
			"\nStatus: NOT FINISHED";
		EXPECT_CALL(*fwMock, writeInFile(path, output)).Times(1);
		
		ReminderEntry entry(dtMock, fwMock);

		//std::cout << "REFERENCE: " << std::endl;
		//std::cout << "Title: " << entry.getTitle() << std::endl;
		//std::cout << "Description: " << entry.getDescription() << std::endl;
		//std::cout << "Date created: " << entry.getDateCreated()->getFormat(true) << std::endl;
		//std::cout << "Path: " << entry.getFilePath() << std::endl;

		return entry;
	}

	ReminderEntry* getPointer() {
		ReminderEntry* entry;
		dtMock = new DateTimeWorkerMock();
		fwMock = new FileWorkerMock();

		EXPECT_CALL(*dtMock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime* dt = new DateTime(dtMock);

		std::string path = "out/_"
			+ helper.getPath(dt->getDay(), dt->getMonth(), dt->getYear(), dt->getHours(), dt->getMinutes(), dt->getSeconds()) + "_"
			+ NULL_DATETIME_FORMAT_FILENAME;
		std::string output = "Title: \nDescription: \nDate created: " + dt->getFormat(true) + "\nExecution date: " + NULL_DATETIME_FORMAT_OUTPUT +
			"\nStatus: NOT FINISHED";
		EXPECT_CALL(*fwMock, writeInFile(path, output)).Times(1);
		
		entry = new ReminderEntry(dtMock, fwMock);

		//std::cout << "POINTER: " << std::endl;
		//std::cout << "Title: " << entry->getTitle() << std::endl;
		//std::cout << "Description: " << entry->getDescription() << std::endl;
		//std::cout << "Date created: " << entry->getDateCreated()->getFormat(true) << std::endl;
		//std::cout << "Path: " << entry->getFilePath() << std::endl;

		return entry;
	}

	void TearDown() {
		Mock::AllowLeak(this->fwMock);
		Mock::VerifyAndClear(this->fwMock);
		Mock::AllowLeak(this->dtMock);
		Mock::VerifyAndClear(this->dtMock);
	}
};

TEST(ReminderEntryTestNoSuit_Constructor, Constructor_WithFileOutput) {
	StringVector fileReading;
	fileReading.push_back("Title: title");
	fileReading.push_back("Description: description");
	fileReading.push_back("Date created: 24/05/2022 13:30:43");
	fileReading.push_back("Execution date: 26/09/2022 05:35:00");
	fileReading.push_back("Status: NOT FINISHED");

	ReminderEntry* e = new ReminderEntry(fileReading);

	EXPECT_EQ("title", e->getTitle());
	EXPECT_EQ("description", e->getDescription());
	EXPECT_TRUE(DateTime(24, 5, 2022, 13, 30, 43) == *e->getDateCreated());
	EXPECT_TRUE(DateTime(26, 9, 2022, 5, 35) == *e->getExecutionDate());
	EXPECT_EQ(EntryStatus::NOT_FINISHED, e->getEntryStatus());
	std::string expected_path = "out/title_24052022_133043_26092022_053500";
	EXPECT_EQ(expected_path, e->getFilePath());
}

TEST_F(ReminderEntryTest_Constructor, DefaultConstructor) {
	SetUp();
	testConstructor(nullptr, "", "");
	TearDown();
}

TEST_F(ReminderEntryTest_Constructor, CorrectValues_ForConstructor) {
	std::string _title = "title";
	std::string _description = "description";

	SetUp(_title, _description);
	testConstructor(nullptr, _title, _description);
	TearDown();
}

TEST_F(ReminderEntryTest_Constructor, CorrectValues_BoundaryCaseForTitleAndDescription) {
	std::string longDescription = "";
	std::string longTitle = "";
	for (int i = 0; i <= MAX_TITLE_LEN + 1; i++) {
		longTitle += "a";
	}
	for (int i = 0; i <= MAX_DESCRIPTION_LEN + 1; i++) {
		longDescription += "a";
	}
	SetUp(longTitle, longDescription);
	testConstructor(nullptr, longTitle, longDescription);
	TearDown();
}

TEST_F(ReminderEntryTest_Constructor, CorrectValues_LongTitleAndDescription) {
	std::string longDescription;
	std::string longTitle;

	for (int i = 0; i != 2 * MAX_TITLE_LEN; i++) {
		longTitle += "a";
	}
	for (int i = 0; i != 2 * MAX_DESCRIPTION_LEN; i++) {
		longDescription += "a";
	}

	SetUp(longTitle, longDescription);
	testConstructor(nullptr, longTitle, longDescription);
	TearDown();
}

TEST_F(ReminderEntryTest_Constructor, CorrectValues_ForConstructorWithExecDate) {
	DateTime* _execDate = new DateTime(26,9,2100);
	std::string _title = "title";
	std::string _description = "description";

	SetUp(_execDate, _title, _description);
	testConstructor(_execDate, _title, _description);
	TearDown();
}

TEST_F(ReminderEntryTest_Constructor, InvalidValues_ForExecDate) {
	//date created = 15.6.2030 12:30:30
	std::string _title = "title";
	std::string _description = "description";

	try {
		SetUp(new DateTime(15, 6, 2029, 12, 30, 30), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp(new DateTime(15, 5, 2030, 12, 30, 30), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp(new DateTime(14, 6, 2030, 12, 30, 30), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp(new DateTime(15, 6, 2030, 11, 30, 30), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp(new DateTime(15, 6, 2030, 12, 29, 30), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp(new DateTime(15, 6, 2030, 12, 30, 29), _title, _description);
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}
}

TEST_F(ReminderEntryTest_Setter, InvalidValues_ForExecDate) {
	//date created = 15.6.2030 12:30:30
	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(15, 6, 2029, 12, 30, 30));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}
	

	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(15, 5, 2030, 12, 30, 30));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(14, 6, 2030, 12, 30, 30));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(15, 6, 2030, 11, 30, 30));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(15, 6, 2030, 12, 29, 30));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}

	try {
		SetUp();
		testConstructor(nullptr, "", "");
		setAndTestExecDate(new DateTime(15, 6, 2030, 12, 30, 29));
		FAIL() << "Expected:" + std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
	}
	catch (InvalidExecutionDateException err) {
		std::string msg = std::string(TYPE_INV_EXECDATE_EXCEPTION) + std::string(AUX_MSG) + std::string(ERR_MSG_INVALID_EXEC_DATE);
		EXPECT_EQ(err.what(), msg);
	}
}

TEST_F(ReminderEntryTest_Setter, CorrectValues_ForExecDate) {
	//date created = 15.6.2030 12:30:30
	
	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(15, 6, 2031, 12, 30, 30));
	TearDown();
	
	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(15, 7, 2030, 12, 30, 30));
	TearDown();
	
	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(16, 6, 2030, 12, 30, 30));
	TearDown();
	
	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(15, 6, 2030, 13, 30, 30));
	TearDown();

	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(15, 6, 2030, 12, 31, 30));
	TearDown();

	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestExecDate(new DateTime(15, 6, 2030, 12, 30, 31));
	TearDown();
	
}

TEST_F(ReminderEntryTest_Setter, CorrectValues_ForTitle) {
	std::string title = "This is some random title";
	std::string description = "This is some random description";

	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestTitle(title);
	setAndTestDescription(description);
}

TEST_F(ReminderEntryTest_Setter, CorrectValues_ForLong_TitleAndDescription) {
	std::string longDescription;
	std::string longTitle;

	for (int i = 0; i < 2 * MAX_TITLE_LEN; i++) {
		longTitle += "a";
	}
	for (int i = 0; i < 2 * MAX_DESCRIPTION_LEN; i++) {
		longDescription += "a";
	}

	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestTitle(longTitle);
	setAndTestDescription(longDescription);
}

TEST_F(ReminderEntryTest_Setter, CorrectValues_BoundaryCase_TitleAndDescription) {
	std::string longDescription;
	std::string longTitle;

	for (int i = 0; i < MAX_TITLE_LEN + 1; i++) {
		longTitle += "a";
	}
	for (int i = 0; i < MAX_DESCRIPTION_LEN + 1; i++) {
		longDescription += "a";
	}


	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestTitle(longTitle);
	setAndTestDescription(longDescription);
	TearDown();
}

TEST_F(ReminderEntryTest_Setter, CorrectValues_ForStatus) {
	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestStatus(EntryStatus::FINISHED);
	TearDown();

	SetUp();
	testConstructor(nullptr, "", "");
	setAndTestStatus(EntryStatus::FINISHED);
	setAndTestStatus(EntryStatus::NOT_FINISHED);
	TearDown();

	SetUp("title", "description");
	testConstructor(nullptr, "title", "description");
	setAndTestStatus(EntryStatus::FINISHED);
	TearDown();

	SetUp("title", "description");
	testConstructor(nullptr, "title", "description");
	setAndTestStatus(EntryStatus::FINISHED);
	setAndTestStatus(EntryStatus::NOT_FINISHED);
	TearDown();

	SetUp(new DateTime(22, 6, 2040, 10, 23, 44), "title", "description");
	testConstructor(new DateTime(22, 6, 2040, 10, 23, 44), "title", "description");
	setAndTestStatus(EntryStatus::FINISHED);
	setAndTestStatus(EntryStatus::NOT_FINISHED);
	TearDown();
}

TEST_F(ReminderEntryTest_Setter, ChangeStatus) {
	SetUp();
	testConstructor(nullptr, "", "");
	changeAndTestStatus();
	changeAndTestStatus();
	TearDown();

	SetUp("title", "description");
	testConstructor(nullptr, "title", "description");
	changeAndTestStatus();
	changeAndTestStatus();
	TearDown();

	SetUp(new DateTime(22, 6, 2040, 10, 47, 32), "title", "description");
	testConstructor(new DateTime(22,6,2040,10,47,32), "title", "description");
	changeAndTestStatus();
	changeAndTestStatus();
	TearDown();
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

TEST(ReminderEntryTest_MoveAndCopy, CopyConstructor) {
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

	StringVector _outputInLines;
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
