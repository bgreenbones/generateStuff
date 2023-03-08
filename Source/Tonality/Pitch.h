/*
  ==============================================================================

    Pitch.h
    Created: 23 Jan 2023 10:23:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include <iostream>
#include <vector>
#include "Random.h"

using std::vector, std::cout;

typedef enum PitchClass {
    C = 0, Db, D, Eb, F, Gb, G, Ab, A, Bb, B
} PitchClass;

static const vector<PitchClass> pitches = { C, Db, D, Eb, F, Gb, G, Ab, A, Bb, B };

typedef enum Interval {
    unison = 0, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7, octave
} Interval;
static const Interval b5 = tritone;
static const Interval s5 = m6;
static const vector<Interval> intervals = { unison, m2, M2, m3, M3, P4, tritone, P5, m6, M6, m7, M7, octave };

typedef enum Direction {
    down = -1, up = 1
} Direction;


static const vector<Interval> ionian = { unison, M2, M3, P4, P5, M6, M7 };

static vector<Interval> nthMode(vector<Interval> toModalize, int n) {
    int i = n - 1; // 2nd mode of major is dorian, so we want index = 1 when n = 2, etc.
    if (i < 0 || i >= toModalize.size()) {
        cout << "not good";
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

class Pitch {
private:
    static int getPitchValue(PitchClass pitchClass, int octave) {
        int value = octave * 12 + pitchClass;
        if (value < 0 || value > 127) {
            cout << "this is bad";
        }
        return value;
    }
public:
    int pitchValue;
    operator int() const { return pitchValue; };
    Pitch(PitchClass pitchClass, int octave): pitchValue(getPitchValue(pitchClass, octave)) {}
    Pitch(int value): pitchValue(value) {
        if (value < 0 || value > 127) {
            cout << "this is bad";
        }
    }
    Pitch(): pitchValue(getPitchValue(C, 5)) {}
    
    PitchClass getPitchClass() const {
        int PitchClassValue = pitchValue % 12;
        PitchClass result = PitchClass(PitchClassValue);
        return result;
    }
    Pitch pitchFromInterval(Interval interval, Direction direction) const {
        int newPitchValue = pitchValue + interval * direction;
        return Pitch(newPitchValue);
    }
    Pitch operator+=(Interval interval) {
        pitchValue += interval;
        return *this;
    }
    Pitch operator-=(Interval interval) {
        pitchValue += interval;
        return *this;
    }
};

class Tonality {
public:
    PitchClass root;
    vector<Interval> intervalsUpFromRoot;
    
    Tonality(PitchClass root, vector<Interval> intervals): root(root), intervalsUpFromRoot(intervals) {};
    Tonality(): root(C), intervalsUpFromRoot(ionian) {};
    
    vector<Pitch> getPitches(int octave = 3) {
        octave = (octave < -2 || octave > 8) ? 3 : octave;
        Pitch rootPitch = Pitch(root, octave);
        vector<Pitch> pitches;
        transform(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), back_inserter(pitches),
            [&](Interval &interval) { return rootPitch.pitchFromInterval(interval, up); });
        return pitches;
    }
    
    vector<Pitch> randomVoicing() {
        vector<Pitch> pitches = getPitches();
        vector<Pitch> voicing;
        transform(pitches.begin(), pitches.end(), back_inserter(voicing),
            [&](Pitch &pitch) {
                int octaveChoice = uniformInt(4, 6);
                return Pitch(pitch.getPitchClass(), octaveChoice);
            });
        return voicing;
    }
    
    Tonality getMode(int n) const {
        return Tonality(root, nthMode(intervalsUpFromRoot, n));
    }
};
