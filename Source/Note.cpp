/*
  ==============================================================================

    Note.cpp
    Created: 19 Sep 2022 10:27:15pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Note.hpp"

int Note::accentVelocity = 120;

Sequence<Note> Note::placeOrnament(OrnamentSimple ornamentSimple, double breadth) const {
    double tempo = HostSettings::instance().getTempo();
    Ornament ornament = getOrnament(ornamentSimple, tempo, breadth);
    beats noteLength = ornament.length;
    
//    vector<Note> ornamentNotes = {};
    TimedEvent* parent = (TimedEvent*)this;
    Sequence<Note> ornamentNotes(*parent);
    for (int notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
        Note ornamentNote = Note();
        ornamentNote.startTime = this->startTime + ornament.placement * noteLength * notesLeft;
        ornamentNote.duration = noteLength;
        ornamentNote.velocity = this->velocity / 2.0; // make more dynamic
        ornamentNote.isOrnament = true; // todo: this, probably
        ornamentNote.pitch += 7;
//        ornamentNotes.push_back(ornamentNote);
        ornamentNotes.add(ornamentNote);
    }
    
    return ornamentNotes;
}


