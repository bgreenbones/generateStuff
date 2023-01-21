/*
  ==============================================================================

    Dynamics.cpp
    Created: 29 Nov 2022 9:41:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Dynamics.h"
#include "Note.hpp"

int velocityFromDynamicLevel(DynamicLevel level) {
    double minVelocity = 1;
    double maxVelocity = 127;
    double velocityRange = maxVelocity - minVelocity;
    double numberOfLevels = ffff + 1;
    
    return ((level + 1.) / numberOfLevels) * velocityRange + minVelocity;
}

vector<Note> applyDynamics(vector<Note> source,
                           Dynamics dynamics) {
    
    int highVelocity = velocityFromDynamicLevel(dynamics.range.high);
    int medianVelocity = velocityFromDynamicLevel(dynamics.range.median);
    int lowVelocity = velocityFromDynamicLevel(dynamics.range.low);
    
    switch (dynamics.shape) {
        case cresc:
            return applyDynamics(source, lowVelocity, highVelocity);
        case steady:
            return applyDynamics(source, medianVelocity, medianVelocity);
        case decresc:
            return applyDynamics(source, highVelocity, lowVelocity);
        default:
            return source;
    }
    
    return source;
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
