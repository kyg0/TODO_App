#pragma once
#include <../ReminderSet.h>

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
	void updateSet();

	void runApp();
private:
	void printEntry(int, ReminderEntry&); //ostaje
	void printSet(std::map<int, ReminderEntry*>); // prepraviti => bice printSet()

	void execCommand(std::string command); //
	void execCommand_Help(std::string flag = std::string());
	void execCommand_Get(std::string);
	void execCommand_Get();
	void execCommand_New(std::vector<std::string>);
	void execCommand_Delete(int);
	void execCommand_Sort(std::string, bool descending = true);
	void execCommand_Filter(std::string, FilterMode, DateTime*, bool descending = true);
	void execCommand_Filter(EntryStatus);
	void execCommand_Edit(int);

	void deriveAndExecCommand_Filter(std::vector<std::string>);


	void printStartScreen();
	void printEnterCommandScreen();
	void printWelcomeMessage();
	void printMakeNewEntry();
	void printEditEntry(int, ReminderEntry*);
	
	void clearScreen();

	std::vector<std::string> deriveCommand(std::string command);
	void makeNewEntry(std::string, std::string, std::string);
};

