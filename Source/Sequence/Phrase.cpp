//
//  Phrase.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Phrase.hpp"
#include <JuceHeader.h>

template <class T>
void Phrase::addTimedEvent(T toAdd, vector<T>& eventList) {
    eventList.push_back(toAdd);
    sort(eventList.begin(),
         eventList.end(),
         [](T const &a, T const &b) { return a.startTime < b.startTime; });
}

bool Phrase::addNote(Note toAdd) {
    if (toAdd.pitch < 0 || toAdd.pitch > 127) {
        return false;
    }
    
    bool fitsInPhrase = this->containsPartially(toAdd);
    if (fitsInPhrase) {
        notes.add(toAdd);
    }
    return fitsInPhrase;
}

bool Phrase::addSubdivision(Subdivision toAdd) {
    bool fitsInPhrase = this->containsPartially(toAdd);
    if (fitsInPhrase) {
        subdivisions.add(toAdd);
    }
    return fitsInPhrase;
}

// TODO: call through to Sequence method. or actually this just shouldn't exist.
template <class T>
vector<T> Phrase::concatEvents(vector<T> eventList, vector<T> otherList) const {
    for (auto iter = otherList.begin(); iter < otherList.end(); iter++) {
        iter->startTime += duration;
        addTimedEvent<T>(*iter, eventList);
    }
    return eventList;
}

// TODO: call through to Sequence method?? or actually this just shouldn't exist.
void Phrase::tieSubdivisions() {
    if (subdivisions.size() <= 1) {
        return;
    }
    subdivisions.tie();
//    bool tryAgain = false;
//    vector<Subdivision> newSubdivisions;
//    for (auto subdivIt = subdivisions.begin(); subdivIt < subdivisions.end() - 1; subdivIt++) {
//        auto otherSubdivIt = subdivIt + 1;
//        if (subdivIt->asQuarters() == otherSubdivIt->asQuarters() && subdivIt->endTime() == otherSubdivIt->startTime) {
//            newSubdivisions.push_back(Subdivision(subdivIt->asQuarters(), subdivIt->startTime, subdivIt->duration + otherSubdivIt->duration));
//            tryAgain = true;
//        }
//    }
//    if (tryAgain) {
//        this->subdivisions = newSubdivisions;
//        tieSubdivisions();
//    }
}

// TODO: call through to Sequence methods? or actually this just shouldn't exist...
template <class T>
vector<T> chopAfterDuration(vector<T> toChop, Duration duration) {
    if (toChop.empty()) {
        return toChop;
    }
    
    vector<T> filtered;
    copy_if (toChop.begin(),
             toChop.end(),
             back_inserter(filtered),
             [duration](T &t) { return t.startTime <= duration; });
    
    
    
    if (filtered.empty()) {
        return filtered;
    }
    
    vector<T> chopped;
    transform(filtered.begin(),
              filtered.end(),
              back_inserter(chopped),
              [duration](T &t) {
                bool tooLong = t.endTime() > duration;
                if (tooLong) {
                    t.duration =  duration - t.startTime;
                }
                return t; });
    
    return chopped;
}

Phrase Phrase::concat(Phrase other, bool useLastNote, bool keepDuration) const {
    // todo: what if i just want to concat subdivisions?
    // this currently destroys subdivision phrases (but preserves a single lengthy subdivision.
    // I imagine that later, and for other expressions we want to phrase,
    // we will want to have a concat method that can only concat some particular vectors...
    Phrase phrase(*this);
    Duration durationToPersist = phrase.duration;
    
    
    // // // // // // /// Sequence version.
    phrase.notes.concat(other.notes, useLastNote);
    phrase.subdivisions.concat(other.subdivisions, useLastNote);
    if (keepDuration) {
        phrase.notes.chopAfterDuration(durationToPersist);
        phrase.subdivisions.chopAfterDuration(durationToPersist);
    }
    phrase.subdivisions.tie();
    // // // // // // ///
    
    return phrase;
}

Phrase Phrase::insert(Phrase other, bool overwrite) const {
    Phrase phrase(*this);
    
    phrase.notes.insert(other.notes, Position(0), PushBehavior::ignore, overwrite);
    phrase.subdivisions.insert(other.subdivisions, Position(0), PushBehavior::ignore, overwrite);
    phrase.subdivisions.tie();
    
    return phrase;
}


// TODO: we can actually optionally get subdivs from the subdivs sequence??
Phrase Phrase::parseMininotation(std::string phraseString, Subdivision subdivision) {
    double phraseLength = subdivision.asQuarters() * (double) Mininotation::getLength(phraseString);
    Phrase phrase = Phrase(subdivision, 0, Quarters(phraseLength));
    phrase.notes = notes.parseMininotation(phraseString, subdivision);
    return phrase;
}



//template class Sequence<Phrase>;

