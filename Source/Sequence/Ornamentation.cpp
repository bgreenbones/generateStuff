/*
  ==============================================================================

    Ornamentation.cpp
    Created: 14 Sep 2022 12:27:34am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"
#include "Syncopation.h"
#include "Random.h"




Phrase Phrase::addOrnaments(vector<OrnamentSimple> possibleOrnaments, vector<float> probabilities) const{
    bool isAlreadyOrnamented = false; // todo: know this somehow
    if (isAlreadyOrnamented) {
        throw exception();
    }
    Phrase ornamented = (*this);
    for (auto noteIt = notes.begin(); noteIt < notes.end(); noteIt++) {
        if (noteIt->ornamented) {
//            OrnamentSimple ornament = possibleOrnaments[rollDie(possibleOrnaments.size()) - 1]; // todo: use probabilities map
            OrnamentSimple ornament = draw<OrnamentSimple>(possibleOrnaments); // todo: use probabilities map
            vector<Note> ornamentNotes = placeOrnamentSimple(*noteIt, ornament);
            for_each(ornamentNotes.begin(), ornamentNotes.end(), [&](Note toAdd) -> void { ornamented.addNote(toAdd); });
        }
    }
    return ornamented;
}

Phrase Phrase::addOrnaments(OrnamentSimple ornament) const {
    return addOrnaments(vector<OrnamentSimple> { ornament });
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
        auto tuplet = (double) uniformInt(2, 4);
        subdivision = draw<Subdivision>(availableSubdivisions) / tuplet;
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
        if (flipCoin()) {
            vector<Note> ornamentNotes = placeOrnamentSimple(*note, draw<OrnamentSimple>({ flam, drag, ruff }));
            for(auto ornamentNote : ornamentNotes) {
                if(!filled.addNote(ornamentNote)) {
                    std::cout << "gotta handle notes at the same time in monophonic sequences";
                } }
        } else {
            auto nextNote = note + 1;
            Position targetNoteStartTime;
            if (nextNote == this->notes.end()) {
                nextNote = this->notes.begin();
                targetNoteStartTime = nextNote->startTime + this->duration;
            } else {
                targetNoteStartTime = nextNote->startTime;
            }
            
            Syncopation sync(draw<SyncopationType>({ straight, swing }),
                             boundedNormal(0, 1));
            auto roughPlaceToStart = sync.getPlacement(note->startTime, targetNoteStartTime);
            filled = filled.withRoll(roughPlaceToStart, targetNoteStartTime);
        }
        
    }
    
    return filled;
}
