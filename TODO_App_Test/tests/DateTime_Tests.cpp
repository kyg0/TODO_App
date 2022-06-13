#include "../pch.h"
#include "../../TODO_App/include/DateTime.h"
#include "../mock/DateTimeWorkerMock.h"

using namespace ::testing;

TEST(DateTimeTests_Constructor, DefaultConstructor_withMock) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 20,5,2022,13,30,0 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));

	DateTime* dt = new DateTime(mock);
	DateTime* dt2 = new DateTime(20, 5, 2022, 13, 30);
	EXPECT_TRUE(*dt == *dt2);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTimeTests_Constructor, Constructor_WithFormat) {
	std::string str1 = "01/01/2022 01:01:01";
	DateTime dt1(str1);

	EXPECT_EQ(dt1.getDay(), 1);
	EXPECT_EQ(dt1.getMonth(), 1);
	EXPECT_EQ(dt1.getYear(), 2022);
	EXPECT_EQ(dt1.getHours(), 1);
	EXPECT_EQ(dt1.getMinutes(), 1);
	EXPECT_EQ(dt1.getSeconds(), 1);


	std::string str2 = "31/05/2022 23:59:59";
	DateTime dt2(str2);
	EXPECT_EQ(dt2.getDay(), 31);
	EXPECT_EQ(dt2.getMonth(), 5);
	EXPECT_EQ(dt2.getYear(), 2022);
	EXPECT_EQ(dt2.getHours(), 23);
	EXPECT_EQ(dt2.getMinutes(), 59);
	EXPECT_EQ(dt2.getSeconds(), 59);

	std::string str3 = "29/02/2024 00:00:00";
	DateTime dt3(str3);
	EXPECT_EQ(dt3.getDay(), 29);
	EXPECT_EQ(dt3.getMonth(), 2);
	EXPECT_EQ(dt3.getYear(), 2024);
	EXPECT_EQ(dt3.getHours(), 0);
	EXPECT_EQ(dt3.getMinutes(), 0);
	EXPECT_EQ(dt3.getSeconds(), 0);

	std::string str4 = "28/02/2022 23:59:59";
	DateTime dt4(str4);
	EXPECT_EQ(dt4.getDay(), 28);
	EXPECT_EQ(dt4.getMonth(), 2);
	EXPECT_EQ(dt4.getYear(), 2022);
	EXPECT_EQ(dt4.getHours(), 23);
	EXPECT_EQ(dt4.getMinutes(), 59);
	EXPECT_EQ(dt4.getSeconds(), 59);

	std::string str5 = "31/12/9999 23:59:59";
	DateTime dt5(str5);
	EXPECT_EQ(dt5.getDay(), 31);
	EXPECT_EQ(dt5.getMonth(), 12);
	EXPECT_EQ(dt5.getYear(), 9999);
	EXPECT_EQ(dt5.getHours(), 23);
	EXPECT_EQ(dt5.getMinutes(), 59);
	EXPECT_EQ(dt5.getSeconds(), 59);

	std::string str6 = "30/09/2022 00:00:00";
	DateTime dt6(str6);
	EXPECT_EQ(dt6.getDay(), 30);
	EXPECT_EQ(dt6.getMonth(), 9);
	EXPECT_EQ(dt6.getYear(), 2022);
	EXPECT_EQ(dt6.getHours(), 0);
	EXPECT_EQ(dt6.getMinutes(), 0);
	EXPECT_EQ(dt6.getSeconds(), 0);

	std::string str7 = "25-02-2022";
	DateTime dt7(str7);
	EXPECT_EQ(dt7.getDay(), 25);
	EXPECT_EQ(dt7.getMonth(), 2);
	EXPECT_EQ(dt7.getYear(), 2022);
	EXPECT_EQ(dt7.getHours(), 0);
	EXPECT_EQ(dt7.getMinutes(), 0);
	EXPECT_EQ(dt7.getSeconds(), 0);

	std::string str8 = "01-01-2022 01:01:01";
	DateTime dt8(str8);
	EXPECT_EQ(dt8.getDay(), 1);
	EXPECT_EQ(dt8.getMonth(), 1);
	EXPECT_EQ(dt8.getYear(), 2022);
	EXPECT_EQ(dt8.getHours(), 1);
	EXPECT_EQ(dt8.getMinutes(), 1);
	EXPECT_EQ(dt8.getSeconds(), 1);

	std::string str9 = "31-12-2033 23:59";
	DateTime dt9(str9);
	EXPECT_EQ(dt9.getDay(), 31);
	EXPECT_EQ(dt9.getMonth(), 12);
	EXPECT_EQ(dt9.getYear(), 2033);
	EXPECT_EQ(dt9.getHours(), 23);
	EXPECT_EQ(dt9.getMinutes(), 59);
	EXPECT_EQ(dt9.getSeconds(), 0);

	std::string str10 = "28\\02\\2022 12.12.12";
	DateTime dt10(str10);
	EXPECT_EQ(dt10.getDay(), 28);
	EXPECT_EQ(dt10.getMonth(), 2);
	EXPECT_EQ(dt10.getYear(), 2022);
	EXPECT_EQ(dt10.getHours(), 12);
	EXPECT_EQ(dt10.getMinutes(), 12);
	EXPECT_EQ(dt10.getSeconds(), 12);

	std::string str11 = "29\\05\\2022 12.12";
	DateTime dt11(str11);
	EXPECT_EQ(dt11.getDay(), 29);
	EXPECT_EQ(dt11.getMonth(), 5);
	EXPECT_EQ(dt11.getYear(), 2022);
	EXPECT_EQ(dt11.getHours(), 12);
	EXPECT_EQ(dt11.getMinutes(), 12);
	EXPECT_EQ(dt11.getSeconds(), 0);

	std::string str12 = "31\\10\\2022";
	DateTime dt12(str12);
	EXPECT_EQ(dt12.getDay(), 31);
	EXPECT_EQ(dt12.getMonth(), 10);
	EXPECT_EQ(dt12.getYear(), 2022);
	EXPECT_EQ(dt12.getHours(), 0);
	EXPECT_EQ(dt12.getMinutes(), 0);
	EXPECT_EQ(dt12.getSeconds(), 0);

	std::string str13 = "01\\01\\2022 12/12/12";
	DateTime dt13(str13);
	EXPECT_EQ(dt13.getDay(), 1);
	EXPECT_EQ(dt13.getMonth(), 1);
	EXPECT_EQ(dt13.getYear(), 2022);
	EXPECT_EQ(dt13.getHours(), 12);
	EXPECT_EQ(dt13.getMinutes(), 12);
	EXPECT_EQ(dt13.getSeconds(), 12);

	std::string str14 = "01\\01\\2022 12/12";
	DateTime dt14(str14);
	EXPECT_EQ(dt14.getDay(), 1);
	EXPECT_EQ(dt14.getMonth(), 1);
	EXPECT_EQ(dt14.getYear(), 2022);
	EXPECT_EQ(dt14.getHours(), 12);
	EXPECT_EQ(dt14.getMinutes(), 12);
	EXPECT_EQ(dt14.getSeconds(), 0);

	std::string str15 = "26 09 2022 05 35 45";
	DateTime dt15(str15);
	EXPECT_EQ(dt15.getDay(), 26);
	EXPECT_EQ(dt15.getMonth(), 9);
	EXPECT_EQ(dt15.getYear(), 2022);
	EXPECT_EQ(dt15.getHours(), 5);
	EXPECT_EQ(dt15.getMinutes(), 35);
	EXPECT_EQ(dt15.getSeconds(), 45);

	std::string str16 = "26 09 2022 05 35";
	DateTime dt16(str16);
	EXPECT_EQ(dt16.getDay(), 26);
	EXPECT_EQ(dt16.getMonth(), 9);
	EXPECT_EQ(dt16.getYear(), 2022);
	EXPECT_EQ(dt16.getHours(), 5);
	EXPECT_EQ(dt16.getMinutes(), 35);
	EXPECT_EQ(dt16.getSeconds(), 0);

	std::string str17 = "26 09 2022";
	DateTime dt17(str17);
	EXPECT_EQ(dt17.getDay(), 26);
	EXPECT_EQ(dt17.getMonth(), 9);
	EXPECT_EQ(dt17.getYear(), 2022);
	EXPECT_EQ(dt17.getHours(), 0);
	EXPECT_EQ(dt17.getMinutes(), 0);
	EXPECT_EQ(dt17.getSeconds(), 0);
}

