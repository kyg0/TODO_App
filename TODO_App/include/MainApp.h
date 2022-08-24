#pragma once
#include <Windows.h>
#include <../ReminderSet.h>
#include "../exception/include/InvalidCommandException.h"

class MainApp
{
protected:
	static MainApp* instance;
	ReminderSet* set;
	MainApp();
public:
	MainApp(MainApp&) = delete;
	void operator= (const MainApp&) = delete;
	static MainApp* getInstance();
	ReminderSet* getSet();

	void runApp();
private:
	void printEntry(int, ReminderEntry&);
	void printSet(SetMap);

	void execCommand(std::string command); 
	void execCommand_Help(std::string flag = std::string());
	void execCommand_Get(std::string);
	void execCommand_Get();
	void execCommand_New(StringVector);
	void execCommand_Delete(int);
	void execCommand_Sort(std::string, bool descending = true);
	void deriveAndExecCommand_Filter(StringVector);
	void execCommand_Filter(std::string, FilterMode, DateTime*, bool descending = true);
	void execCommand_Filter(EntryStatus);
	void execCommand_Edit(int);
	void execCommand_Exit();

	void printStartScreen();
	void printEnterCommandScreen();
	void printWelcomeMessage();
	void printMakeNewEntry();
	void printEditEntry(int, ReminderEntry*);

	StringVector deriveCommand(std::string command);
	void makeNewEntry(std::string, std::string, std::string);
};