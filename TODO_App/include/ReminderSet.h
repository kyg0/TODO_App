#pragma once
#include "ReminderEntry.h" // Move this user-defined include bellow system includes
#include <map>
#include <vector>
#include <filesystem>

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

	void makeNewEntry(ReminderEntry*);
	void deleteEntry(int);
	void deleteEntry(ReminderEntry*);
	void editEntry(ReminderEntry*, ReminderEntry*);
	void editEntry(int, ReminderEntry*);

	std::map<int, ReminderEntry*> getAll();
	std::map<int, ReminderEntry*> filterByDateCreated(FilterMode, DateTime*, bool descending = true); // true=descending false=ascending	
	std::map<int, ReminderEntry*> filterByExecDate(FilterMode, DateTime*, bool descending = true);
	std::map<int, ReminderEntry*> filterByStatus(EntryStatus status = EntryStatus::NOT_FINISHED);
	

	std::map<int, ReminderEntry*> sortByDateCreated(bool descending = true);
	std::map<int, ReminderEntry*> sortByStatus(bool finishedFirst = true); //true=>FINISHED, NOT false=>NOT, FINISHED
	std::map<int, ReminderEntry*> sortByExecDate(bool descending = true);
	std::map<int, ReminderEntry*> sortByTitle(bool descending = true);
	std::map<int, ReminderEntry*> sortByDescription(bool descending = true);

	ReminderEntry* readFromFile(std::string);
};