TEST(DateTimeTests_Constructor, Constructor_WithFormat_InvalidFormat) {
	try {
		std::string str = "9/9/2022 9:30:32";
		DateTime dt(str);

		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "09/09/2022 9:9:9";
		DateTime dt(str);

		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "29/09/2o22 13:00:00";
		DateTime dt(str);
		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "29/09/2022 13/0/0";
		DateTime dt(str);
		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "29/09/999 13:00:00";
		DateTime dt(str);
		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "30/11/10000 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}

	try {
		std::string str = "30/o3/2019 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong format";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong format");
	}
}

TEST(DateTimeTests_Constructor, Constructor_WithFormat_WrongValues) {
	try {
		std::string str = "32/01/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "00/01/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "29/02/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "30/02/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "32/03/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "31/04/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "31/06/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "31/09/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "31/11/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "31/11/2022 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		std::string str = "30/11/2019 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for year");
	}

	try {
		std::string str = "30/11/0223 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for year");
	}

	try {
		std::string str = "30/00/2023 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}


	try {
		std::string str = "30/13/2023 12:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}

	try {
		std::string str = "30/03/2023 24:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}

	try {
		std::string str = "30/03/2023 25:00:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}

	try {
		std::string str = "30/03/2023 23:60:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}

	try {
		std::string str = "30/03/2023 23:99:00";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}

	try {
		std::string str = "30/03/2023 23:00:60";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}

	try {
		std::string str = "30/03/2023 23:00:61";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}

	try {
		std::string str = "30/03/2023 23:00:99";
		DateTime dt(str);

		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}
}

TEST(DateTimeTests_Constructor, CorrectValues1) {
	DateTime dt(1, 1, 2043);

	EXPECT_EQ(dt.getDay(), 1);
	EXPECT_EQ(dt.getMonth(), 1);
	EXPECT_EQ(dt.getYear(), 2043);

	EXPECT_EQ(dt.getHours(), 0);
	EXPECT_EQ(dt.getMinutes(), 0);
	EXPECT_EQ(dt.getSeconds(), 0);

	EXPECT_EQ(dt.getFormat(false), "01/01/2043 00:00");
	EXPECT_EQ(dt.getFormat(true), "01/01/2043 00:00:00");
}

TEST(DateTimeTests_Constructor, CorrectValues_BoundaryValuesForFebruary) {
	DateTime dt(29, 2, 2028);
	EXPECT_EQ(dt.getDay(), 29);
	EXPECT_EQ(dt.getMonth(), 2);

	DateTime dt2(28, 2, 2027);
	EXPECT_EQ(dt2.getDay(), 28);
}

TEST(DateTimeTests_Constructor, InvalidValues_ForDays_30DaysMonth) {
	try {
		DateTime dt(31, 4, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch(const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(31, 6, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(31, 9, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(31, 11, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForDays_AnyMonth) {
	try {
		DateTime dt(32, 11, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(0, 4, 2100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForMonth) {
	try {
		DateTime dt(15, 0, 2025);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for month", err);
	}

	try {
		DateTime dt(15, 13, 2025);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for month", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForYear) {
	try {
		DateTime dt(30, 4, 2019);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}

	try {
		DateTime dt1(31, 4, 0);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}

	try {
		DateTime dt2(31, 4, -1);
		FAIL() << "Exptected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}

	try {
		DateTime dt3(31, 4, 10000);
		FAIL() << ("Expected: Wrong value for year");
	}
	catch(const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForFebruary) {
	try {
		DateTime dt(29, 2, 2101);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(30, 2, 2028);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForHours) {
	try {
		DateTime dt(30, 4, 2100, -1, 0);
		FAIL() << "Expected: Hours can have value between 0 and 23";
	}
	catch (const char* err) {
		EXPECT_STREQ("Hours can have value between 0 and 23", err);
	}

	try {
		DateTime dt(30, 4, 2100, 24, 0);
		FAIL() << "Expected: Hours can have value between 0 and 23";
	}
	catch (const char* err) {
		EXPECT_STREQ("Hours can have value between 0 and 23", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForMinutes) {
	try {
		DateTime dt(30, 4, 2100, 10, -1);
		FAIL() << "Expected: Minutes can have value between 0 and 59";
	}
	catch (const char* err) {
		EXPECT_STREQ("Minutes can have value between 0 and 59", err);
	}

	try {
		DateTime dt(30, 4, 2100, 10, 60);
		FAIL() << "Expected: Minutes can have value between 0 and 59";
	}
	catch (const char* err) {
		EXPECT_STREQ("Minutes can have value between 0 and 59", err);
	}
}

TEST(DateTimeTests_Constructor, InvalidValues_ForSeconds) {
	try {
		DateTime dt(30, 4, 2100, 0, 0, -1);
		FAIL() << "Expected: Seconds can have value between 0 and 59";
	}
	catch (const char* err) {
		EXPECT_STREQ("Seconds can have value between 0 and 59", err);
	}

	try {
		DateTime dt(30, 4, 2100, 0, 0, 60);
		FAIL() << "Expected: Seconds can have value between 0 and 59";
	}
	catch (const char* err) {
		EXPECT_STREQ("Seconds can have value between 0 and 59", err);
	}
}

TEST(DateTimeTests_Setter, CorrectValues_ForDays) {
	DateTime dt(26,9,2022);

	dt.setDay(1);
	EXPECT_EQ(dt.getDay(), 1);

	dt.setDay(30);
	EXPECT_EQ(dt.getDay(),30);

	dt.setMonth(10);
	dt.setDay(31);
	EXPECT_EQ(dt.getDay(), 31);
}

TEST(DateTimeTests_Setter, CorrectValue_ForMonth) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5, 5};
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(mock);
	
	dt->setMonth(1);
	EXPECT_EQ(dt->getMonth(), 1);

	dt->setMonth(12);
	EXPECT_EQ(dt->getMonth(), 12);

	dt->setMonth(7);
	EXPECT_EQ(dt->getMonth(), 7);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTimeTests_Setter, CorrectValue_ForYear) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5,5 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(mock);

	dt->setYear(2022);
	EXPECT_EQ(dt->getYear(),2022);

	dt->setYear(9999);
	EXPECT_EQ(dt->getYear(), 9999);

	dt->setYear(2050);
	EXPECT_EQ(dt->getYear(), 2050);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTimeTests_Setter, CorrectValue_ForHours) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5,5 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(mock);

	EXPECT_EQ(dt->getDay(), 26);
	EXPECT_EQ(dt->getMonth(), 9);
	EXPECT_EQ(dt->getYear(), 2022);
	EXPECT_EQ(dt->getHours(), 5);
	EXPECT_EQ(dt->getMinutes(), 5);
	EXPECT_EQ(dt->getSeconds(), 5);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);

	dt->setHours(23);
	EXPECT_EQ(dt->getHours(), 23);

	dt->setHours(0);
	EXPECT_EQ(dt->getHours(), 0);

	dt->setHours(13);
	EXPECT_EQ(dt->getHours(), 13);
}

TEST(DateTimeTests_Setter, CorrectValue_ForMinutes) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5,5 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(mock);

	dt->setMinutes(59);
	EXPECT_EQ(dt->getMinutes(), 59);

	dt->setMinutes(0);
	EXPECT_EQ(dt->getMinutes(), 0);

	dt->setMinutes(30);
	EXPECT_EQ(dt->getMinutes(), 30);

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);
}

TEST(DateTimeTests_Setter, CorrectValue_ForSeconds) {
	DateTimeWorkerMock* mock = new DateTimeWorkerMock();

	std::vector<int> date{ 26,9,2022,5, 5,5 };
	EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
	DateTime* dt = new DateTime(mock);

	EXPECT_TRUE(*dt == DateTime(26, 9, 2022, 5, 5, 5));

	Mock::AllowLeak(mock);
	Mock::VerifyAndClear(mock);

	dt->setSeconds(59);
	EXPECT_EQ(dt->getSeconds(), 59);

	dt->setSeconds(0);
	EXPECT_EQ(dt->getSeconds(), 0);

	dt->setSeconds(30);
	EXPECT_EQ(dt->getSeconds(), 30);
}

TEST(DateTimeTests_Setter, InvalidValues_ForDays) {
	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}
}

TEST(DateTimeTests_Setter, InvalidValues_ForDays_30DaysMonth) {
	try {
		DateTime dt(26,4,2040);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 4, 2040);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 4, 2040);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 6, 2040);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 6, 2040);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 6, 2040);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 6, 2040);

		dt.setDay(100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 9, 2040);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 9, 2040);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 9, 2040);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 9, 2040);

		dt.setDay(100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 9, 2040);

		dt.setDay(32);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 11, 2040);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 11, 2040);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 11, 2040);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 11, 2040);

		dt.setDay(32);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}

	try {
		DateTime dt(26, 11, 2040);

		dt.setDay(100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for days", err);
	}
}

