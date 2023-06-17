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

class ChordScale {
public:
    Tonality scale;
    Tonality harmony;

    // ChordScale(Tonality scale, Tonality harmony, Position startTime, Duration duration);
    ChordScale(Tonality scale, Position startTime, Duration duration);
    // ChordScale(PitchClass root, vector<Interval> intervals, Position startTime, Duration duration);
    ChordScale(Tonality scale, Tonality harmony);
    ChordScale(Tonality scale);
    // ChordScale(PitchClass root, vector<Interval> intervals);
    ChordScale(PitchClass root, vector<Interval> intervals);
    // ChordScale(Position startTime, Duration duration);
    ChordScale();
    // ChordScale(char mininotation, Position startTime, Duration duration);
    
public:
    bool operator==(ChordScale const & other) const;
};
