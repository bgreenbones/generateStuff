/*
  ==============================================================================

    Subdivision.h
    Created: 28 Aug 2022 11:31:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "TimedEvent.h"

class Subdivision: public Duration, public TimedEvent {
private:

public:
    Subdivision(Duration divisionLength, Position startTime, Duration span):
        Duration(divisionLength), TimedEvent(startTime, span) { };
    Subdivision(Duration divisionLength):
        Duration(divisionLength), TimedEvent(0, Bars(1)) { };
    Subdivision(): Duration(), TimedEvent() {}
    
    void updateTimeSignature() {
        timeSignature = HostSettings::instance().getTimeSignature();
        TimedEvent::updateTimeSignature();
    }
//    Subdivision operator=(const double other) {
//        this->timeSignature = HostSettings::instance().getTimeSignature();
//        this->value = Beats(other).asQuarters();
//        return *this;
//    }
};

//class Subdivision {
//public:
//    Subdivision();
//    Subdivision(double frequency);
//    Subdivision(double frequency, duration span);
//    Subdivision(Subdivision const&);
//    Subdivision operator=(Subdivision other);
//    Subdivision operator=(double other);
//    bool operator>(Subdivision other);
//    bool operator>(double other);
//    bool operator>=(Subdivision other);
//    bool operator>=(double other);
//    bool operator==(Subdivision other);
//    bool operator==(double other);
//    bool operator<=(Subdivision other);
//    bool operator<=(double other);
//    bool operator<(Subdivision other);
//    bool operator<(double other);
//    Subdivision operator+(Subdivision other);
//    Subdivision operator+(double other);
//    Subdivision operator-(Subdivision other);
//    Subdivision operator-(double other);
//    Subdivision operator*(Subdivision other);
//    Subdivision operator*(double other);
//    Subdivision operator/(Subdivision other);
//    Subdivision operator/(double other);
//    Subdivision operator++();
//    Subdivision operator--();
//private:
//    double guard(double frequency);
//    double frequency;
//    duration span;
//};
