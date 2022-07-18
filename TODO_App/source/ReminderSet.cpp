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
	StringVector fileReading;
	StringVector files = fileWorker->getAllFromDirectory(path);

	try {

		for (auto& filePath : files) {
			if (fileWorker->fileExists(filePath)) {
				fileReading = fileWorker->readFromFileInLines(filePath);
				entries.insert(MapPair(i, new ReminderEntry(fileReading, dtWorker, fileWorker)));
				i++;
			}
		}
	}
	catch (DateTimeException err) {
		throw err;
	}
	catch (InvalidIndexException err) {
		throw err;
	}
	catch (NullObjectException err) {
		throw err;
	}
	
}

ReminderEntry* ReminderSet::readFromFile(std::string path)
{
	std::string reading = fileWorker->readFromFile(path);
	return new ReminderEntry(StringVector(), fileWorker);
}

ReminderSet::~ReminderSet() 
{}

void ReminderSet::makeNewEntry(ReminderEntry* newEntry)
{
	if (newEntry == nullptr) {
		throw NullObjectException(ERR_MSG_NULL_NEW_OBJ);
	}
	try {
		for (int i = 1;; i++) {
			if (!entries.count(i)) {
				if (!fileWorker->fileExists(newEntry->getFilePath())) {
					fileWorker->writeInFile(newEntry->getFilePath(), newEntry->getFileOutput());
				}
				entries.insert(MapPair(i, newEntry));
				break;
			}
		}
	}
	catch (DateTimeException err) {
		throw err;
	}
	catch (InvalidExecutionDate err) {
		throw err;
	}

}

void ReminderSet::deleteEntry(int index)
{
	if (entries.count(index)) {
		MapIterator entry = entries.find(index);
		int key = entry->first;

		entry->second->fileWorker = this->fileWorker;
		entry->second->deleteEntry(entry->second->getFilePath());
		entries.erase(key);
	}
	else {
		throw InvalidIndexException(ERR_MSG_INVALID_INDEX);
	}
}

void ReminderSet::deleteEntry(ReminderEntry* entryToDelete)
{
	if (entryToDelete == nullptr) {
		throw NullObjectException(ERR_MSG_NULL_OBJ);
	}
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		if (*entryToDelete == *it->second) {
			it->second->fileWorker = fileWorker;
			it->second->deleteEntry(it->second->getFilePath());
			entries.erase(it);
			return;
		}
	}
	throw InvalidIndexException(ERR_MSG_INVALID_OBJECT);
}

void ReminderSet::editEntry(ReminderEntry* old, ReminderEntry* edit)
{
	if (old == nullptr){
		throw NullObjectException(ERR_MSG_NULL_OBJ);
	}
	if (edit == nullptr) {
		throw NullObjectException(ERR_MSG_NULL_NEW_OBJ);
	}
	
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		if (*old == *it->second) {
			it->second->editEntry(edit);
		}
	}
}

void ReminderSet::editEntry(int index, ReminderEntry* edit)
{
	if (edit == nullptr) {
		throw NullObjectException(ERR_MSG_NULL_OBJ);
	}
	if (entries.find(index) == entries.end()) {
		throw InvalidIndexException(ERR_MSG_INVALID_INDEX);
	}
	MapIterator it;
	it = entries.find(index);
	it->second->editEntry(edit);
}

SetMap ReminderSet::getAll()
{
	return entries;
}

SetMap ReminderSet::filterByDateCreated(FilterMode filterMode,DateTime* filterDate, bool descending)
{
	std::vector<ReminderEntry*> vector;
	SetMap filtered;
	MapIterator it;

	switch (filterMode) {
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
		return SetMap();
	}
	if (vector.size() == 1) {
		filtered.insert(MapPair(1, vector[0]));
		return filtered;
	}

	

	for (int i = 0; i < vector.size() - 1; i++) {
		int toSwapId= i;

		for (int j = i + 1; j < vector.size(); j++) {
			if (descending && *vector[j]->getDateCreated() > *vector[toSwapId]->getDateCreated()) {
				toSwapId = j;
			}
			else if (!descending && *vector[j]->getDateCreated() < *vector[toSwapId]->getDateCreated()) {
				toSwapId = j;
			}
		}

		std::swap(vector[toSwapId], vector[i]);
	}

	for (int i = 0; i < vector.size(); i++) {
		filtered.insert(MapPair(i + 1, vector[i]));
	}

	return filtered;
}

