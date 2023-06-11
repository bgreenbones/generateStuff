/*

    Pitch.cpp
    Created: 23 Jan 2023 10:23:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Pitch.h"

Interval invert(Interval toInvert) {
    int octaves = (int)toInvert / (int)octave;
    int negative = (int) toInvert - (octaves * 2 + 1) * (int) octave;
    return (Interval) abs(negative);
}
Interval pitchClassInterval(PitchClass low, PitchClass high) {
    return low > high
        ? (Interval) (12 + high - low)
        : (Interval) (high - low);
}

PitchClass pitchClassIncrement(PitchClass pitch, Interval interval) {
    return Pitch(pitch, 0).pitchFromInterval(interval, Direction::up).getPitchClass();
}

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
Pitch Pitch::operator+=(int interval) {
    pitchValue += interval;
    return *this;
}
Pitch Pitch::operator-=(Interval interval) {
    pitchValue -= interval;
    return *this;
}

Interval Pitch::operator-(Pitch other) {
    return (Interval) abs(pitchValue - other.pitchValue);
}

Pitch Pitch::operator+(int other) {
    return Pitch(this->pitchValue + other);
}

Pitch Pitch::randomInRange(PitchClass pitchClass, Pitch rangeMinimum, Pitch rangeMaximum) {
    Pitch minimum = Pitch(pitchClass, rangeMinimum.getOctave());
    if (minimum < rangeMinimum) { minimum += octave; }
    Pitch maximum = Pitch(pitchClass, rangeMaximum.getOctave());
    if (maximum > rangeMaximum) { maximum -= octave; }
    int octave = uniformInt(minimum.getOctave(), maximum.getOctave());
    return Pitch(pitchClass, octave);
}

    // void Pitch::makeCloser(Pitch other, Tonality tonality) {
        
    //     }
    void Pitch::within(Pitch const& other, Interval interval) {
        if (interval < tritone) { interval = tritone; }
        while (*this - other > interval) {
            makeCloserKeepPitchClass(other, interval);
        }
    }
    void Pitch::makeCloserKeepPitchClass(Pitch const& other, Probability maybe, Interval interval) {
        if (interval < tritone) { interval = tritone; }
        if (*this - other > interval) {
            if (*this > other) {
                pitchValue -= octave;
            } else {
                pitchValue += octave;
            }
        }
    }
    // void Pitch::maybeMakeCloser(Pitch other, Probability maybe = 0.6);
