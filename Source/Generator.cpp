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

Playable Generator::cascara() {
    if (!initialize()) { throw exception(); }
    Rhythm r = rhythms.front();
    cascaraSequence = r.randomCascara();
    Playable result = Playable(cascaraSequence, cascaraSequence.phrasing, 1);
    return result;
}

Playable Generator::claveFromCascara() {
    if (!initialize()) { throw exception(); }
    claveSequence = cascaraSequence.rhythm.claveFromCascara(cascaraSequence);
    Playable result = Playable(claveSequence, claveSequence.phrasing, 2);
    return result;
}
