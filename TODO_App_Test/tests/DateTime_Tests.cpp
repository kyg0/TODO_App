#include "../pch.h"
#include "../../TODO_App/include/DateTime.h"
#include "../mock/DateTimeWorkerMock.h"

using namespace ::testing;

// Check if framework supports test suits (test set up and tear ups, to remove dependencies between tests)

TEST(DateTimeTests_Constructor, DefaultConstructor_withMock) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 20,5,2022,13,30,0 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));

	try {
		DateTime* dt = new DateTime(mock);
		DateTime* dt2 = new DateTime(20, 5, 2022, 13, 30);
		EXPECT_TRUE(*dt == *dt2);
	}
	catch (std::exception err) {
		std::cout << "ERROR: " << err.what() << std::endl;
		EXPECT_TRUE(false);
	}

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTimeTests_Constructor, DefaultConstructor_withNullMock) {
	DateTimeWorkerMock* mock = nullptr;

	std::vector<int> date{ 20,5,2022,13,30,0 };
	

	try {
		DateTime* dt = new DateTime(mock);
		DateTime* dt2 = new DateTime(dt->dtWorker);

		EXPECT_TRUE(*dt == *dt2);
	}
	catch (const char* err) {
		EXPECT_FALSE(true); // this should not happen ever
	}

	//Mock::AllowLeak(mock);
	//Mock::VerifyAndClear(mock);
}

// It should be split into smaller parts. Each of those should be a separate test that covers some test case.
// A lot better that have one massive test. Also in that way easier to isolate tests and remove possibility that
// previous checks will affect following steps.
// Yes, at the end it will give you a lot of tests, but making it atomic, it will help you to easier locate problem later on
//
// In general, I like the range of tests that you provided. I can see that you have invested effort for thinking about corner cases. Good work!


class DateTimeTest_Constructor : public Test {
protected:
	DateTime* dt;
	DateTimeWorkerMock* mock;
public:
	void SetUp(std::string format) {
		try {
			dt = new DateTime(format);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}

	void SetUp(int day, int month, int year) {
		try {
			dt = new DateTime(day, month, year);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}

	void SetUp(int day, int month, int year, int hours, int minutes) {
		try {
			dt = new DateTime(day, month, year, hours, minutes);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}

	void SetUp(int day, int month, int year, int hours, int minutes, int seconds) {
		try {
			dt = new DateTime(day, month, year, hours, minutes, seconds);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}

	void SetUp(std::vector<int> date) {
		mock = new DateTimeWorkerMock();
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));

		dt = new DateTime(mock);
	}

	void TearDown() {
		dt = nullptr;
	}

	void forceTearDown() {
		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		TearDown();
	}
};

class DateTimeTest_Setter : public DateTimeTest_Constructor {
public:
	void setAndTestDay(int day) {
		try {
			dt->setDay(day);
			EXPECT_EQ(dt->getDay(), day);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
	void setAndTestMonth(int month) {
		try {
			dt->setMonth(month);
			EXPECT_EQ(dt->getMonth(), month);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
	void setAndTestYear(int year) {
		try {
			dt->setYear(year);
			EXPECT_EQ(dt->getYear(), year);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
	void setAndTestHours(int hours) {
		try {
			dt->setHours(hours);
			EXPECT_EQ(dt->getHours(), hours);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
	void setAndTestMinutes(int minutes) {
		try {
			dt->setMinutes(minutes);
			EXPECT_EQ(dt->getMinutes(), minutes);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
	void setAndTestSeconds(int seconds) {
		try {
			dt->setSeconds(seconds);
			EXPECT_EQ(dt->getSeconds(), seconds);
		}
		catch (DateTimeException err) {
			throw err;
		}
	}
};

TEST_F(DateTimeTest_Constructor, CorrectFormat) {
	SetUp("01/01/2022 01:01:01");
	EXPECT_EQ(dt->getDay(), 1);
	EXPECT_EQ(dt->getMonth(), 1);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 1);
	EXPECT_EQ(dt->getMinutes(), 1);
	EXPECT_EQ(dt->getSeconds(), 1);
	EXPECT_EQ(dt->getFormat(true), "01/01/2022 01:01:01");

	SetUp("31/05/2022 23:59:59");
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 5);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 23);
	EXPECT_EQ(dt->getMinutes(), 59);
	EXPECT_EQ(dt->getSeconds(), 59);
	EXPECT_EQ(dt->getFormat(true), "31/05/2022 23:59:59");

	SetUp("29/02/2024 00:00:00");
	EXPECT_EQ(dt->getDay(), 29);
	EXPECT_EQ(dt->getMonth(), 2);
	EXPECT_EQ(dt->getYear(), 2024);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "29/02/2024 00:00:00");

	SetUp("28/02/2022 23:59:59");
	EXPECT_EQ(dt->getDay(), 28);
	EXPECT_EQ(dt->getMonth(), 2);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 23);
	EXPECT_EQ(dt->getMinutes(), 59);
	EXPECT_EQ(dt->getSeconds(), 59);
	EXPECT_EQ(dt->getFormat(true), "28/02/2022 23:59:59");

	SetUp("31/12/9999 23:59:59");
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 12);
	EXPECT_EQ(dt->getYear(), 9999);
	EXPECT_EQ(dt->getHours(), 23);
	EXPECT_EQ(dt->getMinutes(), 59);
	EXPECT_EQ(dt->getSeconds(), 59);
	EXPECT_EQ(dt->getFormat(true), "31/12/9999 23:59:59");

	SetUp("30/09/2022 00:00:00");
	EXPECT_EQ(dt->getDay(), 30);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "30/09/2022 00:00:00");

	SetUp("25-02-2022");
	EXPECT_EQ(dt->getDay(), 25);
	EXPECT_EQ(dt->getMonth(), 2);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "25/02/2022 00:00:00");

	SetUp("01-01-2022 01:01:01");
	EXPECT_EQ(dt->getDay(), 1);
	EXPECT_EQ(dt->getMonth(), 1);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 1);
	EXPECT_EQ(dt->getMinutes(), 1);
	EXPECT_EQ(dt->getSeconds(), 1);
	EXPECT_EQ(dt->getFormat(true), "01/01/2022 01:01:01");

	SetUp("31-12-2033 23:59");
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 12);
	EXPECT_EQ(dt->getYear(), 2033);
	EXPECT_EQ(dt->getHours(), 23);
	EXPECT_EQ(dt->getMinutes(), 59);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "31/12/2033 23:59:00");

	SetUp("28\\02\\2022 12.12.12");
	EXPECT_EQ(dt->getDay(), 28);
	EXPECT_EQ(dt->getMonth(), 2);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 12);
	EXPECT_EQ(dt->getMinutes(), 12);
	EXPECT_EQ(dt->getSeconds(), 12);
	EXPECT_EQ(dt->getFormat(true), "28/02/2022 12:12:12");

