/*
  ==============================================================================

    Pitch.h
    Created: 23 Jan 2023 10:23:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include <vector>
#include "Random.h"
#include "Probability.h"

using std::vector;

typedef enum PitchClass {
    C = 0, Db, D, Eb, F, Gb, G, Ab, A, Bb, B
} PitchClass;

static const vector<PitchClass> pitches = { C, Db, D, Eb, F, Gb, G, Ab, A, Bb, B };

typedef enum Interval {
    nm2 = -1, nM2 = -2, nm3 = -3, nM3 = -4, nP4 = -5, ntritone = -6, 
    nP5 = -7, nm6 = -8, nM6 = -9, nm7 = -10, nM7 = -11, noctave = -12,
    nm9 = -13, nM9 = -14, nm10 = -15, nM10 = -16, nP11 = -17, nM11 = -18,
    nP12 = -19, nm13 = -20, nM13 = -21, nm14 = -22, nM14 = -23, ndoubleOctave = -24, 
    IntervalLowerBound = -25,
    unison = 0, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7,
    octave, m9, M9, m10, M10, P11, M11, P12, m13, M13, m14, M14,
    doubleOctave, m16, M16, m17, M17, P18, M18, P19, m20, M20, m21, M21,
    tripleOctave, m23, M23, m24, M24, P25, M25, P26, m27, M27, m28, M28,
    quadOctave, IntervalUpperBound
} Interval;
static const Interval b5 = tritone;
static const Interval s5 = m6;

Interval intervalDifference(Interval large, Interval small);

typedef enum Direction {
    down = -1, up = 1
} Direction;

Interval pitchClassInterval(PitchClass low, PitchClass high);
PitchClass pitchClassIncrement(PitchClass pitch, Interval interval);
Interval invert(Interval ToInvert);

class Pitch {
private:
    static int getPitchValue(PitchClass pitchClass, int octave);
public:
    int pitchValue;
    static const int max = 127;
    static const int min = 0;
    operator int() const;
    Pitch(PitchClass pitchClass, int octave);
    Pitch(int value);
    Pitch();
    static Pitch randomInRange(PitchClass pitchClass, Pitch minimum, Pitch maximum);
    
    int getOctave() const;
    PitchClass getPitchClass() const;
    Pitch pitchFromInterval(Interval interval, Direction direction) const;
    Pitch operator+=(int interval);
    Pitch operator-=(int interval);
    Pitch operator+=(Interval interval);
    Pitch operator-=(Interval interval);
    Interval operator-(Pitch other);
    Pitch operator+(int other);
    Pitch operator-(int other);

    void within(Pitch const& other, Interval interval);
    void makeCloserKeepPitchClass(Pitch const& other, Probability maybe = 0.6, Interval interval = tritone);
    void keepInRange(Pitch rangeMinimum, Pitch rangeMaximum);
    double gravity(Pitch rangeMax = Pitch::max, Pitch rangeMin = Pitch::min);

    // void makeCloser(Pitch other);
    // void maybeMakeCloser(Pitch other, Probability maybe = 0.6);
};
