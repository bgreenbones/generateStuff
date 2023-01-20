/*
  ==============================================================================

    Ornamentation.cpp
    Created: 14 Sep 2022 12:27:34am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"
#include "Random.h"
#include "Dynamics.h"

beats flamishLength (double tempo, double breadth) {
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
    return breadth * length;
}

Ornament flamish (OrnamentSimple ornamentSimple, double tempo, double breadth) {
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
        .length = flamishLength (tempo, breadth),
    };
}

Ornament getOrnament(OrnamentSimple ornamentSimple, double tempo, double breadth) {
    Ornament result;
    switch (ornamentSimple) {
        case flam:
        case drag:
        case ruff:
            result = flamish (ornamentSimple, tempo, breadth);
            break;
        case flamA:
            break;
        case cheese:
            break;
    }
    return result;
}



//        if (flipCoin()) {
//            vector<Note> ornamentNotes = placeOrnamentSimple(*note, draw<OrnamentSimple>({ flam, drag, ruff }));
//            for(auto ornamentNote : ornamentNotes) {
//                if(!filled.addNote(ornamentNote)) {
//                    std::cout << "gotta handle notes at the same time in monophonic sequences";
//                }
//            }
//        } else {
Phrase Phrase::addOrnaments(vector<OrnamentSimple> possibleOrnaments, Probability prob, double breadth, vector<float> probabilities) const{
    bool isAlreadyOrnamented = false; // todo: know this somehow
    if (isAlreadyOrnamented) {
        throw exception();
    }
    Phrase ornamented = (*this);
    ornamented.notes.clear();
    for (auto noteIt = notes.begin(); noteIt < notes.end(); noteIt++) {
        if (!noteIt->ornamented || !prob) {
            continue;
        }
        OrnamentSimple ornament = draw<OrnamentSimple>(possibleOrnaments); // todo: use probabilities map
        Sequence<Note> ornamentNotes = noteIt->placeOrnament(ornament, breadth);
//        Sequence<Note> ornamentNotes = noteIt->placeOrnament(ornament, breadth);
        for_each(ornamentNotes.begin(), ornamentNotes.end(), [&](Note toAdd) -> void { ornamented.addNote(toAdd); });
    }
    return ornamented;
}

Phrase Phrase::addOrnaments(OrnamentSimple ornament, Probability prob, double breadth) const {
    return addOrnaments(vector<OrnamentSimple> { ornament }, prob, breadth);
}


int rollVelocity = 40;
vector<Note> roll(Duration length, Subdivision subdivision) {
    double numNotesInFill = length.asQuarters() / subdivision.asQuarters();
    if (std::fmod(numNotesInFill, 1.) != 0) {
        DBG ("something's wrong");
    }
    
    vector<Note> roll = Mininotation::parse<Note>(std::string(floor(numNotesInFill), 'x'), subdivision);
    for (Note &note : roll) {
        note.pitch += 12; // TODO: idk
        note.isOrnament = true;
        note.velocity = rollVelocity; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
    }
    return roll;
}


// TODO: pick up and rebound can refer to other structures too...not just rolls....
Phrase Phrase::withRoll(Position start, Position target, Association association) const {
    if (start >= target) {
        DBG ("bad input to this guy");
    }
    
    Phrase withRoll(*this);
    double tuplet = (double) uniformInt(2, 4);
    Duration subdivision = subdivisions.drawByPosition(start % this->duration) / tuplet;
    
    vector<Note> rollNotes;
    switch (association) {
        case pickup:
            start = quantize(start, subdivision, target);
            rollNotes = roll(target - start, subdivision);
            rollNotes = applyDynamics(rollNotes, 1, rollVelocity); // crescendo
            break;
        case rebound:
            start += subdivision;
            target = quantize(target, subdivision, start);
            rollNotes = roll(target - start, subdivision);
            rollNotes = applyDynamics(rollNotes, rollVelocity, 1); // decrescendo
            break;
        default:
            cout << "you added more associations without thinking about this.";
            break;
    }
    
    
    if (start >= target) {
        DBG ("input to this guy was made bad.");
    }
    
    withRoll.notes.insert(rollNotes, start, PushBehavior::wrap);
    return withRoll;
}

Phrase Phrase::fillWithRolls(Probability rollProb,
                             Probability associationProb,
                             Probability rollLengthProb) const {
    Phrase filled(*this);
    filled.notes.monophonic = false;
    filled.notes.clear();
    
    for (auto note = notes.begin(); note < notes.end(); note++) {
        if (!rollProb) {
            break;
        }

        auto nextNote = note + 1;
        Position targetNoteStartTime;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            targetNoteStartTime = nextNote->startTime + this->duration;
        } else {
            targetNoteStartTime = nextNote->startTime;
        }
        
        //        Association association = associationValue >= 0.5 ? pickup : rebound;
        //        double syncopationAmount = abs(associationValue - 0.5) * 2;
        //        Association association = draw<Association>({ pickup, rebound });
        //        Syncopation sync(association == pickup ? wonk : swing, boundedNormal(0, 1, 0.5)); // going for long rolls, for now.
        Association association = flipWeightedCoin(associationProb) ? pickup : rebound;
        double syncopationAmount = boundedNormal(0, 1, 0.35, rollLengthProb);
        Syncopation sync(association == pickup ? wonk : swing, syncopationAmount);
        Position roughPlace = sync.getPlacement(note->startTime, targetNoteStartTime);
//            Position roughPlace = boundedNormal(note->startTime, targetNoteStartTime, 0.5);
        Position start = association == pickup ? roughPlace : note->startTime;
        Position end = association == rebound ? roughPlace : targetNoteStartTime;
        filled = filled.withRoll(start, end, association);
    }
    
    return filled;
}
