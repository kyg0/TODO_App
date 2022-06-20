#pragma once
#include "../interface/DateTimeWorkerInterface.h"

class DateTimeWorker : public DateTimeWorkerInterface
{
public:
	std::vector<int> GetCurrentDateAndTime();
};

