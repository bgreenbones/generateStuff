/*
  ==============================================================================

    Note.cpp
    Created: 19 Sep 2022 10:27:15pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Note.hpp"
#include "Dynamics.h"

int Note::accentVelocity = 120;

Sequence<Note> Note::placeOrnament(OrnamentSimple ornamentSimple, double breadth) const {
    double tempo = HostSettings::instance().getTempo();
    Ornament ornament = getOrnament(ornamentSimple, tempo, breadth);
    auto noteLength = (ornament.length / (float) ornament.numNotes);
    
//    vector<Note> ornamentNotes = {};
    TimedEvent* parent = (TimedEvent*)this;
    Sequence<Note> ornamentNotes(*parent);
    
    for (unsigned short notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
        Note ornamentNote = Note();
        auto offset = ornament.placement * noteLength * notesLeft;
        ornamentNote.startTime = this->startTime + offset;
        ornamentNote.duration = noteLength;
        ornamentNote.isOrnament = true;
        ornamentNote.pitch += 7;
//        ornamentNotes.push_back(ornamentNote);
        ornamentNotes.add(ornamentNote); // TODO: sometimes the last note of a drag/ruff don't get placed?
    }
    ornamentNotes.events = applyDynamics(ornamentNotes.events, this->velocity / 4., this->velocity / 2.); // TODO: apply dynamics stored in the ornament's struct
    return ornamentNotes;
}


