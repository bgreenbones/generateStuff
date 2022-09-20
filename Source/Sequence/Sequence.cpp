//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Sequence.hpp"
#include <JuceHeader.h>



bool Sequence::addNote(Note note) {
    if (note.pitch < 0 || note.pitch > 127) {
        return false;
    }
    //    bool noteFitsInPhrase = note.startTime < (double) phrasing.length();
    bool noteFitsInPhrase = this->containsPartially(note);
    if (noteFitsInPhrase) {
        notes.push_back(note);
        sort(notes.begin(), notes.end()); // todo: sort by startTime
    }
    return noteFitsInPhrase;
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
