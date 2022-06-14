#include "../include/ReminderEntry.h"


ReminderEntry::ReminderEntry(DateTimeWorkerInterface* dtw, FileWorkerInterface* fw)
{
	if (fw != nullptr) {
		//std::cout << "ReminderEntry() => " << fw << std::endl;
		fileWorker = std::move(fw);
	}
	dateCreated = new DateTime(dtw);
	executionDate = NULL;
	title = "";
	description = "";
	filePath = "out/_" + getFileNameFormatFromDate(*dateCreated) + "_00000000_000000";

	writeInFile();
}

ReminderEntry::ReminderEntry(std::string title, std::string description, DateTimeWorkerInterface* dtw, FileWorkerInterface* fw) {

	if (fw != nullptr) {
		fileWorker = std::move(fw);
	}

	dateCreated = new DateTime(dtw);
	executionDate = NULL;

	if (title.length() > MAX_TITLE_LEN) {
		this->title = title.substr(0, MAX_TITLE_LEN);
	}
	else {
		this->title = title;
	}
		

	if (description.length() > MAX_DESCRIPTION_LEN) {
		this->description = description.substr(0, MAX_DESCRIPTION_LEN);
	}
	else {
		this->description = description;
	}

	filePath = "out/" + this->title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";

	std::string execDatePath = "00000000_000000";
	
	filePath += execDatePath;
	writeInFile();
}

ReminderEntry::ReminderEntry(std::vector<std::string> reading, DateTimeWorkerInterface* dtw, FileWorkerInterface* fw){
	if (fw != nullptr) {
			fileWorker = std::move(fw);
	}

	if (reading.size() != 5) {
		throw "Wrong format";
	}
	
	if (reading[0].substr(0, 7) != "Title: ") {
		throw "Wrong format";
	}
	if (reading[1].substr(0, 13) != "Description: ") {
		throw "Wrong format";
	}
	if (reading[2].substr(0, 14) != "Date created: ") {
		throw "Wrong format";
	}
	if (reading[3].substr(0, 16) != "Execution date: ") {
		throw "Wrong format";
	}
	if (reading[4].substr(0, 8) != "Status: ") {
		throw "Wrong format";
	}


	this->title = reading[0].substr(7, reading[0].size() - 7);
	this->description = reading[1].substr(13, reading[1].size() - 13);
	std::string dt = reading[2].substr(14, reading[2].size() - 14);
	this->dateCreated = new DateTime(dt);
	
	std::string tmp = reading[3].substr(16, reading[3].size() - 16);
	if (tmp == "00/00/0000 00:00:00")
		this->executionDate = nullptr;
	else {
		try {
			this->executionDate = new DateTime(tmp);
		}
		catch (const char* err) {
			throw err;
			//std::cout << err << std::endl;
		}
	}

	tmp = reading[4].substr(8, reading[4].size() - 8);
	if (tmp == "NOT FINISHED") {
		this->status = EntryStatus::NOT_FINISHED;
	}
	else if (tmp == "FINISHED") {
		this->status = EntryStatus::FINISHED;
	}

	this->filePath = filePath = "out/" + title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";

	std::string execDatePath = getFileNameFormatFromDate(*executionDate);

	filePath += execDatePath;
}

ReminderEntry::ReminderEntry(DateTime* execDate, std::string title, std::string description, DateTimeWorkerInterface* dtw, FileWorkerInterface* fw) {
	if (fw != nullptr) {
			fileWorker = std::move(fw);
	}

	dateCreated = new DateTime(dtw);

	try {
		checkExecDateValidity(*execDate);

		this->executionDate = new DateTime(
			execDate->getDay(), execDate->getMonth(), execDate->getYear(),
			execDate->getHours(), execDate->getMinutes(), execDate->getSeconds()
		);
	}
	catch (const char* err) {
		throw err;
	}

	
	if (title.length() > MAX_TITLE_LEN)
		this->title = title.substr(0, MAX_TITLE_LEN);
	else
		this->title = title;

	if (description.length() > MAX_DESCRIPTION_LEN) {
		this->description = description.substr(0, MAX_DESCRIPTION_LEN);
	}
	else
		this->description = description;


	filePath = "out/" + this->title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";

	std::string execDatePath = getFileNameFormatFromDate(*executionDate);
	
	filePath += execDatePath;
	writeInFile();
}

ReminderEntry::ReminderEntry(const ReminderEntry& other, FileWorkerInterface* fw) {
	//std::cout << "Copy constructor called : ";
	
	if (this->filePath == "") {
		if (fw != nullptr) {
			this->fileWorker = std::move(fw);
		}
		*this = other;
		//std::cout << "path : " << this->filePath << std::endl;
	}
	else {
		if (deleteEntry(this->filePath)) {
			*this = other;
			if (fw != nullptr) {
				this->fileWorker = std::move(fw);
			}
		}
	}
	writeInFile();
}

ReminderEntry& ReminderEntry::operator=(const ReminderEntry& other) {
	if (this != &other) {
		this->title = other.title;
		this->description = other.description;
		this->dateCreated = other.dateCreated;
		this->executionDate = other.executionDate;
		this->status = other.status;
		this->filePath = other.filePath;
	}

	return *this;
}

ReminderEntry::~ReminderEntry()
{
}

DateTime* ReminderEntry::getDateCreated()
{
	return dateCreated;
}

DateTime* ReminderEntry::getExecutionDate()
{
	return executionDate;
}

