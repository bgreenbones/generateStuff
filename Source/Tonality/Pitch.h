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

using std::vector;

typedef enum PitchClass {
    C = 0, Db, D, Eb, F, Gb, G, Ab, A, Bb, B
} PitchClass;

static const vector<PitchClass> pitches = { C, Db, D, Eb, F, Gb, G, Ab, A, Bb, B };

typedef enum Interval {
    unison = 0, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7, octave
} Interval;
static const Interval b5 = tritone;
static const Interval s5 = m6;

typedef enum Direction {
    down = -1, up = 1
} Direction;

PitchClass pitchClassIncrement(PitchClass pitch, Interval interval);

class Pitch {
private:
    static int getPitchValue(PitchClass pitchClass, int octave);
public:
    int pitchValue;
    operator int() const;
    Pitch(PitchClass pitchClass, int octave);
    Pitch(int value);
    Pitch();
    
    int getOctave() const;
    PitchClass getPitchClass() const;
    Pitch pitchFromInterval(Interval interval, Direction direction) const;
    Pitch operator+=(Interval interval);
    Pitch operator-=(Interval interval);
    Interval operator-(Pitch other);
};
