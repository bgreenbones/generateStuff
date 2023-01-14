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

class Subdivision: public Duration, public TimedEvent {
private:

public:
    Subdivision(Duration divisionLength, Position startTime, Duration span):
        Duration(divisionLength), TimedEvent(startTime, span) { };
    Subdivision(Duration divisionLength):
        Duration(divisionLength), TimedEvent(0, Bars(1)) { };
    Subdivision(): Duration(), TimedEvent() {};
    Subdivision(char mininotation, Position startTime, Duration span): Subdivision(Beats(1), startTime, span) {
        if (Mininotation::isNote(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (mininotation == Mininotation::modifiedDefault) {
            this->durationValueInQuarters /= 2.0; // idfk, i guess this is an interpretation.
        }
    }
    
    Subdivision& operator=(Subdivision const other) {
        Duration::operator=(other);
        this->duration = other.duration;
        this->startTime = other.startTime;
        return *this;
    };
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
    
//    Subdivision operator=(const double other) {
//        this->timeSignature = HostSettings::instance().getTimeSignature();
//        this->value = Beats(other).asQuarters();
//        return *this;
//    }
};
