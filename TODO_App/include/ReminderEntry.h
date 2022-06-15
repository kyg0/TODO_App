#pragma once
#define ERR_MSG "Invalid execution date.Must be in the future"
#define MAX_TITLE_LEN 30
#define MAX_DESCRIPTION_LEN 1000
#include <fstream>
#include "../include/FileWorker.h"
#include "DateTime.h"
#include "DateTimeWorker.h";
#include <filesystem>
#include <iostream>

// Minor thing: just move 3 defines here, bellow all includes. Really liked that you used defines!

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
	//FileWorkerInterface* fileWorker = new FileWorker();
	FileWorkerInterface* fileWorker = new FileWorker();

	ReminderEntry(DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(std::string, std::string, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(DateTime*, std::string title, std::string description, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr);
	ReminderEntry(std::vector<std::string>, DateTimeWorkerInterface* dtw = new DateTimeWorker(), FileWorkerInterface* fw = nullptr); // first argument is file output
	ReminderEntry(const ReminderEntry&, FileWorkerInterface* fw = nullptr);
	ReminderEntry& operator=(const ReminderEntry&);
	bool operator==(ReminderEntry&);

	~ReminderEntry();
	
	DateTime* getDateCreated();
	DateTime* getExecutionDate();
	std::string getTitle();
	std::string getDescription();
	EntryStatus getEntryStatus();
	std::string getFilePath();

	void setExecutionDate(DateTime&);
	void setTitle(std::string);
	void setDescription(std::string);
	void setStatus(EntryStatus);
	void changeEntryStatus();

	void editEntry(ReminderEntry*);
	bool deleteEntry(std::string);
private:
	void writeInFile();
	void checkExecDateValidity(DateTime&);
	std::string getFileNameFormatFromDate(DateTime&);
	std::string getCorrectFormatForDate(int);
public:
	std::string getFileOutput();

	void setFilePath();
};

