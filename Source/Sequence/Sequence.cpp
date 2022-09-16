//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Sequence.hpp"



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
