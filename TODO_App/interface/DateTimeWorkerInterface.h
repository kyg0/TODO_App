#pragma once
#pragma warnings(disable:4996)
#include <ctime> // doesn't seem that you need all those includes here. Move unnecessary ones to .c file
#include <string>
#include <iostream>
#include <vector>

class DateTimeWorkerInterface {
public:
	virtual std::vector<int> GetCurrentDateAndTime() = 0;
};
