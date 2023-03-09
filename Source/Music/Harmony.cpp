/*
  ==============================================================================

    Harmony.cpp
    Created: 8 Mar 2023 10:50:55pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Harmony.h"



Tonality harmony::randomTonality() {
    PitchClass root = draw<PitchClass>(pitches);
    Interval third = draw<Interval>({ m3, M3 });
    Interval fifth = P5; // draw<Interval>({ b5, P5 });
    Interval seventh = draw<Interval>({ m7, M7 });
    Tonality harm;
    harm.root = root;
    harm.intervalsUpFromRoot = { unison, third, fifth, seventh };
    return harm;
}


vector<Pitch> harmony::randomChord() { return randomTonality().randomVoicing(); }
