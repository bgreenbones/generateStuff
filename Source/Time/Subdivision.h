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

// TODO: eventually delete this class
class Subdivision: public Timed<Duration> {
private:

public:
    Subdivision(Duration divisionLength, Position startTime, Duration span):
        Timed(Time(startTime, span), divisionLength) { };
    Subdivision(Duration divisionLength):
        Timed(Time(0, Bars(1)), divisionLength) { };
    Subdivision(Position startTime, Duration span): Subdivision(Beats(1), startTime, span) { };
    Subdivision(): Timed(Time(), Duration(0.25)) {};
    Subdivision(char mininotation, Position startTime, Duration span): Subdivision(startTime, span) {
        if (Mininotation::isValue(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (Mininotation::isAlternate(mininotation)) {
            this->item = this->item / 2.0; // idfk, i guess this is an interpretation.
        }
    }
};
