#pragma once
#include <map>
#include "ReminderEntry.h"
#include "../exception/include/InvalidIndexException.h"
#include "../exception/include/NullObjectException.h"

namespace fs = std::filesystem;
using SetMap = std::map<int, ReminderEntry*>;
using MapIterator = SetMap::iterator;
using MapPair = std::pair<int, ReminderEntry*>;


enum FilterMode {
	BeforeDate = 0,
	AfterDate = 1
};

class ReminderSet
{
private:
	SetMap entries;
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

	SetMap getAll();
	SetMap filterByDateCreated(FilterMode filterMode, DateTime* filterDate, bool descending = true); // true=descending false=ascending	
	SetMap filterByExecDate(FilterMode filterMode, DateTime* filterDate, bool descending = true);
	SetMap filterByStatus(EntryStatus status = EntryStatus::NOT_FINISHED);

	SetMap sortByDateCreated(bool descending = true);
	SetMap sortByStatus(bool finishedFirst = true); //true=>FINISHED, NOT false=>NOT, FINISHED
	SetMap sortByExecDate(bool descending = true);
	SetMap sortByTitle(bool descending = true);
	SetMap sortByDescription(bool descending = true);

	ReminderEntry* readFromFile(std::string path);
};