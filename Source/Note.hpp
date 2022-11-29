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
#include "Mininotation.h"
#include "Ornamentation.h"

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
    Note(char mininotation, Position startTime, Duration duration): Note(60, 70, startTime, duration) {
        if (Mininotation::isNote(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (mininotation == Mininotation::modifiedDefault) {
            accent();
            if (accented != 1.0) {
                DBG ("i guess accented() called the const version");
            }
        }
    }
    
    static int accentVelocity;
    
    Note operator+(const Duration duration);
    Note operator+(const Note pitch);
    
    Note& operator=(Note const other) {
        this->startTime = other.startTime;
        this->duration = other.duration;
        this->pitch = other.pitch;
        this->velocity = other.velocity;
        this->accented = other.accented;
        this->ornamented = other.ornamented;
        this->isOrnament = other.isOrnament;
        return *this;
    };
    
    Note displace(Duration toDisplaceBy, bool forwards = true);
    Note accent() const {
        Note modified = Note(*this);
        modified.accented = 1.0;
        modified.velocity = accentVelocity;
        return modified;
    };
//    Note accent() {
//        this->accented = 1.0;
//        this->velocity = accentVelocity;
//        return *this;
//    };
    Note ornament() const {
        Note modified = Note(*this);
        modified.ornamented = 1.0;
        return modified;
    }
    
    vector<Note> placeOrnament(OrnamentSimple ornamentSimple, double breadth) const;
    
    int pitch; // todo: use my C pitch definitions
    int velocity; // todo: use juce types for unsigned shorts and others?
    Probability accented;
    Probability ornamented;
    bool isOrnament;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) const { return startTime < other.startTime; }
    bool operator> (const Note &other) const { return startTime > other.startTime; }
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
};
