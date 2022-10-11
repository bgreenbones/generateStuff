/*
  ==============================================================================

    Ornamentation.cpp
    Created: 14 Sep 2022 12:27:34am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"
#include "Syncopation.h"

Phrase Phrase::addOrnaments(vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities) const{
    bool isAlreadyOrnamented = false; // todo: know this somehow
    if (isAlreadyOrnamented) {
        throw exception();
    }
    Phrase ornamented = (*this);
    for (auto noteIt = notes.begin(); noteIt < notes.end(); noteIt++) {
        if (noteIt->ornamented) {
            OrnamentSimple ornament = possibleOrnaments[rand() % possibleOrnaments.size()]; // todo: use probabilities map
            vector<Note> ornamentNotes = placeOrnamentSimple(*noteIt, ornament);
            for_each(ornamentNotes.begin(), ornamentNotes.end(), [&](Note toAdd) -> void { ornamented.addNote(toAdd); });
        }
    }
    return ornamented;
}

Phrase Phrase::addOrnaments(OrnamentSimple ornament, float tempo) const {
    return addOrnaments(vector<OrnamentSimple> { ornament }, tempo);
}



vector<Note> roll(Duration length, Subdivision subdivision) {
    double numNotesInFill = length.asQuarters() / subdivision.asQuarters();
    if (std::fmod(numNotesInFill, 1.) != 0) {
        DBG ("something's wrong");
    }
    
    vector<Note> roll = Mininotation::parse<Note>(std::string(floor(numNotesInFill), 'x'), subdivision);
    for (Note &note : roll) {
        note.velocity = 20; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
    }
    return roll;
}

Phrase Phrase::withRoll(Position start, Position target) const {
    if (start >= target) {
        DBG ("bad input to this guy");
    }
    
    Phrase withRoll(*this);
    
    
    auto availableSubdivisions = subdivisions.byPosition(std::fmod(start.asQuarters(), this->duration.asQuarters()));
    Subdivision subdivision;
    if (availableSubdivisions.empty()) {
        DBG ("No subdivisions :(");
    } else {
        subdivision = availableSubdivisions[rand() % availableSubdivisions.size()] / ((double)(rand() % 2) + 2);
    }
    
    vector<Note> rollNotes = roll(target - start, subdivision);
    withRoll.notes.insert(rollNotes, start, PushBehavior::wrap);
    
    return withRoll;
}

Phrase Phrase::fillInGaps() const {
    Phrase filled(*this);
    filled.notes.monophonic = false;
//    filled.notes.clear(); // TODO: do this optionally and provide notes on a separate channel?
    
    for (auto note = notes.begin(); note < notes.end(); note++) {
//        auto availableSubdivisions = subdivisions.byPosition(note->startTime);
//        Subdivision subdivision;
//        if (availableSubdivisions.empty()) {
//            DBG ("No subdivisions :(");
//        } else {
//            subdivision = availableSubdivisions[rand() % availableSubdivisions.size()] / 2.;
//        }
    
        auto nextNote = note + 1;
        Position targetNoteStartTime;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            targetNoteStartTime = nextNote->startTime + this->duration;
        } else {
            targetNoteStartTime = nextNote->startTime;
        }

        Syncopation sync(straight, 0.5);
        auto roughPlaceToStart = sync.getPlacement(note->startTime, targetNoteStartTime);
        filled = filled.withRoll(roughPlaceToStart, targetNoteStartTime);
//        auto subdivisionToStart = sync.getPlacement(note->startTime, targetNoteStartTime, subdivision);
        
        
//        double numNotesInFill = (target - start).asBeats() / subdivision.asBeats();
//    //    double numNotesInFill = (targetNoteStartTime - subdivisionToStart).asBeats() / subdivision.asBeats();
//        if (std::fmod(numNotesInFill, 1.) != 0) {
//            DBG ("something's wrong");
//        }
//
//        Sequence<Note> toInsert = filled.notes.parseMininotation(std::string(floor(numNotesInFill), 'x'), subdivision);
//        for (Note &newNote : toInsert) {
//            newNote.velocity = 20; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
//        }
    }
    
    return filled;
}
