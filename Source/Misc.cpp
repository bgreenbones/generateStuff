/*
  ==============================================================================

    Misc.cpp
    Created: 26 Aug 2022 1:10:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Misc.h"
#include <random>


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
    return length;
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
//
//vector<Note> placeOrnament(Note ornamented, Ornament ornament) {
////    Beats noteLength = ornament.length / (float) ornament.numNotes;
//    beats noteLength = ornament.length;
//
//    vector<Note> ornamentNotes = {};
//    for (int notesLeft = ornament.numNotes; notesLeft > 0; notesLeft--) {
//        Note ornamentNote = Note();
//        ornamentNote.startTime = ornamented.startTime + ornament.placement * noteLength * notesLeft;
//        ornamentNote.duration = noteLength;
//        ornamentNote.velocity = ornamented.velocity / 2.0; // make more dynamic
//        ornamentNote.isOrnament = true; // todo: this, probably
//        ornamentNote.pitch += 7;
//        ornamentNotes.push_back(ornamentNote);
//    }
//
//    return ornamentNotes;
//}
//
//vector<Note> placeOrnamentSimple(Note primaryNote, OrnamentSimple ornamentSimple, double breadth) {
//    double tempo = HostSettings::instance().getTempo();
//    Ornament ornament = getOrnament(ornamentSimple, tempo, breadth);
//    return placeOrnament(primaryNote, ornament);
//}


Position quantize(Position toQuantize, Duration quantizeGrid, Position relativeTo) {
//    Position smaller = toQuantize < relativeTo ? toQuantize : relativeTo;
    double temp = toQuantize.asQuarters() - relativeTo.asQuarters();
//    double mod = std::fmod(temp, quantizeGrid.asQuarters());
    double mod = (toQuantize - relativeTo) % quantizeGrid;
    int roundDirection = (mod <= (quantizeGrid.asQuarters() * 0.5)) ? -1 : 1;
    double quantized = temp + roundDirection * mod;
    return quantized + relativeTo;
}



// shaping - want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
vector<Note> applyDynamics(vector<Note> source,
                           int originVelocity,
                           int targetVelocity) {
    if (source.size() == 0) { return source; }
    vector<Note> expressive;
    
    // possible approaches:
    // shape wise:
    //  - linear
    //  - log
    //  - exp
    // time wise:
    //  - each note takes an equal step towards the end, even if some are longer than others
    //  - strict adherence to time.
    // velocity wise:
    //  - operate on each individual note's velocity - so something that is already crescendoing will end up crescendoing MORE
    //  - ignore individual notes' velocities - just start with an origin velocity and end at the target velocity.
    
    // simplest approach for now:
    //  - linear, each note steps, ignore source velocity.
    
    auto linear = [originVelocity, targetVelocity](double x) {
        int crescendoWidth = targetVelocity - originVelocity;
        double y = x * crescendoWidth + originVelocity;
        return y;
    };
    for (double i = 0.; i < source.size(); i++) {
        Note note = source[i];
        double x = (i / (double)source.size());
        note.velocity = linear(x);
        expressive.push_back(note);
    }
    
    return expressive;
}
