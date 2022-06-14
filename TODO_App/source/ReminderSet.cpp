#include "../include/ReminderSet.h"

ReminderSet::ReminderSet(FileWorkerInterface* fw, DateTimeWorkerInterface* dtw)
{
	if (dtw != nullptr) {
		this->dtWorker = std::move(dtw);
	}
	if (fw != nullptr) {
		this->fileWorker = std::move(fw);
	}

	int i = 1;

	std::string path = "out/";

	std::vector<std::string> fileReading;
	
	std::vector<std::string> files = fileWorker->getAllFromDirectory(path);

	for (auto& filePath : files) {
		

		if (fileWorker->exists(filePath)) {
			fileReading = fileWorker->readFromFileInLines(filePath);

			//TODO
			entries.insert(std::pair<int, ReminderEntry*>(i, new ReminderEntry(fileReading, dtWorker, fileWorker)));

			i++;
		}
	}
	
}

ReminderEntry* ReminderSet::readFromFile(std::string path)
{
	//ReminderEntry* e = new ReminderEntry();
	std::string reading = fileWorker->readFromFile(path);

	//TODO
	return new ReminderEntry(std::vector<std::string>(), fileWorker);
}

ReminderSet::~ReminderSet() {
	//std::cout << "DESTRUKTOR ~ReminderSet()" << std::endl;
}

void ReminderSet::makeNewEntry(ReminderEntry* newEntry)
{
	if (newEntry == nullptr) {
		return;
	}
	for (int i = 1;; i++) {
		if (!entries.count(i)) {
			if (!fileWorker->exists(newEntry->getFilePath())) {
				fileWorker->writeInFile(newEntry->getFilePath(), newEntry->getFileOutput());
			}
			entries.insert(std::pair<int, ReminderEntry*>(i, newEntry));
			break;
		}
	}

}

void ReminderSet::deleteEntry(int index)
{
	if (entries.count(index)) {
		std::map<int, ReminderEntry*>::iterator entry = entries.find(index);
		int key = entry->first;

		entry->second->fileWorker = this->fileWorker;
		entry->second->deleteEntry(entry->second->getFilePath());
		entries.erase(entry);
	}
	else {
		throw "Invalid index";
	}
}

void ReminderSet::deleteEntry(ReminderEntry* entry)
{
	if (entry == nullptr) {
		return;
	}
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		if (*entry == *it->second) {
			//it->second->setFileWorker(fileWorker);
			it->second->fileWorker = fileWorker;
			it->second->deleteEntry(it->second->getFilePath());
			entries.erase(it);
			return;
		}
	}
}

void ReminderSet::editEntry(ReminderEntry* old, ReminderEntry* edit)
{
	if (old == nullptr || edit == nullptr) {
		return;
	}
	
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		if (*old == *it->second) {
			it->second->editEntry(edit);
		}
	}
}

void ReminderSet::editEntry(int index, ReminderEntry* edit)
{
	if (edit == nullptr) {
		throw "New object is null";
	}
	if (!entries.count(index)) {
		throw "Invalid index";
	}
	std::map<int, ReminderEntry*>::iterator it;
	it = entries.find(index);
	it->second->editEntry(edit);
}

std::map<int, ReminderEntry*> ReminderSet::getAll()
{
	return entries;
}

std::map<int, ReminderEntry*> ReminderSet::filterByDateCreated(FilterMode filter,DateTime* filterDate, bool descending)
{
	std::vector<ReminderEntry*> vector;
	std::map<int, ReminderEntry*> filtered;
	std::map<int, ReminderEntry*>::iterator it;

	switch (filter) {
	case FilterMode::BeforeDate:
		for (it = entries.begin(); it != entries.end(); it++) {
			if (( * it->second->getDateCreated() > *filterDate) || (*it->second->getDateCreated() == *filterDate)) {
				vector.push_back(it->second);
			}
		}
		break;
	case FilterMode::AfterDate:
		for (it = entries.begin(); it != entries.end(); it++) {
			if (*it->second->getDateCreated() < *filterDate) {
				vector.push_back(it->second);
			}
		}
		break;
	}
	
	if (vector.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vector.size() == 1) {
		filtered.insert(std::pair<int, ReminderEntry*>(1, vector[0]));
	}

	for (int i = 1; i < vector.size(); i++) {
		ReminderEntry* key = vector[i];
		int j = i - 1;

		while (j >= 0 && *vector[j]->getExecutionDate() > *key->getExecutionDate()) {
			vector[j + 1] = vector[j];
			j = j - 1;
		}
		vector[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vector.size(); i++) {
			filtered.insert(std::pair<int, ReminderEntry*>(i + 1, vector[i]));
		}
	}
	else {
		for (int i = 0; i < vector.size(); i++) {
			filtered.insert(std::pair<int, ReminderEntry*>(i + 1, vector[vector.size() - 1 - i]));
		}
	}
	
	return filtered;
}

