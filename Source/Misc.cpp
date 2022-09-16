/*
  ==============================================================================

    Misc.cpp
    Created: 26 Aug 2022 1:10:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Misc.h"
#include <random>


beats flamishLength (float tempo) {
    // at like 60-100 bpm, flams are like a 64th note behind the target note?
    // at higher tempos, they're like a 32nd, down to like a 16th up in the 300bpm range??
    float highTempo = 300; // bpm
    float lowTempo = 50;
    float flamDistanceAtHighTempo = 1.0 / 8.0;
    float flamDistanceAtLowTempo = 1.0 / 24.0;
    float slope = (highTempo - lowTempo) / (flamDistanceAtHighTempo - flamDistanceAtLowTempo);
    float length = ((tempo - lowTempo) / slope) + flamDistanceAtLowTempo;
    
//    float notesInOrnament = (float) ornament;
//    flamDistance = flamDistance / notesInOrnament; // todo: not sold on this math
    // todo: add randomness
    // todo: account for multiple stroke flams like ruffs and drags
    return length;
}

Ornament flamish (OrnamentSimple ornamentSimple, float tempo) {
    return Ornament {
        .placement = ahead,
        .griddedness = gridFree,
        .content = even,
        .dynamics = Dynamics {
            .shape = steady,
            .range = DynamicRange {
                .high = p,
                .low = pp,
            }
        },
        .numNotes = (unsigned short) ornamentSimple,
        .length = flamishLength (tempo),
    };
}

Ornament getOrnament(OrnamentSimple ornamentSimple, float tempo) {
    Ornament result;
    switch (ornamentSimple) {
        case flam:
        case drag:
        case ruff:
            result = flamish (ornamentSimple, tempo);
            break;
        case flamA:
            break;
        case cheese:
            break;
    }
    return result;
}

vector<Note> placeOrnament(Note ornamented, Ornament ornament) {
//    Beats noteLength = ornament.length / (float) ornament.numNotes;
    beats noteLength = ornament.length;
    
    vector<Note> ornamentNotes = {};
    for (unsigned short notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
        Note ornamentNote = Note();
        ornamentNote.startTime = ornamented.startTime + ornament.placement * noteLength * (float) notesLeft;
        ornamentNote.duration = noteLength;
        ornamentNote.velocity = ornamented.velocity / 2.0; // make more dynamic
        ornamentNote.ornament = true; // todo: this, probably
        ornamentNotes.push_back(ornamentNote);
    }
    
    return ornamentNotes;
}

vector<Note> placeOrnamentSimple(Note accentNote, OrnamentSimple ornamentSimple) {
    Ornament ornament = getOrnament(ornamentSimple, HostSettings::instance().getTempo());
    return placeOrnament(accentNote, ornament);
}
//
//Sequence ornamentSequence(Sequence sequence, vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities) {
//    vector<Note> ornaments = { };
//    for (auto noteIt = sequence.notes.begin(); noteIt < sequence.notes.end(); noteIt++) {
//        if (noteIt->ornamented) {
//            OrnamentSimple ornament = possibleOrnaments[rand() % possibleOrnaments.size()]; // todo: use probabilities map
//            vector<Note> noteOrnament = placeOrnamentSimple(*noteIt, ornament, (Time) { .tempo = tempo });
//            ornaments.insert(ornaments.end(), noteOrnament.begin(), noteOrnament.end());
//        }
//    }
//    
//    for (auto noteIt = ornaments.begin(); noteIt < ornaments.end(); noteIt++) {
//        sequence.addNote(*noteIt);
//    }
//    
//    return sequence;
//}
//
//Sequence ornamentSequence(Sequence sequence, OrnamentSimple ornament, float tempo) {
//    return ornamentSequence(sequence, vector<OrnamentSimple> { ornament }, tempo);
//}
