//
//  Sequence.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Rhythm/Rhythm.hpp"
//#include "Phrase.hpp"
#include "Sequence.hpp"

Sequence& Sequence::operator=(Sequence other) {
    swap(notes, other.notes);
    swap(rhythm, other.rhythm);
    swap(phrasing, other.phrasing);
    return *this;
};
Sequence::Sequence(): rhythm(Rhythm::defaultRhythm), phrasing(Phrase::defaultPhrase) {}
Sequence::Sequence(Rhythm &rhythm, Phrase &phrasing): rhythm(rhythm), phrasing(phrasing) {}

//Note Phrase::firstNote() {
//    sort(mNotes.begin(), mNotes.end());
//    return mNotes.at(0);
//}

//bool addNote(Note note); // todo: implement. keep mNotes in order of note time.
