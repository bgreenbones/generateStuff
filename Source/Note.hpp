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

#endif /* Note_hpp */

class Note
{
public:
    Note(short pitch = 60,
         short velocity = 100,
         float startTime = 0,
         float duration = 1);
    
    short pitch; // todo: use my C pitch definitions
    short velocity; // todo: use juce types for unsigned shorts and others?
    float startTime; // in quarter notes
    float duration; // in quarter notes
    bool accent;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) const { return startTime < other.startTime; }
    bool operator> (const Note &other) const { return startTime > other.startTime; }
};
