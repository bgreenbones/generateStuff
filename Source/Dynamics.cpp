/*
  ==============================================================================

    Dynamics.cpp
    Created: 29 Nov 2022 9:41:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Dynamics.h"
#include "Note.hpp"

//int velocityFromDynamicLevel(DynamicLevel level) {
//    double minVelocity = 1;
//    double maxVelocity = 127;
//    double velocityRange = maxVelocity - minVelocity;
//    double numberOfLevels = ffff + 1;
//
//    return ((level + 1.) / numberOfLevels) * velocityRange + minVelocity;
//}

vector<Note>& dynamics::shape(vector<Note>& source, Dynamics dynamics) {
    int highVelocity = dynamics.range.high;
    int medianVelocity = dynamics.range.median;
    int lowVelocity = dynamics.range.low;
    
    switch (dynamics.shape) {
        case cresc:
            return dynamics::shape(source, lowVelocity, highVelocity);
        case steady:
            return dynamics::shape(source, medianVelocity, medianVelocity);
        case decresc:
            return dynamics::shape(source, highVelocity, lowVelocity);
        default:
            return source;
    }
    
    return source;
}




// shaping - want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
vector<Note>& dynamics::shape(vector<Note>& source, int originVelocity, int targetVelocity)
{
    if (source.size() == 0) { return source; }
//    vector<Note> expressive;
    
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
//        Note note = source[i];
        double x = (i / (double)source.size());
        source[i].velocity = linear(x);
//        expressive.push_back(note);
    }
    
//    return expressive;
    return source;
}

vector<Note>& dynamics::stretch(vector<Note>& source, DynamicRange targetRange)
{
    int currentMinVelocity = 127;
    int currentMaxVelocity = 1;
    
    for (Note note : source) {
        currentMaxVelocity = max(note.velocity, currentMaxVelocity);
        currentMinVelocity = min(note.velocity, currentMinVelocity);
    }
    
    double currentRangeWidth = currentMaxVelocity - currentMinVelocity;
    
    double targetRangeWidth = targetRange.high - targetRange.low;
    double scaler = targetRangeWidth / currentRangeWidth;
    double transposer = targetRange.low < targetRange.high // if you want I think you could flip dynamics by giving targetRange.high < targetRange.low
                        ? targetRange.low
                        : 2 * targetRange.high + targetRange.low;
    for (Note &note : source) {
        note.velocity = (note.velocity - currentMinVelocity) * scaler + transposer;
    }
    
    return source;
}

vector<Note>& dynamics::stretch(vector<Note>& source, DynamicLevel targetMinimum, DynamicLevel targetMaximum)
{
    return dynamics::stretch(source, DynamicRange { .low = targetMinimum, .high = targetMaximum });
}


