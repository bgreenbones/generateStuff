/*
  ==============================================================================

    Subdivision.h
    Created: 28 Aug 2022 11:31:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "TimedEvent.h"
//#include "TimeTypedefs.h"

class Subdivision: public Duration, public TimedEvent {
private:
    bool guard() {
        if ((this->duration % *this) != 0.0) {
            throw exception();
        }
        return true;
    }
public:
    Subdivision(): Duration(), TimedEvent() {}
    Subdivision(Duration divisionLength, Position startTime, Duration span):
        Duration(divisionLength), TimedEvent(startTime, span) { guard(); };
//    Subdivision operator=(const double other): TimedEvent( {
//        this->timeSignature = HostSettings::instance().getTimeSignature();
//        this->value = other;
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
