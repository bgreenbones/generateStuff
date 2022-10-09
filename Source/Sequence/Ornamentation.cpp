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
        Duration timeBetweenNotes;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            timeBetweenNotes = nextNote->startTime + this->duration - note->startTime;
        } else {
            timeBetweenNotes = nextNote->startTime - note->startTime;
        }
        double subdivisionsBetweenNotes = timeBetweenNotes.asBeats() / subdivision.asBeats();
        //        auto subdivisionToStart = std::fmod( // TODO: optionally choose at random?
        Sequence<Note> toInsert = filled.notes.parseMininotation(std::string(floor(subdivisionsBetweenNotes) - 1, 'x'), subdivision);
        for (Note &newNote : toInsert) {
            newNote.velocity = 20; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
        }
        filled.notes.insert(toInsert, note->startTime + subdivision, PushBehavior::wrap);
    }
    
    return filled;
}