	SetUp("29\\05\\2022 12.12");
	EXPECT_EQ(dt->getDay(), 29);
	EXPECT_EQ(dt->getMonth(), 5);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 12);
	EXPECT_EQ(dt->getMinutes(), 12);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "29/05/2022 12:12:00");

	SetUp("31\\10\\2022");
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 10);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "31/10/2022 00:00:00");

	SetUp("01\\01\\2022 12/12/12");
	EXPECT_EQ(dt->getDay(), 1);
	EXPECT_EQ(dt->getMonth(), 1);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 12);
	EXPECT_EQ(dt->getMinutes(), 12);
	EXPECT_EQ(dt->getSeconds(), 12);
	EXPECT_EQ(dt->getFormat(true), "01/01/2022 12:12:12");

	SetUp("01\\01\\2022 12/12");
	EXPECT_EQ(dt->getDay(), 1);
	EXPECT_EQ(dt->getMonth(), 1);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 12);
	EXPECT_EQ(dt->getMinutes(), 12);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "01/01/2022 12:12:00");

	SetUp("26 09 2022 05 35 45");
	EXPECT_EQ(dt->getDay(), 26);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 5);
	EXPECT_EQ(dt->getMinutes(), 35);
	EXPECT_EQ(dt->getSeconds(), 45);
	EXPECT_EQ(dt->getFormat(true), "26/09/2022 05:35:45");

	SetUp("26 09 2022 05 35");
	EXPECT_EQ(dt->getDay(), 26);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 5);
	EXPECT_EQ(dt->getMinutes(), 35);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(true), "26/09/2022 05:35:00");

	SetUp("26 09 2022");
	EXPECT_EQ(dt->getDay(), 26);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
}

