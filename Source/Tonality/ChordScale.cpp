/*
  ==============================================================================

    ChordScale.cpp
    Created: 19 Mar 2023 5:09:13pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "ChordScale.h"


ChordScale::ChordScale(Tonality scale, Position startTime, Duration duration): TimedEvent(startTime, duration), scale(scale) {
    harmony = scale.scaleToHarmony();
    // TODO: functions for seeing if scale tones are chord tones or not...
}
ChordScale::ChordScale(PitchClass root, vector<Interval> intervals, Position startTime, Duration duration): ChordScale(Tonality(root, intervals), startTime, duration) {};
ChordScale::ChordScale(PitchClass root, vector<Interval> intervals): ChordScale(root, intervals, 0, 0) {};
ChordScale::ChordScale(Position startTime, Duration duration): ChordScale(C, ionian, startTime, duration) {};
ChordScale::ChordScale(): ChordScale(C, ionian) {};
ChordScale::ChordScale(char mininotation, Position startTime, Duration duration): ChordScale(startTime, duration) {}

bool ChordScale::equalsExcludingTime(ChordScale &other) const {
    return other.scale == scale && other.harmony == harmony;
}
