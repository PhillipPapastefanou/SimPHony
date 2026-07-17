//
// Created by Phillip on 10.03.24.
//
#pragma once
#include <string>
#include <ctime>  // Include this header for the tm structure


class DateTime {

public:
    DateTime();
    DateTime(tm tms);
    DateTime(std::string date_string, std::string format);
    DateTime(const DateTime& dateTime, long seconds);
    explicit DateTime(time_t epoch_seconds);
    DateTime AddSeconds(long seconds);
    ~DateTime();
    void print();

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
    void populate_fields(); 
};

long operator -(DateTime t1, DateTime t2);
bool operator < (DateTime t1, DateTime t2);
bool operator > (DateTime t1, DateTime t2);
bool operator == (DateTime t1, DateTime t2);
bool operator != (DateTime t1, DateTime t2);