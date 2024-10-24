//
// Created by Phillip on 24/10/2024.
//
#pragma once
#include <string>
#include <vector>
#include <chrono>

static std::string format_duration( std::chrono::milliseconds ms ) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    std::stringstream ss;
    std::string hour_str;
    if (hour.count() < 10)
        hour_str = "0";
    hour_str += std::to_string(hour.count());

    std::string min_str;
    if (mins.count() < 10)
        min_str = "0";
    min_str += std::to_string(mins.count());

    std::string sec_str;
    if (secs.count() < 10)
        sec_str = "0";
    sec_str += std::to_string(secs.count());

    ss << hour_str << ":" << min_str << ":" << sec_str;
    return ss.str();
}


static std::string remaining_str(std::chrono::milliseconds elapsed_ms, int elapsed_sims, int total_sims){

    std::chrono::milliseconds remaining_seconds = (total_sims - elapsed_sims) * elapsed_ms / elapsed_sims;

    return format_duration(remaining_seconds);
}
