/*
  ==============================================================================

    Subdivision.h
    Created: 28 Aug 2022 11:31:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "TimedEvent.h"
//#include "Note.hpp"
#include "Mininotation.h"

class Subdivision: public Duration, public Timed {
private:

public:
    Subdivision(Duration divisionLength, Position startTime, Duration span):
        Duration(divisionLength), Timed(startTime, span) { };
    Subdivision(Duration divisionLength):
        Duration(divisionLength), Timed(0, Bars(1)) { };
    Subdivision(Position startTime, Duration span): Subdivision(Beats(1), startTime, span) { };
    Subdivision(): Duration(0.25), Timed() {};
    Subdivision(char mininotation, Position startTime, Duration span): Subdivision(startTime, span) {
        if (Mininotation::isValue(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (Mininotation::isAlternate(mininotation)) {
            this->durationValueInQuarters /= 2.0; // idfk, i guess this is an interpretation.
        }
    }
    
    Subdivision& operator=(Subdivision other) {
        Duration::operator=(other);
        this->duration = other.duration;
        this->startTime = other.startTime;
        return *this;
    };
    
    bool equalsExcludingTime(Subdivision &other) {
        return this->asQuarters() == other.asQuarters();
    }
    
//    Subdivision operator=(const double other) {
//        this->timeSignature = HostSettings::instance().getTimeSignature();
//        this->value = Beats(other).asQuarters();
//        return *this;
//    }
};
