#pragma once

#include "../include/FileWorker.h"
#include "DateTime.h"
#include "DateTimeWorker.h"
#include "../exception/InvalidExecutionDate.h"
#include "../exception/NullObjectException.h"


#define ERR_DEFAULT_MSG "Something went wrong"


#define NULL_DATETIME_FORMAT_FILENAME "00000000_000000"
#define NULL_DATETIME_FORMAT_OUTPUT "00/00/0000 00:00:00"

#define MAX_TITLE_LEN 30
#define MAX_DESCRIPTION_LEN 1000

enum EntryStatus {
	NOT_FINISHED = false,
	FINISHED = true
};

class ReminderEntry
{
	
private:
	DateTime* dateCreated;
	DateTime* executionDate;
	std::string title;
	std::string description;
	EntryStatus status = EntryStatus::NOT_FINISHED;
	std::string filePath;

public:
	FileWorkerInterface* fileWorker = new FileWorker();

	ReminderEntry(DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(std::string title, std::string description, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(DateTime* execDate, std::string title, std::string description, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(std::vector<std::string> readingFromFile, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(const ReminderEntry& other, FileWorkerInterface* fw = nullptr);
	ReminderEntry& operator=(const ReminderEntry& other);
	bool operator==(ReminderEntry& other);

	~ReminderEntry();
	
	DateTime* getDateCreated();
	DateTime* getExecutionDate();
	std::string getTitle();
	std::string getDescription();
	EntryStatus getEntryStatus();
	std::string getFilePath();

	void setExecutionDate(DateTime& execDate);
	void setTitle(std::string title);
	void setDescription(std::string description);
	void setStatus(EntryStatus status);
	void changeEntryStatus();

	void editEntry(ReminderEntry* edit);
	bool deleteEntry(std::string filePath);
private:
	void writeInFile();
	void checkExecDateValidity(DateTime& dt);
	std::string getFileNameFormatFromDate(DateTime& dt);
	std::string addLeadingZeroToDate(int n);
public:
	std::string getFileOutput();

	void setFilePath();
};

