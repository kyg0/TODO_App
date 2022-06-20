#pragma once
#include <map>
#include "ReminderEntry.h"
#include "../exception/InvalidIndexException.h"
#include "../exception/NullObjectException.h"

namespace fs = std::filesystem;

enum FilterMode {
	BeforeDate = 0,
	AfterDate = 1
};

class ReminderSet
{
private:
	std::map<int, ReminderEntry*> entries;
public:
	FileWorkerInterface* fileWorker = new FileWorker(); // Q: Why initialize it here instead of in constructor?
	DateTimeWorkerInterface* dtWorker = new DateTimeWorker();

	ReminderSet(FileWorkerInterface* fw = nullptr, DateTimeWorkerInterface* dtw = nullptr);
	~ReminderSet();

	void makeNewEntry(ReminderEntry* newEntry);
	void deleteEntry(int index);
	void deleteEntry(ReminderEntry* entryToDelete);
	void editEntry(ReminderEntry* old, ReminderEntry* edit);
	void editEntry(int index , ReminderEntry* edit);

	std::map<int, ReminderEntry*> getAll();
	std::map<int, ReminderEntry*> filterByDateCreated(FilterMode filterMode, DateTime* filterDate, bool descending = true); // true=descending false=ascending	
	std::map<int, ReminderEntry*> filterByExecDate(FilterMode filterMode, DateTime* filterDate, bool descending = true);
	std::map<int, ReminderEntry*> filterByStatus(EntryStatus status = EntryStatus::NOT_FINISHED);

	std::map<int, ReminderEntry*> sortByDateCreated(bool descending = true);
	std::map<int, ReminderEntry*> sortByStatus(bool finishedFirst = true); //true=>FINISHED, NOT false=>NOT, FINISHED
	std::map<int, ReminderEntry*> sortByExecDate(bool descending = true);
	std::map<int, ReminderEntry*> sortByTitle(bool descending = true);
	std::map<int, ReminderEntry*> sortByDescription(bool descending = true);

	ReminderEntry* readFromFile(std::string path);
};




