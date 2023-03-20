/*
  ==============================================================================

    Tonality.cpp
    Created: 19 Mar 2023 5:09:13pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Tonality.h"


Tonality::Tonality(PitchClass root, vector<Interval> intervals, Position startTime, Duration duration):
    TimedEvent(startTime, duration), root(root), scale(intervals), harmonyRoot(root) {
        std::sort(scale.begin(), scale.end());
        
        // TODO: initialize harmonyRoot and harmony...
        harmony.push_back(unison);
        if (scale.size() < 2) { return; }
        if (scale.size() < 3) {
            harmony.push_back(scale.at(2));
            return;
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
        
        if (fifths.empty()) { return; }
        harmony.push_back(draw<Interval>(fifths));
        
        // TODO: use harmony data when generating harmony instead of pulling out of tonality.
        // TODO: functions for seeing if scale tones are chord tones or not...
    };
Tonality::Tonality(PitchClass root, vector<Interval> intervals): Tonality(root, intervals, 0, 0) {};
Tonality::Tonality(Position startTime, Duration duration): Tonality(C, ionian, startTime, duration) {};
Tonality::Tonality(): Tonality(C, ionian) {};
Tonality::Tonality(char mininotation, Position startTime, Duration duration): Tonality(startTime, duration) {}

int Tonality::stepHelper(Interval first, Direction direction) const {
    auto tonalityMemberInterval = std::find(scale.begin(), scale.end(), first);
    bool notInTonality = tonalityMemberInterval == scale.end();
    if (notInTonality) {
        auto lessIt = scale.begin();
        while (lessIt + 1 != scale.end() && *(lessIt + 1) < first) { lessIt++; }
        int less = *lessIt < first ? *lessIt : *(scale.end()) - 12;
        int more = lessIt + 1 != scale.end() ? *(lessIt + 1) : *scale.begin() + 12;
        return direction == Direction::up ? more : less;
    }
    
    Interval beginInterval = *scale.begin();
    Interval endInterval = *(scale.end() - 1);

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

vector<Pitch> Tonality::getPitches(int octave) const {
    octave = (octave < -2 || octave > 8) ? 3 : octave;
    Pitch rootPitch = Pitch(root, octave);
    vector<Pitch> pitches;
    transform(scale.begin(), scale.end(), back_inserter(pitches),
        [&](Interval const& interval) { return rootPitch.pitchFromInterval(interval, up); });
    return pitches;
}

vector<Pitch> Tonality::randomVoicing() const {
    vector<Pitch> pitches = getPitches();
    auto chooseOctave = []() { return uniformInt(4, 6); };
    vector<Pitch> voicing;
    for (int i = 0; 2*i < pitches.size(); i++) {
        voicing.push_back(Pitch(pitches.at(2*i).getPitchClass(), chooseOctave()));
    }
    return voicing;
}

Tonality Tonality::getMode(int n) const {
    return Tonality(root, nthMode(scale, n));
}

bool Tonality::equalsExcludingTime(Tonality &other) const {
    if (other.scale.size() != scale.size() || other.harmony.size() != harmony.size()) { return false; }
    bool equals = root == other.root;

    std::sort(other.scale.begin(), other.scale.end());
    for (int i = 0; i < scale.size(); i++) {
        equals = equals && scale.at(i) == other.scale.at(i);
    }

    std::sort(other.harmony.begin(), other.harmony.end());
    for (int i = 0; i < harmony.size(); i++) {
        equals = equals && harmony.at(i) == other.harmony.at(i);
    }

    return equals;
}
