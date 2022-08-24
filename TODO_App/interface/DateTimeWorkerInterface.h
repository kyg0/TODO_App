#pragma once
#include <ctime>
#include <iostream>
#include <vector>

class DateTimeWorkerInterface {
public:
	virtual std::vector<int> GetCurrentDateAndTime() = 0;
};