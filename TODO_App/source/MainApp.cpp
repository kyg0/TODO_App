#include "../include/MainApp.h"

MainApp::MainApp()
{
	set = new ReminderSet();
}

MainApp* MainApp::getInstance()
{
	if (instance == nullptr) {
		instance = new MainApp();
	}
	return instance;
}

ReminderSet* MainApp::getSet()
{
	return set;
}

void MainApp::updateSet() {
	//TODO
}

void MainApp::execCommand(std::string command)
{
	std::vector<std::string> cmd_split = deriveCommand(command);
	
	std::string cmd = cmd_split[0];
	for (int i = 0; i < cmd.size(); i++) {
		cmd[i] = ::tolower(cmd[i]);
	}

	if (cmd == "new") {
		execCommand_New(cmd_split);
	}
	else if (cmd == "get") {
		if (cmd_split.size() > 1) {
			std::cout << std::endl << "  Too many arguments" << std::endl;
			return;
		}
		execCommand_Get();
	}
	else if (cmd == "delete") {
		if (cmd_split.size() > 2) {
			std::cout << std::endl << "  Too many arguments." << std::endl;
		}
		execCommand_Delete(std::stoi(cmd_split[1]));
	}
	else if (cmd == "sort") {
		if (cmd_split.size() > 3) {
			std::cout << std::endl << "Too many arguments" << std::endl;
			return;
		}
		bool descending = true;
		if (cmd_split.size() == 2) {
			execCommand_Sort(cmd_split[1]);
		}
		else if (cmd_split.size() == 3) {
			if (cmd_split[1] == "-des" || cmd_split[1] == "-1") {
				descending = true;
				execCommand_Sort(cmd_split[2], descending);
			}
			else if (cmd_split[1] == "-asc" || cmd_split[1] == "-0") {
				descending = false;
				execCommand_Sort(cmd_split[2], descending);
			}
			else if (cmd_split[2] == "-asc" || cmd_split[2] == "-0") {
				descending = false;
				execCommand_Sort(cmd_split[1], descending);
			}
			else if (cmd_split[2] == "-des" || cmd_split[2] == "-1") {
				descending = true;
				execCommand_Sort(cmd_split[1], descending);
			}
			else {
				std::cout << std::endl << "  Wrong set of arguments" << std::endl;
			}
		}
	}

	else if (cmd == "filter") {
		deriveAndExecCommand_Filter(cmd_split);
	}
	else if (cmd == "edit") {
		if (cmd_split.size() != 2) {
			std::cout << "  Wrong use of this command." << std::endl;
			return;
		}
		execCommand_Edit(std::stoi(cmd_split[1]));
	}
	else if (cmd == "help") {
		if (cmd_split.size() == 1) {
			execCommand_Help();
		}
		else if (cmd_split.size() == 2) {
			execCommand_Help(cmd_split[1]);
		}
		else {
			std::cout << "  Wrong usage of command <help>." << std::endl;
			std::cout << "  Try  'help' or 'help -[command]'" << std::endl;
			std::cout << std::endl;
			return;
		}
	}
	else if (cmd == "close" || cmd == "end" || cmd == "exit" || cmd == "quit" || cmd == "close") {
		system("cls");

		std::cout << "Program closed...";
		for (int i = 0; i < 5; i++) {
			std::cout << std::endl;
		}

		exit(0);
	}
	else if (cmd == "cls" || cmd == "clear") {
		if (cmd_split.size() > 1) {
			std::cout << std::endl << "Too many arguments for this command" << std::endl;
			return;
		}
		system("cls");
	}
	else
	{
		if (cmd_split[0][0] != '-') {
			std::cout << std::endl << "  <" << cmd_split[0] << "> is invalid command" << std::endl;
		}
		else {
			std::string str = "";
			for (std::string x : cmd_split) {
				str += " " + x;
			}

			std::cout << std::endl << "  " << str << " invalid set of commands" << std::endl;
			
		}
	}
}