TEST_F(DateTimeTest_Constructor, InvalidFormat) {
	try {
		SetUp("9/9/2022 9:30:32");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("09/09/2022 9:9:9");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("29/09/2o22 13:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("29/09/2022 13/0/0");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("29/09/999 13:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("30/11/10000 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}

	try {
		SetUp("30/o3/2019 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_FORMAT);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_FORMAT);
	}
}

TEST_F(DateTimeTest_Constructor, WrongValues) {
	try {
		SetUp("32/01/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("00/01/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("29/02/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("30/02/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("32/03/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("31/04/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("31/06/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("31/09/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("31/11/2022 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp("30/11/2019 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp("30/11/0223 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp("30/00/2023 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}

	try {
		SetUp("30/13/2023 12:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}

	try {
		SetUp("30/03/2023 24:00:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}

	try {
		SetUp("30/03/2023 25:00:00");
			FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}

	try {
		SetUp("30/03/2023 23:60:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}

	try {
		SetUp("30/03/2023 23:99:00");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}

	try {
		SetUp("30/03/2023 23:00:60");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}

	try {
		SetUp("30/03/2023 23:00:61");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}

	try {
		SetUp("30/03/2023 23:00:99");
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}
}

TEST_F(DateTimeTest_Constructor, CorrectValuesAny) {
	SetUp(26, 9, 2043);
	EXPECT_EQ(dt->getDay(), 26);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2043);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(false), "26/09/2043 00:00");
	EXPECT_EQ(dt->getFormat(true), "26/09/2043 00:00:00");
}

TEST_F(DateTimeTest_Constructor, CorrectValues_MinMax) {
	SetUp(MIN_DAY_VALUE, MIN_MONTH_VALUE, MIN_YEAR_VALUE, MIN_HOURS_VALUE, MIN_MINUTES_VALUE, MIN_SECONDS_VALUE);

	EXPECT_EQ(dt->getDay(), 1);
	EXPECT_EQ(dt->getMonth(), 1);
	EXPECT_EQ(dt->getYear(), 2020);
	EXPECT_EQ(dt->getHours(), 0);
	EXPECT_EQ(dt->getMinutes(), 0);
	EXPECT_EQ(dt->getSeconds(), 0);
	EXPECT_EQ(dt->getFormat(false), "01/01/2020 00:00");
	EXPECT_EQ(dt->getFormat(true), "01/01/2020 00:00:00");

	SetUp(15, MAX_MONTH_VALUE, MAX_YEAR_VALUE, MAX_HOURS_VALUE, MAX_MINUTES_VALUE, MAX_SECONDS_VALUE);
	EXPECT_EQ(dt->getDay(), 15);
	EXPECT_EQ(dt->getMonth(), 12);
	EXPECT_EQ(dt->getYear(), 9999);
	EXPECT_EQ(dt->getHours(), 23);
	EXPECT_EQ(dt->getMinutes(), 59);
	EXPECT_EQ(dt->getSeconds(), 59);
	EXPECT_EQ(dt->getFormat(false), "15/12/9999 23:59");
	EXPECT_EQ(dt->getFormat(true), "15/12/9999 23:59:59");
}

TEST_F(DateTimeTest_Constructor, CorrectValue_30DayMonth_Boundary) {
	SetUp(MAX_DAY_VALUE_30DAY_MONTH, Month::APRIL, 2100);
	EXPECT_EQ(dt->getDay(), 30);
	EXPECT_EQ(dt->getMonth(), 4);

	SetUp(MAX_DAY_VALUE_30DAY_MONTH, Month::JUNE, 2100);
	EXPECT_EQ(dt->getDay(), 30);
	EXPECT_EQ(dt->getMonth(), 6);

	SetUp(MAX_DAY_VALUE_30DAY_MONTH, Month::SEPTEMBER, 2100);
	EXPECT_EQ(dt->getDay(), 30);
	EXPECT_EQ(dt->getMonth(), 9);

	SetUp(MAX_DAY_VALUE_30DAY_MONTH, Month::NOVEMBER, 2100);
	EXPECT_EQ(dt->getDay(), 30);
	EXPECT_EQ(dt->getMonth(), 11);
}

TEST_F(DateTimeTest_Constructor, CorrectValue_31DayMonth_Boundary) {
	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::JANUARY, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 1);

	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::MARCH, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 3);

	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::MAY, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 5);

	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::JULY, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 7);

	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::AUGUST, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 8);

	SetUp(MAX_DAY_VALUE_31DAY_MONTH, Month::DECEMBER, 2100);
	EXPECT_EQ(dt->getDay(), 31);
	EXPECT_EQ(dt->getMonth(), 12);
}

TEST_F(DateTimeTest_Constructor, CorrectValues_BoundaryValuesForFebruary) {
	SetUp(MAX_DAY_VALUE_FEBRUATY_LEAP, Month::FEBRUARY, 2028);
	EXPECT_EQ(dt->getDay(), 29);
	EXPECT_EQ(dt->getMonth(), 2);

	SetUp(MAX_DAY_VALUE_FEBRUARY_NOLEAP, Month::FEBRUARY, 2027);
	EXPECT_EQ(dt->getDay(), 28);
	EXPECT_EQ(dt->getMonth(), 2);
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForDays_30DaysMonth) {
	try {
		SetUp(MAX_DAY_VALUE_30DAY_MONTH + 1, Month::APRIL, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch(DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(MAX_DAY_VALUE_30DAY_MONTH + 1, Month::JUNE, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(MAX_DAY_VALUE_30DAY_MONTH + 1, Month::SEPTEMBER, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(MAX_DAY_VALUE_30DAY_MONTH + 1, Month::NOVEMBER, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForDays_AnyMonth) {
	try {
		SetUp(MAX_DAY_VALUE_31DAY_MONTH + 1, Month::NOVEMBER, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		DateTime dt(MIN_DAY_VALUE - 1, Month::APRIL, 2100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForMonth) {
	try {
		SetUp(15, MIN_MONTH_VALUE - 1, 2025);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_MONTH, err.what());
	}

	try {
		SetUp(15, MAX_MONTH_VALUE + 1, 2025);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_MONTH, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForYear) {
	try {
		SetUp(30, 4, MIN_YEAR_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_YEAR, err.what());
	}

	try {
		SetUp(30, 4, 0);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_YEAR, err.what());
	}

	try {
		SetUp(30, 4, -1);
		FAIL() << "Exptected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_YEAR, err.what());
	}

	try {
		DateTime dt3(30, 4, MAX_YEAR_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch(DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_YEAR, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForFebruary) {
	try {
		SetUp(MAX_DAY_VALUE_FEBRUATY_LEAP, Month::FEBRUARY, 2101);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(MAX_DAY_VALUE_30DAY_MONTH, Month::FEBRUARY, 2028);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(MIN_DAY_VALUE - 1, Month::FEBRUARY, 2028);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForHours) {
	try {
		SetUp(30, 4, 2100, MIN_HOURS_VALUE - 1, 0);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_HOURS, err.what());
	}

	try {
		SetUp(30, 4, 2100, MAX_HOURS_VALUE + 1, 0);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_HOURS, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForMinutes) {
	try {
		SetUp(30, 4, 2100, 10, MIN_MINUTES_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_MINUTES, err.what());
	}

	try {
		SetUp(30, 4, 2100, 10, MAX_MINUTES_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_MINUTES, err.what());
	}
}

TEST_F(DateTimeTest_Constructor, InvalidValues_ForSeconds) {
	try {
		SetUp(30, 4, 2100, 0, 0, MIN_SECONDS_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_SECONDS, err.what());
	}

	try {
		SetUp(30, 4, 2100, 0, 0, MAX_SECONDS_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_SECONDS, err.what());
	}
}

TEST_F(DateTimeTest_Setter, CorrectValues_ForDays_30DaysMonth) {
	SetUp(10, Month::APRIL, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH);

	SetUp(10, Month::JUNE, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH);

	SetUp(10, Month::SEPTEMBER, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH);

	SetUp(10, Month::NOVEMBER, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH);
}

TEST_F(DateTimeTest_Setter, CorrectValues_ForDays_31DaysMonth) {
	SetUp(10, Month::JANUARY, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::MARCH, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::MAY, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::JULY, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::AUGUST, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::OCTOBER, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);

	SetUp(10, Month::DECEMBER, 2030);
	setAndTestDay(MIN_DAY_VALUE);
	setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH);
}

//OVJE STIGAO

TEST_F(DateTimeTest_Setter, CorrectValue_ForMonth) {
	std::vector<int> date{ 26,9,2022,5, 5, 5 };
	
	SetUp(date);
	setAndTestMonth(MIN_MONTH_VALUE);

	SetUp(date);
	setAndTestMonth(MAX_MONTH_VALUE);

	SetUp(date);
	setAndTestMonth(7);
}

TEST_F(DateTimeTest_Setter, CorrectValue_ForYear) {
	std::vector<int> date{ 26,9,2022,5, 5,5 };
	
	SetUp(date);
	setAndTestYear(MIN_YEAR_VALUE);

	SetUp(date);
	setAndTestYear(MAX_YEAR_VALUE);

	setAndTestYear(2050);
}

TEST_F(DateTimeTest_Setter, CorrectValue_ForHours) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	
	SetUp(date);
	setAndTestHours(MAX_HOURS_VALUE);

	SetUp(date);
	setAndTestHours(MIN_HOURS_VALUE);

	SetUp(date);
	setAndTestHours(13);
}

TEST_F(DateTimeTest_Setter, CorrectValue_ForMinutes) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	
	SetUp(date);
	setAndTestMinutes(MAX_MINUTES_VALUE);

	SetUp(date);
	setAndTestMinutes(MIN_MINUTES_VALUE);

	SetUp(date);
	setAndTestMinutes(30);
}

TEST_F(DateTimeTest_Setter, CorrectValue_ForSeconds) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };

	SetUp(date);
	setAndTestSeconds(MAX_SECONDS_VALUE);

	SetUp(date);
	setAndTestSeconds(MIN_SECONDS_VALUE);

	SetUp(date);
	setAndTestSeconds(30);
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForDays) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);
		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(date);
		setAndTestDay(-5);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(date);
		setAndTestDay(40);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForDays_30DaysMonth) {
	try {
		SetUp(26, Month::APRIL, 2040);
		setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::APRIL, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::APRIL, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::APRIL, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JUNE, 2040);
		setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JUNE, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JUNE, 2040);
		setAndTestDay(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JUNE, 2040);
		setAndTestDay(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::SEPTEMBER, 2040);
		setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::SEPTEMBER, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::SEPTEMBER, 2040);
		setAndTestDay(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::SEPTEMBER, 2040);
		setAndTestDay(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::NOVEMBER, 2040);
		setAndTestDay(MAX_DAY_VALUE_30DAY_MONTH + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::NOVEMBER, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::NOVEMBER, 2040);
		setAndTestDay(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::NOVEMBER, 2040);
		setAndTestDay(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForDays_31DayMonth) {
	try {
		SetUp(26, Month::JANUARY, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JANUARY, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
	
	try {
		SetUp(26, Month::JANUARY, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JANUARY, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MARCH, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MARCH, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MARCH, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MARCH, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MAY, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MAY, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MAY, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::MAY, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JULY, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JULY, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JULY, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::JULY, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::AUGUST, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::AUGUST, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::AUGUST, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::AUGUST, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::OCTOBER, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::OCTOBER, 2040);
		setAndTestDay(MIN_DAY_VALUE -1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::OCTOBER, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::OCTOBER, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::DECEMBER, 2040);
		setAndTestDay(MAX_DAY_VALUE_31DAY_MONTH + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::DECEMBER, 2040);
		setAndTestDay(MIN_DAY_VALUE - 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::DECEMBER, 2040);
		setAndTestDay(-1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}

	try {
		SetUp(26, Month::DECEMBER, 2040);
		setAndTestDay(100);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(ERR_MSG_FOR_DAYS, err.what());
	}
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForDays_February) {
	try {
		SetUp(26, Month::FEBRUARY, 2100);
		setAndTestDay(MAX_DAY_VALUE_FEBRUATY_LEAP + 1);

		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2100);

		setAndTestDay(50);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2100);

		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2100);

		setAndTestDay(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2099);

		setAndTestDay(MAX_DAY_VALUE_FEBRUARY_NOLEAP + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2099);

		setAndTestDay(MIN_DAY_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2099);

		setAndTestDay(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}

	try {
		SetUp(26, Month::FEBRUARY, 2099);

		setAndTestDay(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_DAYS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_DAYS);
	}
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForMonth) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);

		setAndTestMonth(MIN_MONTH_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}

	try {
		SetUp(date);

		setAndTestMonth(MAX_MONTH_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}

	try {
		SetUp(date);

		setAndTestMonth(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}

	try {
		SetUp(date);

		setAndTestMonth(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MONTH);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MONTH);
	}
}

TEST_F(DateTimeTest_Setter, InvalidValues_ForYear) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);

		setAndTestYear(MIN_YEAR_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp(date);

		setAndTestYear(MAX_YEAR_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp(date);

		setAndTestYear(0);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp(date);

		setAndTestYear(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}

	try {
		SetUp(date);

		setAndTestYear(100000);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_YEAR);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_YEAR);
	}
}

TEST_F(DateTimeTest_Setter, InvalidValue_ForHours) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);

		setAndTestHours(MIN_HOURS_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}

	try {
		SetUp(date);

		setAndTestHours(MAX_HOURS_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}

	try {
		SetUp(date);

		setAndTestHours(60);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}

	try {
		SetUp(date);

		setAndTestHours(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_HOURS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_HOURS);
	}
}

TEST_F(DateTimeTest_Setter, InvalidValue_ForMinutes) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);

		setAndTestMinutes(MIN_MINUTES_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}

	try {
		SetUp(date);

		setAndTestMinutes(MAX_MINUTES_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}

	try {
		SetUp(date);

		setAndTestMinutes(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}

	try {
		SetUp(date);

		setAndTestMinutes(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_MINUTES);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_MINUTES);
	}
}

TEST_F(DateTimeTest_Setter, InvalidValue_ForSeconds) {
	std::vector<int> date{ 26, 9, 2022, 5, 5, 5 };
	try {
		SetUp(date);

		setAndTestSeconds(MIN_SECONDS_VALUE - 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}

	try {
		SetUp(date);

		setAndTestSeconds(MAX_SECONDS_VALUE + 1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}

	try {
		SetUp(date);

		setAndTestSeconds(100);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}

	try {
		SetUp(date);

		setAndTestSeconds(-1);
		FAIL() << "Expected: " + std::string(ERR_MSG_FOR_SECONDS);
	}
	catch (DateTimeException err) {
		EXPECT_EQ(err.what(), ERR_MSG_FOR_SECONDS);
	}
}

TEST(DateTime_ComparisonTest, ComparationOperator) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();
	std::vector<int> date{ 5, 5, 2100, 12, 12, 12 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));

	DateTime* a = new DateTime(mock);
	DateTime* b = new DateTime(mock);

	EXPECT_TRUE(!(*a < *b) && !(*a > *b));

	b->setYear(b->getYear() + 1);
	EXPECT_TRUE(*a > *b);

	b->setYear(b->getYear() - 1);
	b->setMonth(b->getMonth() + 1);
	EXPECT_TRUE(*a > *b);

	b->setMonth(b->getMonth() - 1);
	b->setDay(b->getMonth() + 1);
	EXPECT_TRUE(*a > *b);

	b->setDay(b->getDay() - 1);
	b->setHours(b->getHours() + 1);
	EXPECT_TRUE(*a > *b);

	b->setHours(b->getHours() - 1);
	b->setMinutes(b->getMinutes() + 1);
	EXPECT_TRUE(*a > *b);


	b->setMinutes(b->getMinutes() - 1);
	b->setSeconds(b->getSeconds() + 1);
	EXPECT_TRUE(*a > *b);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTime_ComparisonTest, EqualsOperator_Case1) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5,5 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
	DateTime dt1(mock);
	DateTime dt2(mock);
	EXPECT_TRUE(dt1 == dt2);

	
	DateTime* dt3 = new DateTime(mock);
	DateTime dt4(mock);
	EXPECT_TRUE(*dt3 == dt4);

	
	DateTime* dt5 = new DateTime(mock);
	DateTime* dt6 = new DateTime(mock);
	EXPECT_TRUE(*dt5 == *dt6);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTime_ComparisonTest, EqualsOperator_Case2){
	DateTime dt1(5, 5, 2022);
	DateTime dt2(5, 5, 2022);
	EXPECT_TRUE(dt1 == dt2);

	dt1.setSeconds(dt1.getSeconds() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setSeconds(dt2.getSeconds() + 1);
	EXPECT_TRUE(dt2 == dt1);

	dt1.setMinutes(dt1.getMinutes() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setMinutes(dt2.getMinutes() + 1);
	EXPECT_TRUE(dt1 == dt2);

	dt1.setHours(dt1.getHours() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setHours(dt2.getHours() + 1);
	EXPECT_TRUE(dt1 == dt2);

	dt1.setDay(dt1.getDay() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setDay(dt2.getDay() + 1);
	EXPECT_TRUE(dt1 == dt2);

	dt1.setMonth(dt1.getMonth() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setMonth(dt2.getMonth() + 1);
	EXPECT_TRUE(dt1 == dt2);

	dt1.setYear(dt1.getYear() + 1);
	EXPECT_FALSE(dt1 == dt2);
	dt2.setYear(dt2.getYear() + 1);
	EXPECT_TRUE(dt1 == dt2);
}

