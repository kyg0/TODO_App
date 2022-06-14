#pragma once
#pragma warnings(disable:4996)
#include <ctime>
#include <string>
#include <iostream>
#include <vector>

class DateTimeWorkerInterface {
public:
	virtual std::vector<int> GetCurrentDateAndTime() = 0;
};