void MainApp::deriveAndExecCommand_Filter(std::vector<std::string> command) {
	std::string attFilter = "";
	FilterMode* filterMode = nullptr;
	DateTime* filterDate = nullptr;
	bool descending = true;

	std::string tmp_date;

	if (command.size() > 8) {
		std::cout << std::endl << "  Too many arguments" << std::endl;
		return;
	}
	else if (command.size() < 2) {
		std::cout << std::endl << "  Too few arguments" << std::endl;
		return;
	}
	else if (command.size() == 3 || command.size() == 2) {
		bool finished = false;
		bool isCorrectArg = false;
		for (int i = 1; i < command.size(); i++) {
			if (command[i] == "-s") {
				isCorrectArg = true;
			}
			else if (command[i] == "-1" || command[i] == "-0") {
				if (command[i] == "-1")
					finished = true;
				else
					finished = false;
			}
		}
		if (isCorrectArg) {
			if (finished)
				execCommand_Filter(EntryStatus::FINISHED);
			else
				execCommand_Filter(EntryStatus::NOT_FINISHED);
			return;
		}
		else {
			std::cout << "  Wrong use of command" << std::endl;
			return;
		}
	}
	for (int i = 1; i < command.size(); i++) {
		if (command[i] == "-ed" || command[i] == "-dc") {
			attFilter = command[i];
		}
		else if (command[i] == "-mode") {
			if (i + 1 >= command.size()) {
				std::cout << std::endl << "  You have to define argument for -mode" << std::endl;
				std::cout << "  '-mode [1|0]' or '-mode [after|before]" << std::endl;
				return;
			}
			else if (command[i + 1] == "before" || command[i + 1] == "0") {
				filterMode = new FilterMode(FilterMode::BeforeDate);
				i++;
			}
			else if (command[i + 1] == "after" || command[i + 1] == "1") {
				filterMode = new FilterMode(FilterMode::AfterDate);
				i++;
			}
			else {
				std::cout << std::endl << "  You have to define argument for -mode" << std::endl;
				std::cout << "  '-mode [1|0]' or '-mode [after|before]" << std::endl;
				return;
			}
		}
		else if (command[i] == "-date") {
			if (i + 1 < command.size()) {
				tmp_date = command[i + 1];
			}
			else {
				std::cout << std::endl << "  You have to define date and time for -date" << std::endl;
				std::cout << "  'Format: -date <date> <time>' or '-date \"date and time\"'" << std::endl;
				return;
			}
			if (i + 2 < command.size()) {
				if (command[i + 2] != "-mode" && command[i + 2] != "-asc" && command[i + 2] != "-des" && command[i + 2] != "-ed" && command[i + 2] != "-dc") {
					try {
						tmp_date = tmp_date + " " + command[i + 2];
						filterDate = new DateTime(tmp_date);
					}
					catch (const char* err) {
						std::cout << std::endl << "Something went wrong: " << err << std::endl;
					}
				}
				else {
					try {
						filterDate = new DateTime(tmp_date);
					}
					catch (const char* err) {
						std::cout << std::endl << "Something went wrong: " << err << std::endl;
					}
				}
			}
			else {
				try {
					filterDate = new DateTime(tmp_date);
				}
				catch (const char* err) {
					std::cout << std::endl << "Something went wrong: " << err << std::endl;
				}
			}
		}
		else if (command[i] == "-asc" || command[i] == "-des" || command[i] == "-0"  || command[i] == "-1") {
			if (command[i] == "-asc" || command[i] == "-0")
				descending = false;
			else
				descending = true;
		}
	}

	if (attFilter != "" && filterMode != nullptr && filterDate != nullptr) {
		execCommand_Filter(attFilter, *filterMode, filterDate, descending);
	}
	else {
		std::cout << std::endl << "  Too few arguments" << std::endl;
	}
}

