//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"



bool Generator::initialized() {
    return this->sequences.size() > 0 &&
            this->rhythms.size() > 0 &&
            this->phrases.size() > 0;
}

template<typename T> // todo: put in common utility module?
//void initializeCollection(vector<reference_wrapper<T>> &collection) {
void initializeCollection(vector<T> &collection) {
    if (collection.size() <= 0) {
        T t = T();
        collection.push_back(t);
    }
    return;
}

//vector<reference_wrapper<Sequence>> Generator::initialSequences() {
vector<Sequence> Generator::initialSequences() {
    return { cascaraSequence, claveSequence };
};

bool Generator::initialize() {
    if (this->initialized()) { return true; }
    sequences = initialSequences();
    initializeCollection(this->rhythms);
    initializeCollection(this->phrases);
    return initialized();
}

bool Generator::setPhraseLengthBars(const float bars) {
    if (phrases.size() <= 0) { return false; } // idk, maybe it could just initialize
    phrases[0].bars = bars;
    return true;
}

bool Generator::setPhraseLengthBeats(const float beats) {
    if (phrases.size() <= 0) { return false; } // idk, maybe it could just initialize
    phrases[0].beats = beats;
    return true;
}

bool Generator::setTimeSignature(const int numerator, const int denominator) {
    if (phrases.size() <= 0) { return false; } // idk, maybe it could just initialize
    juce::AudioPlayHead::TimeSignature timeSignature = juce::AudioPlayHead::TimeSignature();
    timeSignature.numerator = numerator;
    timeSignature.denominator = denominator;
    phrases[0].timeSignature = timeSignature;
    return true;
}

juce::AudioPlayHead::TimeSignature Generator::getTimeSignature() {
    if (!initialize()) { throw exception(); }
    return phrases[0].timeSignature;
}

Playable Generator::cascara() {
    if (!initialize()) { throw exception(); }
    Rhythm r = rhythms.front();
    Phrase p = phrases.front();
    cascaraSequence = Sequence(r, p);
    cascaraSequence = r.randomCascara(cascaraSequence);
    // todo: check sequences vector to see if this updates those references
    Playable result = Playable(cascaraSequence, cascaraSequence.phrasing, 1);
    return result;
}

Playable Generator::claveFromCascara() {
    if (!initialize()) { throw exception(); }
    claveSequence = cascaraSequence.rhythm.claveFromCascara(cascaraSequence);
    Playable result = Playable(claveSequence, claveSequence.phrasing, 2);
    return result;
}
