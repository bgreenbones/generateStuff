/*
  ==============================================================================

    ChordScale.cpp
    Created: 19 Mar 2023 5:09:13pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "ChordScale.h"


ChordScale::ChordScale(Tonality scale, Tonality harmony): 
    scale(scale), 
    harmony(harmony) {}
ChordScale::ChordScale(Tonality scale): scale(scale) {
    harmony = scale.scaleToHarmony();
    // TODO: functions for seeing if scale tones are chord tones or not...
}
ChordScale::ChordScale(PitchClass root, vector<Interval> intervals): ChordScale(Tonality(root, intervals)) {};
// ChordScale::ChordScale(PitchClass root, vector<Interval> intervals): ChordScale(root, intervals) {};
// ChordScale::ChordScale(Position startTime, Duration duration): ChordScale(C, ionian) {};
ChordScale::ChordScale(): ChordScale(C, chromatic) {};
// ChordScale::ChordScale(char mininotation, Position startTime, Duration duration): ChordScale(startTime, duration) {}

bool ChordScale::operator==(ChordScale const & other) const {
    return other.scale == scale && other.harmony == harmony;
}

