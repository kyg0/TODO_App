#pragma once
#include <ctime> // doesn't seem that you need all those includes here. Move unnecessary ones to .c file
#include <iostream>
#include <vector>

class DateTimeWorkerInterface {
public:
	virtual std::vector<int> GetCurrentDateAndTime() = 0;
};