std::vector<std::string> MainApp::deriveCommand(std::string command) {
	if (command == "") {
		system("cls");
		printStartScreen();
	}

	std::vector<std::string> cmd_split;

	int pos = 0;
	int split_size = 0;

	for (int i = 0; i < command.size(); i++) {
		if (command[i] == '\"') {
			i++;
			std::string tmp_arg = "";
			while (command[i] != '\"' && i < command.size()) {
				tmp_arg += command[i];
				i++;
			}
			std::cout << std::endl;
			cmd_split.push_back(tmp_arg);
			if (i + 1 < command.size() && command[i+1] == ' ') {
				while (command[i+1] == ' ') {
					i++;
					pos = i + 1;
				}
			}
		}
		else {
			if (command[i] == ' ') {
				cmd_split.push_back(command.substr(pos, split_size));
				pos = i + 1;
				split_size = 0;
			}
			else if (i == command.size() - 1) {
				cmd_split.push_back(command.substr(pos, split_size + 1));
			}
			else {
				split_size++;
			}
		}
	}

	return cmd_split;
}

void MainApp::printSet(std::map<int, ReminderEntry*> printSet)
{
	std::map<int, ReminderEntry*>::iterator it;


	std::cout << "========================================================================================" << std::endl;
	std::cout << "=====                                 YOUR REMINDERS                               =====" << std::endl;
	std::cout << "========================================================================================" << std::endl;
	std::cout << "= No. |             Title                |    Date created      |    Reminder date     =" << std::endl;
	std::cout << "========================================================================================" << std::endl;
	
	for (it = printSet.begin(); it != printSet.end(); it++) {
		printEntry(it->first, *it->second);
		std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==" << std::endl;
	}
}

void MainApp::printEntry(int index, ReminderEntry& entry)
{
	int title_length = entry.getTitle().size();
	int description_length = entry.getDescription().size();

	std::string _number;
	if (index < 10) {
		_number = "  " + std::to_string(index);
	}
	else if (index < 100) {
		_number = " " + std::to_string(index);
	}
	else {
		_number = std::to_string(index);
	}

	std::string _title;
	for (int i = 0; i < entry.getTitle().size(); i++) {
		_title += entry.getTitle()[i];
	}

	for (int i = _title.size() - 1; i < MAX_TITLE_LEN; i++) {
		_title += " ";
	}

	std::string _dateCreated = " 00/00/0000 00:00:00  ";
	if (entry.getDateCreated() != nullptr) {
		_dateCreated = " " + entry.getDateCreated()->getFormat(true) + "  ";
	}
	//std::string _dateCreated = " " + entry.getDateCreated()->getFormat(true) + "  ";
	std::string _execDate = "   00/00/0000 00:00   ";
	if (entry.getExecutionDate() != nullptr) {
		_execDate = "   " + entry.getExecutionDate()->getFormat(false) + "   ";
	}

	std::cout << "= " << _number << "   " << _title << "  " << _dateCreated << " " << _execDate << " =" << std::endl;
	std::cout << "=                                                                                      =" << std::endl;

	std::string _description = entry.getDescription();
	std::string emptyRow;
	std::string _row;
	for (int i = 0; i < 88; i++) {
		if (i == 0 || i == 87)
			emptyRow += "=";
		else
			emptyRow += " ";
	}
	_row = emptyRow;

	int rowIndex = 2; // row = "=_ ... _="   _ is space
	//max{rowIndex} = _row.size() - 2 - 1 = 85
	for (int i = 0; i < _description.size(); i++) {
		if (rowIndex != 2 && rowIndex % 85 == 0) {
			if (_description[i + 1] == ' ') {
				i++;
				while (_description[i] == ' ') {
					i++;
				}
				std::cout << _row << std::endl;
				rowIndex = 2;
				_row = emptyRow;
				//_row[rowIndex++] = _description[i]; 
			}
			else if(_description[i+1] != ' ' && _description[i]!=' ') {
				_row[rowIndex++] = '-';
				std::cout << _row << std::endl;
				_row = emptyRow;
				rowIndex = 2;
				_row[rowIndex++] = _description[i];
			}
			else if (_description[i] == ' ') {
				while (_description[i] == ' ') {
					i++;
				}
				std::cout << _row << std::endl;
				_row = emptyRow;
				rowIndex = 2;
				_row[rowIndex++] = _description[i];
			}
			else {
				std::cout << _row << std::endl;
				_row = emptyRow;
				rowIndex = 2;
				_row[rowIndex++] = _description[i];
			}
		}
		else if (_description[i] == ';') {
			std::cout << _row << std::endl;
			rowIndex = 2;
			_row = emptyRow;
			i++;
			if (_description[i] == ' ') {
				while (_description[i] == ' ') {
					i++;
				}
			}
			i--;
		}
		else {
			_row[rowIndex] = _description[i];
			rowIndex++;
		}
	}
	if (rowIndex > 2) {
		std::cout << _row << std::endl;
	}


	std::cout << "= Status : ";
	if (entry.getEntryStatus() == EntryStatus::FINISHED) {
		std::cout << "finished                                                                    =" << std::endl;
	}
	else {
		std::cout << "not finished                                                                =" << std::endl;
	}
 }

