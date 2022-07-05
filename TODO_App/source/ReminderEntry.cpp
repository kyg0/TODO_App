#include "../include/ReminderEntry.h"

ReminderEntry::ReminderEntry(DateTimeWorkerInterface* dtw, FileWorkerInterface* fw)
{
	if (fw != nullptr) {
		fileWorker = std::move(fw);
	}

	try {
		dateCreated = new DateTime(dtw);
		executionDate = NULL;
	}
	catch (DateTimeException err) {
		throw err;
	}

	title = "";
	description = "";
	filePath = "out/_" + getFileNameFormatFromDate(*dateCreated) + "_" + NULL_DATETIME_FORMAT_FILENAME;

	writeInFile();
}

ReminderEntry::ReminderEntry(std::string title, std::string description, DateTimeWorkerInterface* dtw, FileWorkerInterface* fw) {
	if (fw != nullptr) {
		fileWorker = std::move(fw);
	}

	try {
		dateCreated = new DateTime(dtw);
		executionDate = NULL;
	}
	catch (DateTimeException err) {
		throw err;
	}

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

	std::string execDatePath = NULL_DATETIME_FORMAT_FILENAME;
	
	filePath += execDatePath;
	writeInFile();
}

ReminderEntry::ReminderEntry(std::vector<std::string> readingFromFile, DateTimeWorkerInterface* dtw, FileWorkerInterface* fw){
	if (fw != nullptr) {
			fileWorker = std::move(fw);
	}

	const int numOfLines = 5;


	if (readingFromFile.size() != numOfLines) {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}
	
	if (readingFromFile[0].substr(0, 7) != "Title: ") {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}
	if (readingFromFile[1].substr(0, 13) != "Description: ") {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}
	if (readingFromFile[2].substr(0, 14) != "Date created: ") {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}
	if (readingFromFile[3].substr(0, 16) != "Execution date: ") {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}
	if (readingFromFile[4].substr(0, 8) != "Status: ") {
		throw NullObjectException(ERR_MSG_FOR_FORMAT);
	}

	
	this->title = readingFromFile[0].substr(7, readingFromFile[0].size() - 7).substr(0, MAX_TITLE_LEN);
	this->description = readingFromFile[1].substr(13, readingFromFile[1].size() - 13).substr(0,MAX_DESCRIPTION_LEN);
	
	
	
	try {
		std::string dt = readingFromFile[2].substr(14, readingFromFile[2].size() - 14);
		this->dateCreated = new DateTime(dt);

		std::string tmp = readingFromFile[3].substr(16, readingFromFile[3].size() - 16);
		if (tmp == NULL_DATETIME_FORMAT_OUTPUT)
			this->executionDate = nullptr;
		else {
			this->executionDate = new DateTime(tmp);
		}
	}
	catch (DateTimeException err) {
		throw err;
	}
	catch (std::exception err) {
		throw err;
	}

	std::string tmp = readingFromFile[4].substr(8, readingFromFile[4].size() - 8);
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
		dateCreated = new DateTime(dtw);
		checkExecDateValidity(*execDate);

		this->executionDate = new DateTime(
			execDate->getDay(), execDate->getMonth(), execDate->getYear(),
			execDate->getHours(), execDate->getMinutes(), execDate->getSeconds()
		);
	}
	catch (DateTimeException err) {
		throw err;
	}
	catch (InvalidExecutionDate err) {
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
	if (this->filePath == "") {
		if (fw != nullptr) {
			this->fileWorker = std::move(fw);
		}
		*this = other;
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
			if (*this->dateCreated == *other.getDateCreated()) {
				if (*this->executionDate == *other.getExecutionDate()) {
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
	catch (DateTimeException err) {
		throw err;
	}
	catch (InvalidExecutionDate err) {
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
		throw InvalidExecutionDate(ERR_MSG_INVALID_EXEC_DATE);
	}
}

std::string ReminderEntry::getFileNameFormatFromDate(DateTime& dt)
{
	std::string date;
	if (&dt == NULL) {
		date = NULL_DATETIME_FORMAT_FILENAME;
	}
	else {
		date = addLeadingZeroToDate(dt.getDay()) + addLeadingZeroToDate(dt.getMonth()) + addLeadingZeroToDate(dt.getYear()) + "_" +
			addLeadingZeroToDate(dt.getHours()) + addLeadingZeroToDate(dt.getMinutes()) + addLeadingZeroToDate(dt.getSeconds());
	}
	return date;
}

std::string ReminderEntry::addLeadingZeroToDate(int n) {
	if (n >= 10) {
		return std::to_string(n);
	}
	else {
		return "0" + std::to_string(n);
	}
}

std::string ReminderEntry::getFileOutput()
{
	std::string execDate_tmp;
	if (executionDate != nullptr) {
		execDate_tmp = executionDate->getFormat(true);
	}
	else {
		execDate_tmp = NULL_DATETIME_FORMAT_OUTPUT;
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
		execDatePath = NULL_DATETIME_FORMAT_FILENAME;
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

	if (!deleteEntry(filePath)) {
		throw std::exception(ERR_DEFAULT_MSG);
	}

	std::string newFilePath = edit->getFilePath();
	filePath = newFilePath;

	this->title = _title;
	this->description = _description;
	this->executionDate = _execDate;
	this->status = _status;

	std::string newFileOutput = getFileOutput();

	fileWorker->writeInFile(filePath, newFileOutput);
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
