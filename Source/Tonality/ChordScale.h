/*
  ==============================================================================

    ChordScale.h
    Created: 19 Mar 2023 5:09:13pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Tonality.h"
#include "TimedEvent.h"

class ChordScale: public TimedEvent {
public:
    Tonality scale;
    Tonality harmony;

    ChordScale(PitchClass root, vector<Interval> intervals, Position startTime, Duration duration);
    ChordScale(PitchClass root, vector<Interval> intervals);
    ChordScale(Position startTime, Duration duration);
    ChordScale();
    ChordScale(char mininotation, Position startTime, Duration duration);
    
public:
    bool equalsExcludingTime(ChordScale &other) const;
};