std::map<int, ReminderEntry*> ReminderSet::filterByExecDate(FilterMode filter, DateTime* filterDate, bool descending){
	std::vector<ReminderEntry*> vector;
	std::map<int, ReminderEntry*> filtered;
	std::map<int, ReminderEntry*>::iterator it;

	switch (filter) {
	case FilterMode::BeforeDate:
		for (it = entries.begin(); it != entries.end(); it++) {
			if ((*it->second->getExecutionDate() > *filterDate) || (*it->second->getExecutionDate() == *filterDate)) {
				vector.push_back(it->second);
			}
		}
		break;
	case FilterMode::AfterDate:
		for (it = entries.begin(); it != entries.end(); it++) {
			if (*it->second->getExecutionDate() < *filterDate) {
				vector.push_back(it->second);
			}
		}
		break;
	}

	if (vector.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vector.size() == 1) {
		filtered.insert(std::pair<int, ReminderEntry*>(1, vector[0]));
	}

	for (int i = 1; i < vector.size(); i++) {
		ReminderEntry* key = vector[i];
		int j = i - 1;

		while (j >= 0 && *vector[j]->getExecutionDate() > *key->getExecutionDate()) {
			vector[j + 1] = vector[j];
			j = j - 1;
		}
		vector[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vector.size(); i++) {
			filtered.insert(std::pair<int, ReminderEntry*>(i + 1, vector[i]));
		}
	}
	else {
		for (int i = 0; i < vector.size(); i++) {
			filtered.insert(std::pair<int, ReminderEntry*>(i + 1, vector[vector.size() - 1 - i]));
		}
	}

	return filtered;
}

std::map<int, ReminderEntry*> ReminderSet::filterByStatus(EntryStatus status)
{
	std::map<int, ReminderEntry*> filtered;
	std::map<int, ReminderEntry*>::iterator it;
	int i = 0;

	for (it = entries.begin(); it != entries.end(); it++) {
		if (it->second->getEntryStatus() == status) {
			filtered.insert(std::pair<int, ReminderEntry*>(i, it->second));
			i++;
		}
	}

	return filtered;
}

std::map<int, ReminderEntry*> ReminderSet::sortByDateCreated(bool descending)
{
	std::vector<ReminderEntry*> vector;
	std::map<int, ReminderEntry*> sorted;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vector.push_back(it->second);
	}


	if (vector.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vector.size() == 1) {
		sorted.insert(std::pair<int, ReminderEntry*>(1, vector[0]));
	}

	for (int i = 1; i < vector.size(); i++) {
		ReminderEntry* key = vector[i];
		int j = i - 1;

		while (j >= 0 && *vector[j]->getDateCreated() > *key->getDateCreated()) {
			vector[j + 1] = vector[j];
			j = j - 1;
		}
		vector[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vector.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vector[i]));
		}
	}
	else {
		for (int i = 0; i < vector.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vector[vector.size() - 1 - i]));
		}
	}

	return sorted;
}

std::map<int, ReminderEntry*> ReminderSet::sortByExecDate(bool descending)
{
	std::vector<ReminderEntry*> vector;
	std::map<int, ReminderEntry*> sorted;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vector.push_back(it->second);
	}


	if (vector.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vector.size() == 1) {
		sorted.insert(std::pair<int, ReminderEntry*>(1, vector[0]));
	}

	for (int i = 1; i < vector.size(); i++) {
		ReminderEntry* key = vector[i];
		int j = i - 1;

		while (j >= 0 && *vector[j]->getExecutionDate() > *key->getExecutionDate()) {
			vector[j + 1] = vector[j];
			j = j - 1;
		}
		vector[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vector.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vector[i]));
		}
	}
	else {
		for (int i = 0; i < vector.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vector[vector.size() - 1 - i]));
		}
	}

	return sorted;
}

std::map<int, ReminderEntry*> ReminderSet::sortByStatus(bool finishedFirst)
{
	std::vector<ReminderEntry*> vec;
	std::map<int, ReminderEntry*> sorted;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vec.size() == 1) {
		sorted.insert(std::pair<int, ReminderEntry*>(1, vec[0]));
		return sorted;
	}

	if (finishedFirst) {
		int i = 1;
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::FINISHED) {
				sorted.insert(std::pair<int, ReminderEntry*>(i, x));
				i++;
			}
		}
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				sorted.insert(std::pair<int, ReminderEntry*>(i, x));
				i++;
			}
		}
	}
	else {
		int i = 1;
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				sorted.insert(std::pair<int, ReminderEntry*>(i, x));
				i++;
			}
		}
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::FINISHED) {
				sorted.insert(std::pair<int, ReminderEntry*>(i, x));
				i++;
			}
		}
	}

	return sorted;
}

std::map<int, ReminderEntry*> ReminderSet::sortByTitle(bool descending)
{
	std::vector<ReminderEntry*> vec;
	std::map<int, ReminderEntry*> sorted;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vec.size() == 1) {
		sorted.insert(std::pair<int, ReminderEntry*>(1, vec[0]));
	}

	for (int i = 1; i < vec.size(); i++) {
		ReminderEntry* key = vec[i];
		int j = i - 1;

		while (j >= 0  && vec[j]->getTitle() < key->getTitle()) {
			vec[j + 1] = vec[j];
			j = j - 1;
		}
		vec[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vec[i]));
		}
	}
	else {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vec[vec.size() - 1 - i]));
		}
	}

	return sorted;
}

std::map<int, ReminderEntry*> ReminderSet::sortByDescription(bool descending)
{
	std::vector<ReminderEntry*> vec;
	std::map<int, ReminderEntry*> sorted;
	std::map<int, ReminderEntry*>::iterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return std::map<int, ReminderEntry*>();
	}
	if (vec.size() == 1) {
		sorted.insert(std::pair<int, ReminderEntry*>(1, vec[0]));
	}

	for (int i = 1; i < vec.size(); i++) {
		ReminderEntry* key = vec[i];
		int j = i - 1;

		while (j >= 0 && vec[j]->getDescription() < key->getDescription()) {
			vec[j + 1] = vec[j];
			j = j - 1;
		}
		vec[j + 1] = key;
	}

	if (descending) {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vec[i]));
		}
	}
	else {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(std::pair<int, ReminderEntry*>(i + 1, vec[vec.size() - 1 - i]));
		}
	}

	return sorted;
}
