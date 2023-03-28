/*
  ==============================================================================

    Tonality.h
    Created: 21 Mar 2023 5:01:59pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Pitch.h"



static const vector<Interval> intervals = { unison, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7, octave };
static const vector<Interval> chromatic = { unison, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7 };
static const vector<Interval> ionian = { unison, M2, M3, P4, P5, M6, M7 };

static vector<Interval> nthMode(vector<Interval> toModalize, int n) {
    int i = n - 1; // 2nd mode of major is dorian, so we want index = 1 when n = 2, etc.
    if (i < 0 || i >= toModalize.size()) {
        // this not good
        return vector<Interval>(); // TODO: handle with optional?
    }
    vector<Interval> nth_mode;
    auto pythonModulo = [](int a, int b) { return (b + (a % b)) % b; }; // python modulo operator always returns a positive value
    transform(toModalize.begin(), toModalize.end(), back_inserter(nth_mode),
        [&](Interval interval) { return Interval(pythonModulo(interval - toModalize[i], 12)); });
    sort(nth_mode.begin(), nth_mode.end());
    return nth_mode;
}

static const vector<Interval> dorian = nthMode(ionian, 2);
static const vector<Interval> phrygian = nthMode(ionian, 3);
static const vector<Interval> lydian = nthMode(ionian, 4);
static const vector<Interval> mixolydian = nthMode(ionian, 5);
static const vector<Interval> aolean = nthMode(ionian, 6);
static const vector<Interval> lochrian = nthMode(ionian, 7);

static const vector<Interval> majorScale = ionian;
static const vector<Interval> minorScale = aolean;
static const vector<Interval> melodicMinorScaleUp = { unison, M2, m3, P4, P5, M6, M7 };
static const vector<Interval> melodicMinorScaleDown = minorScale;
static const vector<Interval> harmonicMinorScale = { unison, M2, m3, P4, P5, m6, M7 };

static const vector<vector<Interval>> diatonicModes = {
  ionian, dorian, phrygian, lydian, mixolydian, aolean, lochrian
};




class Tonality {
public:
    PitchClass root;
    vector<Interval> intervalsUpFromRoot;
    
    Tonality(PitchClass root, vector<Interval> intervals);
    Tonality();
    
private:
    int stepHelper(Interval first, Direction direction) const;
public:
    Pitch step(Pitch first, Direction direction = Direction::up) const;
    Pitch multiStep(Pitch first, int steps, Direction direction = Direction::up) const;
    vector<Pitch> getPitches(int octave = 3) const;
    vector<PitchClass> getPitchClasses() const;
    vector<Pitch> randomVoicing() const;
    Tonality getMode(int n) const;
    Tonality harmonyToScale() const;
    Tonality scaleToHarmony() const;
    double similarity(Tonality other) const;
    Tonality smoothModulation(int n, Direction direction) const;
    Tonality raise(int n) const;
    Tonality lower(int n) const;
    
    bool operator==(Tonality other) const;
};