void MainApp::printStartScreen() {
	printWelcomeMessage();
	printEnterCommandScreen();
}

void MainApp::printEnterCommandScreen() {
	std::cout << "todo_app> ";
	std::string command;

	getline(std::cin, command);
	execCommand(command);
	printEnterCommandScreen();
}

void MainApp::execCommand_Help(std::string flag) {
	if (flag == "") {
		std::cout << "  Available commands: " << std::endl;
		std::cout << "  get     -   Display all existing reminders" << std::endl;
		std::cout << "  sort    -   Sort reminders" << std::endl;
		std::cout << "  filter  -   Filter reminders" << std::endl;
		std::cout << "  new     -   Make new reminder" << std::endl;
		std::cout << "  delete  -   Delete reminder" << std::endl;
		std::cout << "  edit    -   Edit reminder" << std::endl;
		std::cout << std::endl << "To see detailed description of one command type \"help -[command]\" " << std::endl;
	}
	else {
		if (flag == "-get") {
			std::cout << std::endl << "  Displays all existing reminders" << std::endl << std::endl;
			std::cout << "  Type 'get' with no arguments to display all existing reminders" << std::endl;
		}
		else if(flag == "-sort") {
			std::cout << std::endl << "  Displays sorted set of reminders" << std::endl << std::endl;
			std::cout << "  Type 'sort' to sort reminder the reminder set" << std::endl;
			std::cout << "  Use this command as following" << std::endl;
			std::cout << "  'sort -[arg] -[ord]" << std::endl;
			std::cout << "      [arg] is by what argument you wish to sort reminders by" << std::endl;
			std::cout << "      Available arguments are: -[t|d|ed|dc]" << std::endl;
			std::cout << "      Second argument displays sorted set in descending(1 or -des) or ascending order(0 or -asc)" << std::endl;
			std::cout << "          Descending order is default value" << std::endl;
		}
		else if (flag == "-filter") {
			std::cout << std::endl << "  Displays sorted set of reminders with applied filter" << std::endl << std::endl;
			std::cout << "  Type 'filter' to filter and sort reminder set in desired way" << std::endl;
			std::cout << "  Use this command as following" << std::endl;
			std::cout << "  'filter -mode [after|before|0|1] <date> <time> -[asc|des]" << std::endl;
			std::cout << "      Argument 0(1) means before(after) entered date and time" << std::endl;
			std::cout << "      Last argument displays sorted set in descending or ascending order" << std::endl;
			std::cout << "          Descending order is default value" << std::endl;
		}
		else if (flag == "-new") {
			std::cout << std::endl << "  Makes new reminder" << std::endl;
			std::cout << "  Type 'new' to make new reminder" << std::endl;
			std::cout << std::endl << "  Use this command as following" << std::endl;
			std::cout << "  'new -t <title> -d <description> -ed <exec_date> " << std::endl;
			std::cout << "      Arguments are optional, but if you enter just 'new' you will be" << std::endl;
			std::cout << "      able to enter arguments of reminder one by one" << std::endl << std::endl;
			std::cout << "  DISCLAIMER: If you wish to format description with typing text in multiple" << std::endl;
			std::cout << "              lines. Type ';' and everything after that will be in new line" << std::endl;
		}
		else if (flag == "-delete") {
			std::cout << std::endl << "  Deletes one reminder from the set" << std::endl;

			std::cout << "  Type 'delete n' to delete reminder with the index n" << std::endl;
			std::cout << "  TIP: Use this command along side command 'get' to see all the reminders" << std::endl;
			std::cout << "  which will help you to know what reminder to delete" << std::endl;
		}
		else if (flag == "-edit") {
			std::cout << std::endl << "  Type 'edit n to edit reminder with the index n and you will be" << std::endl;
			std::cout << "  presented with options to enter information and edit." << std::endl << std::endl;
			std::cout << "  TIP: Use this command along side command 'get' to see all the reminders" << std::endl;
			std::cout << "  which will help you to know what reminder to delete" << std::endl;
		}
		else if (flag == "delete" || flag == "new" || flag == "sort" || flag == "filter" || "get" || "edit") {
			std::cout << std::endl << "  You can't use two commands at the same time" << std::endl;
		}
		
		else {
			std::cout << "'" << flag << "'" << " is invalid command" << std::endl;
			printEnterCommandScreen();
		}
	}
}

