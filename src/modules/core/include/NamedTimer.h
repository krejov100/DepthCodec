//
// Created by philip on 8/6/18.
//

#ifndef CAPTURE_TRIANGULATION_NAMEDTIMER_H
#define CAPTURE_TRIANGULATION_NAMEDTIMER_H

#include <string>
#include <boost/timer.hpp>

class NamedTimer{
    std::string mName;
    boost::timer mTimer;
    double mDuration;

public:
    NamedTimer(){}
    NamedTimer(const std::string& name);

    void endTimer() noexcept;
    void printTimer(std::ostream& ostream) const;
    double getTime() const noexcept;
};
#endif //CAPTURE_TRIANGULATION_NAMEDTIMER_H
