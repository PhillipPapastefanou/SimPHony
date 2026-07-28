//
// Created by Phillip on 10.03.24.
//

#include "date_time.h"
#include <iostream>

DateTime::DateTime(std::string date_string, std::string format){
    strptime(date_string.c_str(), format.c_str(), &tmt);
    create_time();

}

DateTime::~DateTime() {

}

DateTime::DateTime(const DateTime &dateTime, long seconds) : DateTime(static_cast<time_t>(dateTime.t + seconds)) {
}

DateTime DateTime::AddSeconds(long seconds) {
    // Deliberately epoch-based (t + seconds, then re-derive the calendar
    // fields via the time_t constructor's gmtime_r) rather than adding into
    // tmt.tm_sec and re-normalizing via timegm(). Model::Run() calls this
    // with `ts`, a cumulative offset that reaches into the tens of millions
    // of seconds over a multi-month run -- timegm()'s overflow
    // normalization for a tm_sec that large turned out to be
    // platform-dependent: correct on macOS's libc, but silently stuck
    // (hour/min never advanced) under Emscripten's musl libc, which broke
    // the once-daily stomatal-closure update in solver_indiv_eulerimp.cpp
    // in the WASM build specifically. Epoch arithmetic sidesteps the whole
    // overflow-normalization question.
    return DateTime(static_cast<time_t>(this->t + seconds));
}

DateTime::DateTime(tm tm) {
    tmt = tm;
    create_time();
}

DateTime::DateTime() {
    tm tm_new{};
    tmt = tm_new;
    create_time();
}

void DateTime::print() {
    std::cout << year << "-" << month <<"-"<<day << " ";
    std::cout << hour << ":" << min << ":" << sec << std::endl;
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

bool operator !=(DateTime t1, DateTime t2){
    if(t1.t != t2.t)
        return true;
    return false;
}


void DateTime::populate_fields() {
    year  = tmt.tm_year + 1900;
    month = tmt.tm_mon + 1;
    day   = tmt.tm_mday;
    hour  = tmt.tm_hour;
    min   = tmt.tm_min;
    sec   = tmt.tm_sec;
}

void DateTime::create_time() {
    // tmt holds the wall-clock fields as given (parsed from a UTC-labelled
    // string, or a caller-supplied tm), so timegm() here yields the true UTC
    // epoch. Re-derive tmt below from the CET-shifted epoch so the exposed
    // year/month/day/hour/min/sec fields are in CET, matching the epoch-based
    // constructor below.
    t = timegm(&tmt);
    time_t local = static_cast<time_t>(t + CET_OFFSET_SECONDS);
    gmtime_r(&local, &tmt);
    populate_fields();
}

DateTime::DateTime(time_t epoch_seconds) {
    t = epoch_seconds;
    time_t local = static_cast<time_t>(t + CET_OFFSET_SECONDS);
    gmtime_r(&local, &tmt);
    populate_fields();
}
