//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Time/Rhythm.hpp"
//#include "Phrase.hpp"
#include "Sequence.hpp"

Sequence& Sequence::operator=(Sequence other) {
//    notes = other.notes;
//    rhythm = other.rhythm;
//    phrasing = other.phrasing;
    swap(notes, other.notes);
    swap(rhythm, other.rhythm);
    swap(phrasing, other.phrasing);
    return *this;
};
Sequence::Sequence() {
    this->rhythm = Rhythm();
    this->phrasing = Phrase();
}
Sequence::Sequence(Rhythm rhythm, Phrase phrasing): rhythm(rhythm), phrasing(phrasing) {}

//Note Phrase::firstNote() {
//    sort(mNotes.begin(), mNotes.end());
//    return mNotes.at(0);
//}

bool Sequence::addNote(Note note) {
    if (note.pitch < 0 || note.pitch > 127) {
        return false;
    }
    bool noteFitsInPhrase = note.startTime < phrasing.length();
    if (noteFitsInPhrase) {
        notes.push_back(note);
        sort(notes.begin(), notes.end()); // todo: sort by startTime
    }
    return noteFitsInPhrase;
}
