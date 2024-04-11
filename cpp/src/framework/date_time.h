//
// Created by Phillip on 10.03.24.
//
#pragma once
#include <string>



class DateTime {

public:
    DateTime();
    DateTime(tm tms);
    DateTime(std::string date_string, std::string format);
    DateTime(const DateTime& dateTime, long seconds);
    DateTime AddSeconds(long seconds);
    ~DateTime();

    int year;
    int month;
    int day;

    int hour;
    int min;
    int sec;

    tm tmt;
    time_t t;

private:
    void create_time();
};

long operator -(DateTime t1, DateTime t2);
bool operator < (DateTime t1, DateTime t2);
bool operator > (DateTime t1, DateTime t2);
bool operator == (DateTime t1, DateTime t2);