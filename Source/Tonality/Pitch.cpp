/*

    Pitch.cpp
    Created: 23 Jan 2023 10:23:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Pitch.h"

PitchRange attenuatePitchRange(PitchRange toAttenuate, float treble, float bass) {
    int center = (toAttenuate.low + toAttenuate.high) / 2;
    int highPitchMin = center + 5;
    int lowPitchMax = center - 6;

    Pitch newHighPitch = (toAttenuate.high - toAttenuate.low) * treble + highPitchMin;
    Pitch newLowPitch = lowPitchMax - (lowPitchMax - toAttenuate.low) * bass;

    return { newLowPitch, newHighPitch };
}

Interval intervalDifference(Interval large, Interval small) {
    int result = large - small;
    if (result < IntervalLowerBound) {
        return IntervalLowerBound;
    }
    if (result > IntervalUpperBound) {
        return IntervalUpperBound;
    }
    return (Interval) result;
}
Interval invert(Interval toInvert) {
    int octaves = (int)toInvert / (int)octave;
    int negative = (int) toInvert - (octaves * 2 + 1) * (int) octave;
    return (Interval) abs(negative);
}
Interval pitchClassInterval(PitchClass low, PitchClass high) {
    return low > high
        ? (Interval) (12 + (int)high - (int)low)
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
Pitch Pitch::operator-=(int interval) {
    pitchValue -= interval;
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
Pitch Pitch::operator-(int other) {
    return Pitch(this->pitchValue - other);
}

Pitch Pitch::randomInRange(PitchClass pitchClass, PitchRange range) {
    return draw<Pitch>(Pitch(pitchClass, 0).inRange(range));
}
void Pitch::keepInRange(PitchRange range) {
    while (*this < range.low) { *this += octave; }
    while (*this > range.high) { *this -= octave; }
}

vector<Pitch> Pitch::inRange(PitchRange range) {
    vector<Pitch> result;
    Pitch cursor(this->getPitchClass(), 0);
    cursor.keepInRange(range);
    while (cursor >= range.low && cursor <= range.high) {
        result.push_back(cursor);
        cursor += octave;
    }
    return result;
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

double Pitch::gravity(PitchRange range) {
  double rangeWidth = range.high - range.low;
  Pitch center = rangeWidth / 2. + range.low;
  return (pitchValue - center.pitchValue) / (rangeWidth / 2.);
}
