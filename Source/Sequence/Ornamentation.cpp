/*
  ==============================================================================

    Ornamentation.cpp
    Created: 14 Sep 2022 12:27:34am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Sequence.hpp"

Sequence Sequence::addOrnaments(vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities) const{
    bool isAlreadyOrnamented = false; // todo: know this somehow
    if (isAlreadyOrnamented) {
        throw exception();
    }
    Sequence ornamented = (*this);
    for (auto noteIt = notes.begin(); noteIt < notes.end(); noteIt++) {
        if (noteIt->ornamented) {
            OrnamentSimple ornament = possibleOrnaments[rand() % possibleOrnaments.size()]; // todo: use probabilities map
            vector<Note> ornamentNotes = placeOrnamentSimple(*noteIt, ornament);
            for_each(ornamentNotes.begin(), ornamentNotes.end(), [&](Note toAdd) -> void { ornamented.addNote(toAdd); });
        }
    }
    return ornamented;
}

Sequence Sequence::addOrnaments(OrnamentSimple ornament, float tempo) const {
    return addOrnaments(vector<OrnamentSimple> { ornament }, tempo);
}
