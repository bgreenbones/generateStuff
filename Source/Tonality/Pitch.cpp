/*
  ==============================================================================

    Pitch.cpp
    Created: 23 Jan 2023 10:23:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Pitch.h"


int Pitch::getPitchValue(PitchClass pitchClass, int octave) {
    int value = octave * 12 + pitchClass;
    if (value < 0 || value > 127) {
        (void)0; // this is bad
    }
    return value;
}

Pitch::operator int() const { return pitchValue; };

Pitch::Pitch(PitchClass pitchClass, int octave): pitchValue(getPitchValue(pitchClass, octave)) {}

Pitch::Pitch(int value): pitchValue(value) {
    if (value < 0 || value > 127) {
        (void)0; // this is bad
    }
}
Pitch::Pitch(): pitchValue(getPitchValue(C, 5)) {}

int Pitch::getOctave() const { return pitchValue / 12; }

PitchClass Pitch::getPitchClass() const {
    int PitchClassValue = pitchValue % 12;
    PitchClass result = PitchClass(PitchClassValue);
    return result;
}
Pitch Pitch::pitchFromInterval(Interval interval, Direction direction) const {
    int newPitchValue = pitchValue + interval * direction;
    return Pitch(newPitchValue);
}
Pitch Pitch::operator+=(Interval interval) {
    pitchValue += interval;
    return *this;
}
Pitch Pitch::operator-=(Interval interval) {
    pitchValue += interval;
    return *this;
}
