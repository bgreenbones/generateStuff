/*
  ==============================================================================

    Ornamentation.cpp
    Created: 14 Sep 2022 12:27:34am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"

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

Phrase Phrase::fillInGaps() const {
    Phrase filled(*this);
    filled.notes.monophonic = false;
//    filled.notes.clear(); // TODO: do this optionally and provide notes on a separate channel?
    
    for (auto note = notes.begin(); note < notes.end(); note++) {
        auto availableSubdivisions = subdivisions.byPosition(note->startTime);
        Subdivision subdivision;
        if (availableSubdivisions.empty()) {
            DBG ("No subdivisions :(");
        } else {
            subdivision = availableSubdivisions[rand() % availableSubdivisions.size()] / 2.;
        }
    
        auto nextNote = note + 1;
        Position targetNoteStartTime;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            targetNoteStartTime = nextNote->startTime + this->duration;
        } else {
            targetNoteStartTime = nextNote->startTime;
        }

        // TODO: this should be its own roll-to-target-note function? we can make different kinds of fills and have fill-in-gaps select them randomly
        // options:
        // XxxxxxxxX
        // X...xxxxX = syncoptation == even
        // X.....xxX = syncopation == swing (higher value means starts later)
        // X.xxxxxxX = syncopation == wonk (higher value means starts earlier)
        // syncopation amount: 0-1 - 0 results in even, 1 results in no filler notes.
        typedef enum Syncopation {
            wonk = -1, even = 0, swing = 1,
        } Syncopation;
        Syncopation syncopation = even;
        double syncopationAmount = 0.5;
        
        Position centerBetweenNotes = (note->startTime + targetNoteStartTime) / 2.;
        Position quantizedCenter = quantize(centerBetweenNotes, subdivision, targetNoteStartTime);
        double syncopationOffset = syncopation * syncopationAmount * (targetNoteStartTime - quantizedCenter);
        auto subdivisionToStart = quantize(quantizedCenter + syncopationOffset, subdivision, targetNoteStartTime);
        double numNotesInFill = (targetNoteStartTime - subdivisionToStart).asBeats() / subdivision.asBeats();
        if (std::fmod(numNotesInFill, 1.) != 0) {
            DBG ("something's wrong");
        }
        
        Sequence<Note> toInsert = filled.notes.parseMininotation(std::string(floor(numNotesInFill), 'x'), subdivision);
        for (Note &newNote : toInsert) {
            newNote.velocity = 20; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
        }
        filled.notes.insert(toInsert, subdivisionToStart, PushBehavior::wrap);
    }
    
    return filled;
}
