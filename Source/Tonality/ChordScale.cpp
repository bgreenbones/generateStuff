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
ChordScale::ChordScale(): ChordScale(Tonality(C, chromatic)) {};

bool ChordScale::operator==(ChordScale const & other) const {
    return other.scale == scale && other.harmony == harmony;
}