TEST(DateTimeTests_Setter, InvalidValues_ForDays_AnyMonth) {
	try {
		DateTime dt(26, 9, 2050);

		dt.setDay(32);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 9, 2050);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 9, 2050);

		dt.setDay(100);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26,10, 2050);

		dt.setDay(32);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 10, 2050);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 10, 2050);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}
}

TEST(DateTimeTests_Setter, InvalidValues_ForDays_February) {
	try {
		DateTime dt(26, 2, 2100);

		dt.setDay(29);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setDay(30);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setDay(0);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2104);

		dt.setDay(30);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2104);

		dt.setDay(31);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}

	try {
		DateTime dt(26, 2, 2104);

		dt.setDay(-1);
		FAIL() << "Expected: Wrong value for days";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for days");
	}
}

TEST(DateTimeTests_Setter, InvalidValues_ForMonth) {
	try {
		DateTime dt(26, 2, 2100);

		dt.setMonth(0);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setMonth(13);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setMonth(100);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}

	try {
		DateTime dt(26, 2, 2100);

		dt.setMonth(-1);
		FAIL() << "Expected: Wrong value for month";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for month");
	}
}

TEST(DateTimeTests_Setter, InvalidValues_ForYear) {
	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock;
		std::vector<int> date{ 26, 2, 2100, 0,0,0 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime dt(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt.setYear(2019);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock;
		std::vector<int> date{ 26, 2, 2100, 0,0,0 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime dt(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt.setYear(-1);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ("Wrong value for year", err);
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock;
		std::vector<int> date{ 26, 2, 2100, 0,0,0 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime dt(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt.setYear(10000);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for year");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock;
		std::vector<int> date{ 26, 2, 2100, 0,0,0 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime dt(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt.setYear(0);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for year");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock;
		std::vector<int> date{ 26, 2, 2100, 0,0,0 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillRepeatedly(Return(date));
		DateTime dt(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt.setYear(-1);
		FAIL() << "Expected: Wrong value for year";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for year");
	}
}

TEST(DateTimeTests_Setter, InvalidValuew_ForHours) {
	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setHours(-1);
		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setHours(24);
		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setHours(60);
		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setHours(100);
		FAIL() << "Expected: Wrong value for hours";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for hours");
	}
}

TEST(DateTimeTests_Setter, InvalidValuew_ForMinutes) {
	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setMinutes(-1);
		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setMinutes(60);
		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setMinutes(99);
		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setMinutes(100);
		FAIL() << "Expected: Wrong value for minutes";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for minutes");
	}
}

TEST(DateTimeTests_Setter, InvalidValuew_ForSeconds) {
	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setSeconds(-1);
		
		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setSeconds(60);
		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setSeconds(99);
		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
	}

	try {
		DateTimeWorkerMock* mock = new DateTimeWorkerMock();

		std::vector<int> date{ 26,9,2022,5, 5,5 };
		EXPECT_CALL(*mock, GetCurrentDateAndTime()).WillOnce(Return(date));
		DateTime* dt = new DateTime(mock);

		Mock::AllowLeak(mock);
		Mock::VerifyAndClear(mock);

		dt->setSeconds(100);
		FAIL() << "Expected: Wrong value for seconds";
	}
	catch (const char* err) {
		EXPECT_STREQ(err, "Wrong value for seconds");
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

