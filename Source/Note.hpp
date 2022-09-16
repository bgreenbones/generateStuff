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
    Note(short pitch = 60,
         short velocity = 100,
         float startTime = 0, // in quarter notes
         float duration = 1): // in quarter notes
    TimedEvent(Position(startTime), Duration(duration)), pitch(pitch), velocity(velocity), accent(false), ornamented(false), ornament(false) { }
    
    
    short pitch; // todo: use my C pitch definitions
    short velocity; // todo: use juce types for unsigned shorts and others?
//    float startTime; // in quarter notes
//    float duration; // in quarter notes
//    bool accent; // todo: make these probabilities!!!
    Probability accent;
    Probability ornamented;
    bool ornament;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) const { return startTime < other.startTime; }
    bool operator> (const Note &other) const { return startTime > other.startTime; }
};
