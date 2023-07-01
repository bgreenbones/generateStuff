/*
  ==============================================================================

    Tonality.h
    Created: 21 Mar 2023 5:01:59pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Pitch.h"

class Note;

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

static const vector<Interval> majorTriad = { unison, M3, P5 };
static const vector<Interval> minorTriad = { unison, m3, P5 };
static const vector<Interval> diminishedTriad = { unison, m3, b5 };
static const vector<Interval> augmentedTriad = { unison, M3, s5 };


static const vector<vector<Interval>> augmentedAndDiminished = {
  diminishedTriad, augmentedTriad  
};

static const vector<Interval> major7chord = { unison, M3, P5, M7 };
static const vector<Interval> minor7chord = { unison, m3, P5, m7 };
static const vector<Interval> majorAdd2 = { unison, M2, M3, P5 };
static const vector<Interval> minorAdd2 = { unison, M2, m3, P5 };
static const vector<Interval> majorAdd4 = { unison, M3, P4, P5 };
static const vector<Interval> minorAdd4 = { unison, m3, P4, P5 };
static const vector<Interval> major6chord = { unison, M3, P5, M6};
static const vector<Interval> minor6chord = { unison, m3, P5, M6 };

static const vector<vector<Interval>> majorMinorOneColorTone = {
  major7chord, minor7chord, majorAdd2, minorAdd2, majorAdd4, minorAdd4, major6chord, minor6chord
};

static const vector<Interval> major9chord = { unison, M3, P5, M7, M9 };
static const vector<Interval> minor9chord = { unison, m3, P5, m7, M9 };

namespace voicing {
  double crunch(vector<Pitch> const& voicing);
  void decreaseCrunch(vector<Pitch> & voicing, PitchRange range);
  void decreaseSpread(vector<Pitch> & voicing);
  void preventRumble(vector<Pitch> & voicing);
}

class Tonality {
public:
    PitchClass root;
    vector<Interval> intervalsUpFromRoot;
    
    Tonality(PitchClass root, vector<Interval> intervals);
    Tonality();
    
    static const Pitch chordsHigh;
    static const Pitch chordsLow;
private:
    int stepHelper(Interval first, Direction direction) const;
public:
    vector<Note> quantize(vector<Note> toQuantize) const; 
    Pitch quantize(Pitch toQuantize) const;
    Pitch step(Pitch first, Direction direction = Direction::up) const;
    Pitch multiStep(Pitch first, int steps, Direction direction = Direction::up) const;
    vector<Pitch> getPitches(int octave = 3) const;
    vector<PitchClass> getPitchClasses() const;
    vector<Pitch> randomVoicing(PitchRange range = { chordsLow, chordsHigh }) const;
    vector<Pitch> smoothVoicing(vector<Pitch> lastVoicing, PitchRange range = { chordsLow, chordsHigh }) const;
    Tonality getMode(int n) const;
    Tonality harmonyToScale() const;
    Tonality scaleToHarmony() const;
    double similarity(Tonality other) const;
    bool includes(vector<Interval> other) const;
    bool includes(Tonality other) const;
    bool containsPitch(Pitch pitch) const;
    Tonality smoothModulation(int n, Direction direction, vector<vector<Interval>> limitChordQualities = {}) const;
    vector<Interval> getModulationOptions(Direction direction) const;
    Tonality raise(int n) const;
    Tonality lower(int n) const;
    Tonality newRootSameTonality(PitchClass newRoot) const;
    
    bool operator==(Tonality other) const;
    bool operator!=(Tonality other) const;
};
