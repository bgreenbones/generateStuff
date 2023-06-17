/*
  ==============================================================================

    Note.cpp
    Created: 19 Sep 2022 10:27:15pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Note.hpp"
#include "Dynamics.h"
#include <cmath>

DynamicLevel Note::accentVelocity = fffff;

Sequence<Note> Note::placeOrnament(OrnamentSimple ornamentSimple, double breadth) const {
    Ornament ornament = getOrnament(ornamentSimple, breadth, gridded);
    auto noteLength = (ornament.length / (float) ornament.numNotes);
    
//    vector<Note> ornamentNotes = {};
    Time* parent = (Time*)this;
    Sequence<Note> ornamentNotes(*parent);
    
    for (unsigned short notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
        Note ornamentNote;
        auto offset = ornament.placement * noteLength * notesLeft;
        ornamentNote.startTime = this->startTime + offset;
        ornamentNote.duration = trunc(noteLength * 100.) / 100.; // truncate to avoid overlapping notes
        ornamentNote.isOrnament = true;
        ornamentNote.pitch += P5;
//        ornamentNotes.push_back(ornamentNote);
        ornamentNotes.add(ornamentNote, PushBehavior::ignorePush, OverwriteBehavior::ignoreOverwrite);
    }
    
    
    
//    ornamentNotes.events = applyDynamics(ornamentNotes.events, ornament.dynamics);
    dynamics::shape(ornamentNotes, ornament.dynamics);
    return ornamentNotes;
}


