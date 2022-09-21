//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Sequence.hpp"
#include <JuceHeader.h>

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
vector<T> Sequence::concatEvents(vector<T> eventList, vector<T> otherList) {
    for (auto iter = otherList.begin(); iter < otherList.end(); iter++) {
        iter->startTime += duration;
        addTimedEvent<T>(*iter, eventList);
    }
    return eventList;
}

Sequence Sequence::concat(Sequence other) {
    Sequence sequence(*this);
    
    sequence.notes = concatEvents<Note>(sequence.notes, other.notes);
    sequence.subdivisions = concatEvents<Subdivision>(sequence.subdivisions, other.subdivisions);
    sequence.duration += other.duration;
    
    return sequence;
} // todo: now do one that concats starting at the end of the last note in the phrase...instead of the whole duration...



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

Sequence parseMininotation(char sequenceString[], Subdivision subdivision) {
    size_t sequenceLength = subdivision.asQuarters() * mininotation::getLength(sequenceString);
    Sequence sequence = Sequence(subdivision, 0, Duration(sequenceLength));
    
    Position startTime = 0;
    for(int i = 0; i < strlen(sequenceString); i++) {
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