void MainApp::execCommand_Get() {
	printSet(this->set->getAll());
}

void MainApp::execCommand_Delete(int index) {
	try {
		set->deleteEntry(index);
	}
	catch (const char* err) {
		std::cout << std::endl <<"  Something went wrong: " << err << std::endl;
	}
}

void MainApp::execCommand_New(std::vector<std::string> cmd) {
	if (cmd.size() == 1) {
		printMakeNewEntry();
		return;
	}
	else {
		if (cmd.size() > 8) {
			std::cout << std::endl << "  Too many arguments" << std::endl;
			return;
		}
	}


	if (cmd[1] != "-t" && cmd[1] != "-d" && cmd[1] != "-ed") {
		std::cout << std::endl << "  Wrong format for this command" << std::endl;
		return;
	}

	std::string title = "";
	std::string description = "";
	std::string execDate = "";
	for (int i = 1; i < cmd.size(); i++) {
		if (cmd[i] == "-t") {
			std::string _title = "";
			try {
				_title = cmd[i + 1];
				title = _title;
			}
			catch (std::exception err) {
				// ...
			}
			if (_title == "-d" || _title == "-ed") {
				title = "";
			}
			else {
				i++;
			}
		}
		else if (cmd[i] == "-d") {
			std::string _description = "";
			description = _description;
			try {
				_description = cmd[i + 1];
				description = _description;
			}
			catch (std::exception err) {
				// ...
			}
			if (_description == "-t" || _description == "-ed") {
				description = "";
			}
			else {
				i++;
			}
		}
		else if (cmd[i] == "-ed") {
			std::string date = "";
			std::string time = "";
			std::string date_time = "";

			try {
				if (i + 1 < cmd.size()) {
					date = cmd[i + 1];
					execDate = date;
				}
				try {
					if (i + 2 < cmd.size()) {
						time = cmd[i + 2];
						execDate += date;
					}
				}
				catch (std::exception err) {
					std::cout << err.what() << std::endl;
				}
				catch (const char* err) {
					std::cout << err << std::endl;
				}
			}
			catch (std::exception err) {
				std::cout << err.what() << std::endl;
			}
			catch (const char* err) {
				std::cout << err << std::endl;
			}


			if (date == "" && time == "") {
				date_time = "";
			}
			else if (date == "" && time != "") {
				std::cout << std::endl << "  Something went wrong" << std::endl;
				return;
			}
			else if (date != "" && time == "") {
				date_time = date;
				i++;
			}
			else if (date == "-t" || date == "-d") {
				date_time = "";
			}
			else {
				if (time == "-t" || time == "-d") {
					time = "";
					date_time = date;
					i++;
				}
				else {
					date_time = date + " " + time;
					i = i + 2;
				}
			}
			execDate = date_time;
		}
	}

	makeNewEntry(execDate, title, description);
}

