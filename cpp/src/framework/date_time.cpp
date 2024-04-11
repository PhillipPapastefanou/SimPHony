//
// Created by Phillip on 10.03.24.
//

#include "date_time.h"

DateTime::DateTime(std::string date_string, std::string format){
    strptime(date_string.c_str(), format.c_str(), &tmt);
    create_time();

}

DateTime::~DateTime() {

}

void DateTime::create_time() {

    t = timegm(&tmt);
    year = tmt.tm_year + 1900;
    month = tmt.tm_mon + 1;
    day = tmt.tm_mday;

    hour = tmt.tm_hour;
    min = tmt.tm_min;
    sec = tmt.tm_sec;

}


DateTime::DateTime(const DateTime &dateTime, long seconds) {

    tmt = dateTime.tmt;
    tmt.tm_sec = dateTime.tmt.tm_sec + seconds;
    create_time();
}

DateTime DateTime::AddSeconds(long seconds) {
    tm tm_new = this->tmt;
    tm_new.tm_sec += seconds;
    return DateTime(tm_new);
}

DateTime::DateTime(tm tm) {
    tmt = tm;
    create_time();
}

DateTime::DateTime() {
    tm tm_new;
    tmt = tm_new;
    create_time();
}

long operator -(DateTime t1, DateTime t2) {
    return t1.t - t2.t;
}

bool operator < (DateTime t1, DateTime t2){
    if(t1.t < t2.t)
        return true;
    return false;
}

bool operator > (DateTime t1, DateTime t2){
    if(t1.t > t2.t)
        return true;
    return false;
}

bool operator ==(DateTime t1, DateTime t2){
    if(t1.t == t2.t)
        return true;
    return false;
}