SetMap ReminderSet::filterByExecDate(FilterMode filterMode, DateTime* filterDate, bool descending){
	std::vector<ReminderEntry*> vector;
	SetMap filtered;
	MapIterator it;

	switch (filterMode) {
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
		return SetMap();
	}
	if (vector.size() == 1) {
		filtered.insert(MapPair(1, vector[0]));
		return filtered;
	}



	for (int i = 0; i < vector.size() - 1; i++) {
		int toSwapId = i;

		for (int j = i + 1; j < vector.size(); j++) {
			if (descending && *vector[j]->getExecutionDate() > *vector[toSwapId]->getExecutionDate()) {
				toSwapId = j;
			}
			else if (!descending && *vector[j]->getExecutionDate() < *vector[toSwapId]->getExecutionDate()) {
				toSwapId = j;
			}
		}

		std::swap(vector[toSwapId], vector[i]);
	}

	for (int i = 0; i < vector.size(); i++) {
		filtered.insert(MapPair(i + 1, vector[i]));
	}

	return filtered;
}

SetMap ReminderSet::filterByStatus(EntryStatus status)
{
	SetMap filtered;
	MapIterator it;
	int i = 1;

	for (it = entries.begin(); it != entries.end(); it++) {
		if (it->second->getEntryStatus() == status) {
			filtered.insert(MapPair(i, it->second));
			i++;
		}
	}

	return filtered;
}

SetMap ReminderSet::sortByDateCreated(bool descending)
{
	std::vector<ReminderEntry*> vector;
	SetMap sorted;
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vector.push_back(it->second);
	}


	if (vector.size() == 0) {
		return SetMap();
	}
	if (vector.size() == 1) {
		sorted.insert(MapPair(1, vector[0]));
	}

	for (int i = 0; i < vector.size() - 1; i++) {
		int toSwapId = i;

		for (int j = i + 1; j < vector.size(); j++) {
			if (descending && *vector[j]->getDateCreated() > *vector[toSwapId]->getDateCreated()) {
				toSwapId = j;
			}
			else if (!descending && *vector[j]->getDateCreated() < *vector[toSwapId]->getDateCreated()) {
				toSwapId = j;
			}
		}

		std::swap(vector[toSwapId], vector[i]);
	}

	for (int i = 0; i < vector.size(); i++) {
		sorted.insert(MapPair(i + 1, vector[i]));
	}

	return sorted;
}

SetMap ReminderSet::sortByExecDate(bool descending)
{
	std::vector<ReminderEntry*> vector;
	SetMap sorted;
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vector.push_back(it->second);
	}


	if (vector.size() == 0) {
		return SetMap();
	}
	if (vector.size() == 1) {
		sorted.insert(MapPair(1, vector[0]));
	}

	for (int i = 0; i < vector.size() - 1; i++) {
		int toSwapId = i;

		for (int j = i + 1; j < vector.size(); j++) {
			if (descending && *vector[j]->getExecutionDate() > *vector[toSwapId]->getExecutionDate()) {
				toSwapId = j;
			}
			else if (!descending && *vector[j]->getExecutionDate() < *vector[toSwapId]->getExecutionDate()) {
				toSwapId = j;
			}
		}

		std::swap(vector[toSwapId], vector[i]);
	}

	for (int i = 0; i < vector.size(); i++) {
		sorted.insert(MapPair(i + 1, vector[i]));
	}

	return sorted;
}

SetMap ReminderSet::sortByStatus(bool finishedFirst)
{
	std::vector<ReminderEntry*> vec;
	SetMap sorted;
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return SetMap();
	}
	if (vec.size() == 1) {
		sorted.insert(MapPair(1, vec[0]));
		return sorted;
	}

	if (finishedFirst) {
		int i = 1;
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::FINISHED) {
				sorted.insert(MapPair(i, x));
				i++;
			}
		}
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				sorted.insert(MapPair(i, x));
				i++;
			}
		}
	}
	else {
		int i = 1;
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::NOT_FINISHED) {
				sorted.insert(MapPair(i, x));
				i++;
			}
		}
		for (ReminderEntry* x : vec) {
			if (x->getEntryStatus() == EntryStatus::FINISHED) {
				sorted.insert(MapPair(i, x));
				i++;
			}
		}
	}

	return sorted;
}

SetMap ReminderSet::sortByTitle(bool descending)
{
	std::vector<ReminderEntry*> vec;
	SetMap sorted;
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return SetMap();
	}
	if (vec.size() == 1) {
		sorted.insert(MapPair(1, vec[0]));
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
			sorted.insert(MapPair(i + 1, vec[i]));
		}
	}
	else {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(MapPair(i + 1, vec[vec.size() - 1 - i]));
		}
	}

	return sorted;
}

SetMap ReminderSet::sortByDescription(bool descending)
{
	std::vector<ReminderEntry*> vec;
	SetMap sorted;
	MapIterator it;
	for (it = entries.begin(); it != entries.end(); it++) {
		vec.push_back(it->second);
	}

	if (vec.size() == 0) {
		return SetMap();
	}
	if (vec.size() == 1) {
		sorted.insert(MapPair(1, vec[0]));
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
			sorted.insert(MapPair(i + 1, vec[i]));
		}
	}
	else {
		for (int i = 0; i < vec.size(); i++) {
			sorted.insert(MapPair(i + 1, vec[vec.size() - 1 - i]));
		}
	}

	return sorted;
}