void MainApp::execCommand_Sort(std::string attSort, bool descending) {
	if (attSort != "-t" && attSort != "-d" && attSort != "-ed" && attSort != "-dc" && attSort != "-s") {
		std::cout << std::endl << "  Wrong argument" << std::endl;
		std::cout << "Type 'help sort' to see what are available arguments for this command" << std::endl;
		return;
	}

	if (attSort == "-t") {
		printSet(set->sortByTitle(descending));
	}
	else if (attSort == "-d") {
		printSet(set->sortByDescription(descending));
	}
	else if (attSort == "-ed") {
		printSet(set->sortByExecDate(descending));
	}
	else if (attSort == "-dc") {
		printSet(set->sortByDateCreated(descending));
	}
	else if (attSort == "-s") {
		printSet(set->sortByStatus(descending));
	}
	else {
		std::cout << std::endl << "  Something went wrong. Try again" << std::endl;
	}
}

void MainApp::execCommand_Filter(std::string attFilter, FilterMode filter, DateTime* filterDate, bool descending) {
	if (attFilter != "-ed" && attFilter != "-dc") {
		std::cout << std::endl << "  Wrong arguments" << std::endl;
		return;
	}
	if (attFilter == "-dc") {
		printSet(set->filterByDateCreated(filter, filterDate, descending));
	}
	else if (attFilter == "-ed") {
		printSet(set->filterByExecDate(filter, filterDate, descending));
	}
	else {
		std::cout << std::endl << "  Something went wrong" << std::endl;
	}
}

void MainApp::execCommand_Filter(EntryStatus filterStatus) {
	printSet(set->filterByStatus(filterStatus));
}

void MainApp::execCommand_Edit(int index) {
	if (!set->getAll().count(index)) {
		std::cout << "  Reminder with index <" << index << "> doesn't exist" << std::endl;
		return;
	}

	printEditEntry(index, set->getAll()[index]); 
}

void MainApp::makeNewEntry(std::string execDate, std::string title, std::string description) {
	if (execDate == "" && title == "" && description == "") {
		try {
			set->makeNewEntry(new ReminderEntry());
		}
		catch (const char* err) {
			std::cout << std::endl << "  Something went wrong: " << err << std::endl;
		}
		catch (...) {
			std::cout << std::endl << "  Unknown error occurred" << std::endl;
		}
	}
	else if (execDate == "") {
		try {
			set->makeNewEntry(new ReminderEntry(title, description));
			if (title.size() > MAX_TITLE_LEN) {
				std::cout << "   Title was too long." << " Title was set to: " << title.substr(0, MAX_TITLE_LEN) << std::endl;
				std::cout << "   Instead of : " << title << std::endl;
			}
			if (description.size() > MAX_DESCRIPTION_LEN) {
				std::cout << "   Description was to long and it was shortened to " << MAX_DESCRIPTION_LEN << " characters" << std::endl;
				std::cout << "   Type 'get' to see what the reminder looks like" << std::endl;
			} 
		}
		catch (const char* err) {
			std::cout << std::endl << "  Something went wrong: " << err << std::endl;
		}
		catch (...) {
			std::cout << std::endl << "  Unknown error occurred" << std::endl;
		}
		
	}
	else if (execDate != "") {
		try {
			set->makeNewEntry(new ReminderEntry(new DateTime(execDate), title, description));
		}
		catch (const char* err) {
			std::cout << std::endl << "  Something went wrong: " << err << std::endl;
		}
		catch (...) {
			std::cout << std::endl << "  Unknown error occurred" << std::endl;
		}
	}
	else {
		std::cout << std::endl << "  Unknown error occurred" << std::endl;
	}
}

void MainApp::printWelcomeMessage() {
	std::cout << "> TODO APP  started" << std::endl << std::endl;
	std::cout << "  Type <help> to see available commands" << std::endl;
}

void MainApp::printMakeNewEntry() {
	std::string title;
	std::string description;
	std::string execDate;

	std::cout << std::endl << "If you want empty field, type '.' + ENTER" << std::endl;
	std::cout << "Enter title for reminder: ";
	getline(std::cin, title);
	std::cout << "Enter description for reminder: ";
	getline(std::cin, description);
	std::cout << "Enter date and/or time when the task should get done: ";
	getline(std::cin, execDate);

	if (execDate == ".")
		execDate = "";
	if (description == ".")
		description = "";
	if (title == ".")
		title = "";
	makeNewEntry(execDate, title, description);
}

