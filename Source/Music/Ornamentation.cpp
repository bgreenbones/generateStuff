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

beats closestGriddedLength(beats unquantized, int notes) {
    vector<beats> grids = { 1./2., 1./3., 1./4., 1./6., 1./8., 1./12., 1./16., 1./24., 1./32., 1./48., 1./64. };
    
    beats closestGrid = grids[0];
    beats closestDifference = 999.;
    for (auto grid : grids) {
        beats difference = abs(unquantized - notes * grid);
        if (difference < closestDifference) {
            closestDifference = difference;
            closestGrid = grid * notes;
        }
    }
    
    return closestGrid;
}

beats graceNoteLength (double breadth) {
    return Seconds(0.015 * breadth).asBeats(); // TODO: this hardcoded multiplier relates to values put in from the UI so maybe we should systemetize that somehow
}

Ornament graceNotes (OrnamentSimple ornamentSimple, double breadth, Griddedness griddedness) {
    unsigned short numNotes = (unsigned short) ornamentSimple;
    beats ornamentLength = graceNoteLength (breadth);
    
    if (griddedness == gridded || griddedness == loose) {
        ornamentLength = closestGriddedLength(ornamentLength, 1);
    }
    
    return Ornament {
        .placement = ahead,
        .griddedness = griddedness,
        .content = even,
        .dynamics = Dynamics {
            .shape = decresc,
            .range = DynamicRange {
                .high = pp,
                .low = ppp,
            }
        },
        .numNotes = numNotes,
        .length = ornamentLength * numNotes,
    };
}

Ornament getOrnament(OrnamentSimple ornamentSimple, double breadth, Griddedness griddedness) {
    Ornament result;
    switch (ornamentSimple) {
        case flam:
        case drag:
        case ruff:
            result = graceNotes (ornamentSimple, breadth, griddedness);
            break;
        case flamA:
            break;
        case cheese:
            break;
    }
    return result;
}

Phrase Phrase::addOrnaments(vector<OrnamentSimple> possibleOrnaments, Probability prob, double breadth, vector<float> probabilities) const{
    Phrase ornamented = (*this);
    ornamented.ornamentationNotes.clear();
    for (auto noteIt = notes.begin(); noteIt < notes.end(); noteIt++) {
        if (!noteIt->item.ornamented || !prob) {
            continue;
        }
        OrnamentSimple ornament = draw<OrnamentSimple>(possibleOrnaments, {}); // todo: use probabilities map
        vector<Timed<Note>> ornamentNotes = placeOrnament(*noteIt, ornament, breadth);
        ornamented.ornamentationNotes.insertVector(ornamentNotes);
//        for_each(ornamentNotes.begin(), ornamentNotes.end(), [&](Timed<Note> toAdd) -> void { ornamented.ornamentationNotes.add(toAdd); });
    }
    return ornamented;
}

Phrase Phrase::addOrnaments(OrnamentSimple ornament, Probability prob, double breadth) const {
    return addOrnaments(vector<OrnamentSimple> { ornament }, prob, breadth);
}


DynamicLevel rollVelocity = pp;
vector<Timed<Note>> roll(Duration length, Duration subdivision) {
    double numNotesInFill = length.asQuarters() / subdivision.asQuarters();
    if (std::fmod(numNotesInFill, 1.) != 0) {
        DBG ("something's wrong");
    }
    
    vector<Timed<Note>> roll =
        Timed<Note>::fromTimes(Mininotation::parse(std::string(floor(numNotesInFill), 'x'), subdivision));
    for (Timed<Note> &note : roll) {
        note.item.pitch += octave; // TODO: idk
        note.item.isOrnament = true;
        note.item.velocity = rollVelocity; // TODO: give a little variance above and below? crescendo/decrescendo into next note?
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
    Duration subdivision = subdivisions.drawByPosition(start % this->time.duration).item / tuplet;
    
    vector<Timed<Note>> rollNotes;
    switch (association) {
        case pickup:
            start = quantize(start, subdivision, target);
            rollNotes = roll(target - start, subdivision);
            dynamics::shape(rollNotes, 1, rollVelocity); // crescendo
            break;
        case rebound:
            start += subdivision;
            target = quantize(target, subdivision, start);
            rollNotes = roll(target - start, subdivision);
            dynamics::shape(rollNotes, rollVelocity, 1); // decrescendo
            break;
        default:
            cout << "you added more associations without thinking about this.";
            break;
    }
    
    
    if (start >= target) {
        DBG ("input to this guy was made bad.");
    }
    
    withRoll.connectingNotes.insertVector(rollNotes, start, PushBehavior::wrap);
    return withRoll;
}

Phrase Phrase::fillWithRolls(Probability rollProb,
                             Probability associationProb,
                             Probability rollLengthProb) const {
    Phrase filled(*this);
    filled.connectingNotes.clear();
    
    for (auto note = notes.begin(); note < notes.end(); note++) {
        if (!rollProb) {
            continue;
        }

        auto nextNote = note + 1;
        Position targetNoteStartTime;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            targetNoteStartTime = nextNote->startTime + this->time.duration;
        } else {
            targetNoteStartTime = nextNote->startTime;
        }
        
        Association association = flipWeightedCoin(associationProb) ? pickup : rebound;
        double syncopationAmount = boundedNormal(0, 1, 0.35, rollLengthProb);
        Syncopation sync(association == pickup ? wonk : swing, syncopationAmount);
        Position roughPlace = sync.getPlacement(note->startTime, targetNoteStartTime);
        Position start = association == pickup ? roughPlace : note->startTime;
        Position end = association == rebound ? roughPlace : targetNoteStartTime;
        filled = filled.withRoll(start, end, association);
    }
    
    return filled;
}

vector<Timed<Note>> placeOrnament(Timed<Note> const& note, OrnamentSimple ornamentSimple, double breadth) {
    Ornament ornament = getOrnament(ornamentSimple, breadth, gridded);
    auto noteLength = (ornament.length / (float) ornament.numNotes);
    
    vector<Timed<Note>> ornamentNotes;
    
    for (unsigned short notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
        Time ornamentTime;
        Note ornamentNote;
        auto offset = ornament.placement * noteLength * notesLeft;
        ornamentTime.startTime = note.startTime + offset;
        ornamentTime.duration = trunc(noteLength * 100.) / 100.; // truncate to avoid overlapping notes
        ornamentNote.isOrnament = true;
        ornamentNote.pitch += P5;
        ornamentNotes.push_back(Timed<Note>(ornamentTime, ornamentNote));
    }
    
    
    
//    ornamentNotes.events = applyDynamics(ornamentNotes.events, ornament.dynamics);
    dynamics::shape(ornamentNotes, ornament.dynamics);
    return ornamentNotes;
}
