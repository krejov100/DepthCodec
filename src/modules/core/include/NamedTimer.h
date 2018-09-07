//
// Created by philip on 8/6/18.
//
#pragma once

#include <string>
#include <boost/timer.hpp>

class NamedTimer{
    std::string mName;
    boost::timer mTimer;
    double mDuration;

public:
    NamedTimer(){}
    NamedTimer(const std::string& name);

    void endTimer();
    void printTimer(std::ostream& ostream) const;
    double getTime() const;
};
