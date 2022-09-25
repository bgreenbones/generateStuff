//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Sequence.hpp"
#include <JuceHeader.h>


void Sequence::updateTimeSignature() {
    TimedEvent::updateTimeSignature();
    for (auto subdiv = subdivisions.begin(); subdiv < subdivisions.end(); subdiv++) {
        subdiv->updateTimeSignature();
    }
    for (auto note = notes.begin(); note < notes.end(); note++) {
        note->updateTimeSignature();
    }
}


template <class T>
void Sequence::addTimedEvent(T toAdd, vector<T>& eventList) {
    eventList.push_back(toAdd);
    sort(eventList.begin(),
         eventList.end(),
         [](T const &a, T const &b) { return a.startTime < b.startTime; });
}


bool Sequence::addNote(Note toAdd) {
    if (toAdd.pitch < 0 || toAdd.pitch > 127) {
        return false;
    }
    //    bool noteFitsInPhrase = note.startTime < (double) phrasing.length();
    bool fitsInPhrase = this->containsPartially(toAdd);
    if (fitsInPhrase) {
        addTimedEvent<Note>(toAdd, notes);
//        notes.push_back(note);
//        sort(notes.begin(), notes.end()); // todo: sort by startTime
    }
    return fitsInPhrase;
//    return addTimedEvent<Note>(toAdd, notes);
}

bool Sequence::addSubdivision(Subdivision toAdd) {
    bool fitsInPhrase = this->containsPartially(toAdd);
    if (fitsInPhrase) {
        addTimedEvent<Subdivision>(toAdd, subdivisions);
    }
    return fitsInPhrase;
}

template <class T>
vector<T> Sequence::concatEvents(vector<T> eventList, vector<T> otherList) const {
    for (auto iter = otherList.begin(); iter < otherList.end(); iter++) {
        iter->startTime += duration;
        addTimedEvent<T>(*iter, eventList);
    }
    return eventList;
}

void Sequence::tieSubdivisions() {
    if (subdivisions.size() <= 1) {
        return;
    }
    bool tryAgain = false;
    vector<Subdivision> newSubdivisions;
    for (auto subdivIt = subdivisions.begin(); subdivIt < subdivisions.end() - 1; subdivIt++) {
        auto otherSubdivIt = subdivIt + 1;
        if (subdivIt->asQuarters() == otherSubdivIt->asQuarters() && subdivIt->endTime() == otherSubdivIt->startTime) {
            newSubdivisions.push_back(Subdivision(subdivIt->asQuarters(), subdivIt->startTime, subdivIt->duration + otherSubdivIt->duration));
            tryAgain = true;
        }
    }
    this->subdivisions = newSubdivisions;
    if (tryAgain) {
        tieSubdivisions();
    }
}

template <class T>
vector<T> chopAfterDuration(vector<T> toChop, Duration duration) {
    if (toChop.empty()) {
        return toChop;
    }
    
    vector<T> chopped;
    copy_if (toChop.begin(),
             toChop.end(),
             back_inserter(chopped),
             [duration](T t){return t.startTime < duration;} );
    
    if (chopped.empty()) {
        return chopped;
    }
    
    T &last = chopped.back();
    if (last.endTime() > duration) {
        last.duration = duration - last.startTime;
    }
    
    return chopped;
}

Sequence Sequence::concat(Sequence other, bool useLastNote, bool keepDuration) const {
    // todo: what if i just want to concat subdivisions?
    // this currently destroys subdivision sequences (but preserves a single lengthy subdivision.
    // I imagine that later, and for other expressions we want to sequence,
    // we will want to have a concat method that can only concat some particular vectors...
    Sequence sequence(*this);
    Duration durationToPersist = sequence.duration;
    
    if (useLastNote) {
        if (sequence.notes.empty()) {
            sequence.duration = 0;
        } else {
            Note lastNote = sequence.notes.back();
            sequence.duration = lastNote.startTime + lastNote.duration;
        }
        
        sequence.subdivisions = chopAfterDuration<Subdivision>(sequence.subdivisions, sequence.duration);
    }
    
    sequence.notes = concatEvents<Note>(sequence.notes, other.notes);
    sequence.subdivisions = concatEvents<Subdivision>(sequence.subdivisions, other.subdivisions);
    sequence.duration += other.duration;
    
    if (keepDuration) {
        sequence.duration = durationToPersist;
        sequence.notes = chopAfterDuration<Note>(sequence.notes, durationToPersist);
        sequence.subdivisions = chopAfterDuration<Subdivision>(sequence.subdivisions, durationToPersist);
        Subdivision &lastSubdiv = sequence.subdivisions.back();
        lastSubdiv.duration = durationToPersist - lastSubdiv.startTime; // keep subdivisions info covering the whole span.
    }
    
    sequence.tieSubdivisions();
    
    return sequence;
}



namespace mininotation {
    char note = 'x';
    char accentNote = 'X';
    char rest = '~';
    char sustain = '.';

    char symbols[4] = {
        note, accentNote, rest, sustain
    };

    char noteSymbols[2] = {
        note, accentNote
    };

    bool isIn(char symbol, char array[]) {
        for (int i = 0; i < strlen(array); i++) {
            if (symbol == array[i]) {
                return true;
            }
        }
        return false;
    }

    bool isInNotation(char symbol) {
        return isIn(symbol, symbols);
    }

    bool isNote(char symbol) {
        return isIn(symbol, noteSymbols);
    }

    size_t getLength(char sequenceString[]) {
        return strlen(sequenceString);
    }
}

Sequence Sequence::parseMininotation(char sequenceString[], Subdivision subdivision) {
    double sequenceLength = subdivision.asQuarters() * (double) mininotation::getLength(sequenceString);
    Sequence sequence = Sequence(subdivision, 0, Duration(sequenceLength));

    Position startTime = 0;
    for(int i = 0; i < mininotation::getLength(sequenceString); i++) {
        char symbol = sequenceString[i];
    
        if (!mininotation::isInNotation(symbol)) {
           DBG ("misuse of mininotation");
        }

        if (mininotation::isNote(symbol)) {
            Note toAdd(startTime, subdivision);
            
            if (symbol == mininotation::accentNote) {
                toAdd = toAdd.accent();
            }
            
            sequence.addNote(toAdd);
        }
        
        if (symbol == mininotation::sustain) {
            if (!sequence.notes.empty()) {
                sequence.notes.back().duration += subdivision;
            }
        }
        
        startTime += subdivision;
    }
    
    return sequence;
}