void MainApp::printEditEntry(int index, ReminderEntry* oldEntry) {
	std::string _newTitle;
	std::string _newDescription;
	std::string _newExecDate;
	std::string _newStatus;

	std::cout << std::endl << "If you want an empty field, type '.' + ENTER" << std::endl;
	std::cout << std::endl << "If you want the field to remain unchanged, type '..' + ENTER" << std::endl;
	std::cout << "Current title: " << std::endl << oldEntry->getTitle() << std::endl;
	std::cout << "Enter new title: ";
	getline(std::cin, _newTitle);

	std::cout << "Current description: " << std::endl << oldEntry->getDescription() << std::endl;
	std::cout << "Enter new description: ";
	getline(std::cin, _newDescription);

	std::cout << "Current execution date: " << std::endl << oldEntry->getExecutionDate()->getFormat(false) << std::endl;
	std::cout << "Enter new execution date: ";
	getline(std::cin, _newExecDate);

	std::cout << "Current status: ";
	if (oldEntry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
		std::cout << "NOT FINISHED" << std::endl;
	}
	else {
		std::cout << "FINISHED" << std::endl;
	}
	bool flag = true;
	while (flag) {
		std::cout << "Do you want to change status [y/n]? ";
		getline(std::cin, _newStatus);

		_newStatus[0] = ::tolower(_newStatus[0]);

		if (_newStatus.size() > 1) {
			std::cout << "Please enter 'y' or 'n'" << std::endl;
		}
		else if (_newStatus != "y" && _newStatus != "n") {
			std::cout << "Please enter 'y' or 'n'" << std::endl;
		}
		else {
			flag = false;
		}
	}

	if (_newExecDate == ".")
		_newExecDate = "";
	else if (_newExecDate == "..") {
		_newExecDate = oldEntry->getExecutionDate()->getFormat(true);
	}
	if (_newDescription == ".")
		_newDescription = "";
	else if (_newDescription == "..") {
		_newDescription = oldEntry->getDescription();
	}
	if (_newTitle == ".")
		_newTitle = "";
	else if (_newTitle == "..") {
		_newTitle = oldEntry->getTitle();
	}
	std::cout << " " << std::endl;
	std::cout << " " << std::endl;

	std::cout << "New title: " << _newTitle << std::endl;
	std::cout << "New description: " << _newDescription << std::endl;
	std::cout << "New execution date: " << _newExecDate << std::endl;
	std::cout << "Status: ";
	if (_newStatus[0] == 'y') {
		std::cout << "changed to ";
		if (oldEntry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			std::cout << "FINISHED" << std::endl;
		}
		else {
			std::cout << "NOT FINISHED" << std::endl;
		}
	}
	else {
		if (oldEntry->getEntryStatus() == EntryStatus::NOT_FINISHED) {
			std::cout << "NOT FINISHED";
		}
		else {
			std::cout << "FINISHED";
		}

		std::cout << " (unchanged)" << std::endl << std::endl;
	}
	
	

	std::string save;
	flag = true;

	while (flag) {
		std::cout << "Do you want to save changes [y/n]? ";
		getline(std::cin, save);

		save[0] = ::tolower(save[0]);
		if (save.size() > 1) {
			std::cout << "Please 'y' or 'n'" << std::endl;
		}
		else if (save.size() == 1 && (save != "y" && save != "n")) {
			std::cout << "Please 'y' or 'n'" << std::endl;
		}
		else {
			flag = false;
		}
	}


	if (save == "y") {
		ReminderEntry* newEntry;
		try {
			newEntry = new ReminderEntry(new DateTime(_newExecDate), _newTitle, _newDescription);
			if (_newStatus == "y") {
				newEntry->changeEntryStatus();
			}
			set->editEntry(index, newEntry);

			std::cout << "  Successfully edited!" << std::endl;
		}
		catch (const char* err) {
			std::cout << "  Something went wrong: " << err << std::endl;
		}
	}
	else {
		std::cout << "Changes have not been applied..." << std::endl;
	}
}

void MainApp::runApp() {
	printStartScreen();
}