bool ReminderEntry::operator==(ReminderEntry& other)
{
	if (this->title == other.getTitle()) {
		if (this->description == other.getDescription()) {
			if (!(*this->dateCreated < *other.getDateCreated()) && !(*this->dateCreated > *other.getDateCreated())) {
				if (!(*this->executionDate < *other.getExecutionDate()) && !(*this->executionDate > *other.getExecutionDate())) {
					if (this->status == other.getEntryStatus()) {
						if (this->filePath == other.getFilePath()) {
							return true;
						}
					}
				}
				
			}
		}
	}
	return false;
}

std::string ReminderEntry::getTitle()
{
	return title;
}

std::string ReminderEntry::getDescription()
{
	return description;
}

EntryStatus ReminderEntry::getEntryStatus()
{
	return status;
}

std::string ReminderEntry::getFilePath()
{
	return filePath;
}

void ReminderEntry::setExecutionDate(DateTime& execDate)
{
	try {
		checkExecDateValidity(execDate);
	}
	catch (const char* err) {
		throw err;
	}

	deleteEntry(filePath);
	this->executionDate = &execDate;

	filePath = "out/" + title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";
	std::string execDatePath = getFileNameFormatFromDate(*executionDate);
	filePath += execDatePath;

	writeInFile();
}

void ReminderEntry::setTitle(std::string title)
{
	deleteEntry(filePath);
	if (title.length() > MAX_TITLE_LEN) {
		this->title = title.substr(0, MAX_TITLE_LEN);
	}
	else {
		this->title = title;
	}

	filePath = "out/" + this->title+ "_" + getFileNameFormatFromDate(*dateCreated) + "_";
	std::string execDatePath = getFileNameFormatFromDate(*executionDate);
	filePath += execDatePath;

	writeInFile();
}

void ReminderEntry::setDescription(std::string description)
{
	deleteEntry(filePath);
	if (description.length() > MAX_DESCRIPTION_LEN) {
		this->description = description.substr(0, MAX_DESCRIPTION_LEN);
	}
	else {
		this->description = description;
	}

	filePath = "out/" + title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";
	std::string execDatePath = getFileNameFormatFromDate(*executionDate);
	filePath += execDatePath;

	writeInFile();
}

void ReminderEntry::setStatus(EntryStatus status)
{
	deleteEntry(filePath);
	this->status = status;
	writeInFile();
}

void ReminderEntry::checkExecDateValidity(DateTime& execDate)
{
	if (execDate > *dateCreated) {
		throw ERR_MSG;
	}
	
}

std::string ReminderEntry::getFileNameFormatFromDate(DateTime& dt)
{
	//std::string date = std::to_string(dt.getDay()) + "_" + std::to_string(dt.getMonth()) + "_" + std::to_string(dt.getYear()) 
	std::string date;
	if (&dt == NULL) {
		date = "00000000_000000";
	}
	else {
		date = getCorrectFormatForDate(dt.getDay()) + getCorrectFormatForDate(dt.getMonth()) + getCorrectFormatForDate(dt.getYear()) + "_" +
			getCorrectFormatForDate(dt.getHours()) + getCorrectFormatForDate(dt.getMinutes()) + getCorrectFormatForDate(dt.getSeconds());
	}
	return date;
}

std::string ReminderEntry::getCorrectFormatForDate(int n) // Suggestion: rename function to addLeadingZeroToDate
{
	if (n >= 10) {
		return std::to_string(n);
	}
	else {
		return "0" + std::to_string(n);
	}
	//return std::string();
}

std::string ReminderEntry::getFileOutput()
{
	std::string execDate_tmp;
	if (executionDate != nullptr) {
		execDate_tmp = executionDate->getFormat(true);
	}
	else {
		execDate_tmp = "00/00/0000 00:00:00";
	}

	std::string fileOutput = "Title: " + title + "\nDescription: " + description + "\nDate created: " + dateCreated->getFormat(true) + "\nExecution date: " 
		+ execDate_tmp + "\nStatus: ";
	std::string str_status = "NOT FINISHED";
	if (status == EntryStatus::FINISHED) {
		str_status = "FINISHED";
	}

	fileOutput += str_status;


	return fileOutput;
}

void ReminderEntry::setFilePath()
{
	filePath = "out/" + title + "_" + getFileNameFormatFromDate(*dateCreated) + "_";

	std::string execDatePath;
	if (executionDate == NULL) {
		execDatePath = "00000000_000000";
	}
	else {
		execDatePath = getFileNameFormatFromDate(*dateCreated);
	}

	filePath += execDatePath;
}

void ReminderEntry::changeEntryStatus()
{
	deleteEntry(filePath);
	if (status == EntryStatus::NOT_FINISHED) {
		status = EntryStatus::FINISHED;
	}
	else {
		status = EntryStatus::NOT_FINISHED;
	}

	writeInFile();
}

void ReminderEntry::editEntry(ReminderEntry* edit)
{
	std::string _title = edit->title;
	std::string _description = edit->description;
	DateTime* _dateCreated = edit->dateCreated;
	DateTime* _execDate = edit->executionDate;
	EntryStatus _status = edit->status;
	
	//std::cout << "Stari path: " << filePath << std::endl;
	//std::cout << "Novi path: " << edit->getFilePath() << std::endl;

	if (!deleteEntry(filePath)) {
		std::cout << "Something went wrong" << std::endl;
		return;
	}
	std::string newFilePath = edit->getFilePath();

	filePath = newFilePath;

	this->title = _title;
	this->description = _description;
	this->executionDate = _execDate;
	this->status = _status;

	std::string newFileOutput = getFileOutput();

	fileWorker->writeInFile(filePath, newFileOutput);
	//writeInFile();
}

bool ReminderEntry::deleteEntry(std::string filePath)
{
	if (fileWorker->deleteFile(filePath)) {

		return true;
	}
	else
	{
		return false;
	}
}

void ReminderEntry::writeInFile() {
	fileWorker->writeInFile(filePath, getFileOutput());
}
