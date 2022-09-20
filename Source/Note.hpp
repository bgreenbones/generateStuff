//
//  Note.hpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#pragma once

#ifndef Note_hpp
#define Note_hpp

#include <stdio.h>
#include "Time/TimedEvent.h"
#include "Probability.h"

#endif /* Note_hpp */

class Note: public TimedEvent
{
public:
    Note(int pitch,
         int velocity,
         Position startTime,
         Duration duration):
    TimedEvent(Position(startTime), Duration(duration)), pitch(pitch), velocity(velocity), accented(false), ornamented(false), isOrnament(false) { }
    
    Note(int pitch = 60,
         int velocity = 70,
         double startTime = 0, // in quarter notes
         double duration = 1): // in quarter notes
    Note(pitch, velocity, Position(startTime), Duration(duration)) { }
    Note(Position startTime, Duration duration): Note(60, 70, startTime, duration) { }
    
    static int accentVelocity;
    
    Note operator+(const Duration duration);
    Note operator+(const Note pitch);
    
    Note displace(Duration toDisplaceBy, bool forwards = true);
    Note accent() {
        Note modified = Note(*this);
        modified.accented = 1.0;
        modified.velocity = accentVelocity;
        return modified;
    };
    Note ornament() {
        Note modified = Note(*this);
        modified.ornamented = 1.0;
        return modified;
    }
    
    int pitch; // todo: use my C pitch definitions
    int velocity; // todo: use juce types for unsigned shorts and others?
    Probability accented;
    Probability ornamented;
    bool isOrnament;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) const { return startTime < other.startTime; }
    bool operator> (const Note &other) const { return startTime > other.startTime; }
};
