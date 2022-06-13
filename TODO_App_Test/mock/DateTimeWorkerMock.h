#pragma once
#include "../pch.h"
#include "../../TODO_App/interface/DateTimeWorkerInterface.h"

class DateTimeWorkerMock : public DateTimeWorkerInterface {
public:
	MOCK_METHOD(std::vector<int>, GetCurrentDateAndTime, (), (override));
};
