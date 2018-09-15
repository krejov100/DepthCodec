//
// Created by philip on 8/6/18.
//

#include "NamedTimer.h"
#include <boost/log/trivial.hpp>
#include <boost/timer.hpp>

NamedTimer::NamedTimer(const std::string& name):mName(name){};

void NamedTimer::endTimer(){
    mDuration = mTimer.elapsed();
}

void NamedTimer::printTimer(std::ostream& ostream) const{
    ostream << "Time to " << mName << ": " << mDuration << " fps: " << 1.0/mDuration << std::endl;
}
double NamedTimer::getTime() const {return mDuration;};
