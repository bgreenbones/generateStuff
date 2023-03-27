/*
  ==============================================================================

    Tonality.cpp
    Created: 21 Mar 2023 5:01:59pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Tonality.h"
#include "Utility.h"



Tonality::Tonality(PitchClass root, vector<Interval> intervals): root(root), intervalsUpFromRoot(intervals) {
        std::sort(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end());
    };
Tonality::Tonality(): Tonality(C, ionian) {};

int Tonality::stepHelper(Interval first, Direction direction) const {
    auto tonalityMemberInterval = std::find(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), first);
    bool notInTonality = tonalityMemberInterval == intervalsUpFromRoot.end();
    if (notInTonality) {
        auto lessIt = intervalsUpFromRoot.begin();
        while (lessIt + 1 != intervalsUpFromRoot.end() && *(lessIt + 1) < first) { lessIt++; }
        int less = *lessIt < first ? *lessIt : *(intervalsUpFromRoot.end()) - 12;
        int more = lessIt + 1 != intervalsUpFromRoot.end() ? *(lessIt + 1) : *intervalsUpFromRoot.begin() + 12;
        return direction == Direction::up ? more : less;
    }
    
    Interval beginInterval = *intervalsUpFromRoot.begin();
    Interval endInterval = *(intervalsUpFromRoot.end() - 1);

    bool wrapDownward = direction == Direction::down && *tonalityMemberInterval == beginInterval;
    if (wrapDownward) { return endInterval - 12; }
    bool wrapUpward = direction == Direction::up && *tonalityMemberInterval == endInterval;
    if (wrapUpward) { return beginInterval + 12; }
    
    return *(tonalityMemberInterval + direction * 1);
}


Pitch Tonality::step(Pitch first, Direction direction) const {
    Interval firstInterval = root > first.getPitchClass()
        ? (Interval) (12 + first.getPitchClass() - root)
        : (Interval) (first.getPitchClass() - root);

    return Pitch(first.pitchValue + stepHelper(firstInterval, direction) - firstInterval);
}

Pitch Tonality::multiStep(Pitch first, int steps, Direction direction) const {
    Pitch newPitch(first);
    while (steps-- > 0) {
        newPitch = step(newPitch, direction);
    }
    return newPitch;
}

vector<Pitch> Tonality::getPitches(int octave) const {
    octave = (octave < -2 || octave > 8) ? 3 : octave;
    Pitch rootPitch = Pitch(root, octave);
    vector<Pitch> pitches;
    transform(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), back_inserter(pitches),
        [&](Interval const& interval) { return rootPitch.pitchFromInterval(interval, up); });
    return pitches;
}

vector<Pitch> Tonality::randomVoicing() const {
    vector<Pitch> pitches = getPitches();
    vector<Pitch> voicing;
    transform(pitches.begin(), pitches.end(), back_inserter(voicing), [](Pitch pitch) {
        int octave = uniformInt(4, 6);
        return Pitch(pitch.getPitchClass(), octave);
    });
    return voicing;
}

Tonality Tonality::getMode(int n) const {
    return Tonality(root, nthMode(intervalsUpFromRoot, n));
}

bool Tonality::operator==(Tonality other) const {
    if (other.intervalsUpFromRoot.size() != intervalsUpFromRoot.size()) { return false; }
    bool equals = root == other.root;

    std::sort(other.intervalsUpFromRoot.begin(), other.intervalsUpFromRoot.end());
    for (int i = 0; i < intervalsUpFromRoot.size(); i++) {
        equals = equals && intervalsUpFromRoot.at(i) == other.intervalsUpFromRoot.at(i);
    }

    return equals;
}

Tonality Tonality::harmonyToScale() const {
    return Tonality(root, {});
}

Tonality Tonality::scaleToHarmony() const {
    vector<Interval> const& scale = intervalsUpFromRoot;
    vector<Interval> harmony;
    harmony.push_back(unison);
    if (scale.size() < 2) { return Tonality(root, harmony); }
    if (scale.size() < 3) {
        harmony.push_back(scale.at(2));
        return Tonality(root, harmony);;
    }
    
    auto isThird = [](Interval interval) { return interval == m3 || interval == M3; };
    vector<Interval> thirds = filter<Interval>(scale, isThird);
    auto isSus = [](Interval interval) { return interval == M2 || interval == P4; };
    vector<Interval> suspensions = filter<Interval>(scale, isSus);
    
    Interval third = thirds.size() > 0 ? draw<Interval>(thirds) :
        suspensions.size() > 0 ? draw<Interval>(suspensions) :
        scale.at(2);

    
    harmony.push_back(third);
    
    auto isFifth = [](Interval interval) { return interval == P5 || interval == tritone; };
    vector<Interval> fifths = filter<Interval>(scale, isFifth);
    
    if (fifths.empty()) { return Tonality(root, harmony); }
    harmony.push_back(draw<Interval>(fifths));
    
    return Tonality(root, harmony);
}

vector<PitchClass> Tonality::getPitchClasses() const {
    vector<Pitch> pitches = getPitches();
    vector<PitchClass> pitchClasses;
    
    transform(pitches.begin(), pitches.end(), back_inserter(pitchClasses),
        [](Pitch pitch) { return pitch.getPitchClass(); });
    
    sort(pitchClasses.begin(), pitchClasses.end());
    return pitchClasses;
}

double Tonality::similarity(Tonality other) const {
    double containedInOther = 0; // will be 1. if all pitch classes in this are also in other
    double containedInThis = 0; // will be 1. if all pitch classes in other are also in this
    // will be < 1. (but >= 0) if some are missing
    
    for(PitchClass inThis : this->getPitchClasses()) {
        bool isInOther = false;
        for(PitchClass inOther : other.getPitchClasses()) {
            isInOther = isInOther || inThis == inOther;
        }
        containedInOther += isInOther ? 1. : 0.;
    }
    containedInOther /= this->getPitchClasses().size();
    
    for(PitchClass inOther : other.getPitchClasses()) {
        bool isInThis = false;
        for(PitchClass inThis : this->getPitchClasses()) {
            isInThis = isInThis || inThis == inOther;
        }
        containedInThis += isInThis ? 1. : 0.;
    }
    containedInThis /= other.getPitchClasses().size();

    return (containedInThis + containedInOther) / 2.; // return average of two contain measures. 
}


