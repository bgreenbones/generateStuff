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
#include "Sequence.h"
#include "Pitch.h"
#include "Probability.h"
#include "Mininotation.h"
#include "Ornamentation.h"
#include "Dynamics.h"

static const DynamicLevel accentVelocity = DynamicLevel::fff;
static const DynamicLevel unaccentedVelocity = DynamicLevel::mp;
static const DynamicLevel defaultVelocity = DynamicLevel::mf;

static const Pitch defaultPitch(C, 5);

class Note
{
public:
    Note(Pitch pitch = defaultPitch,
         int velocity = defaultVelocity
         ): pitch(pitch), velocity(velocity), accented(false), ornamented(false), isOrnament(false) { }
    
    static DynamicLevel accentVelocity;
    operator int() const { return pitch.pitchValue; };
    
    Note operator+(const Duration duration);
    Note operator+(const Note pitch);
    
    Note& operator=(Note const& other) {
        this->pitch = other.pitch;
        this->velocity = other.velocity;
        this->accented = other.accented;
        this->ornamented = other.ornamented;
        this->isOrnament = other.isOrnament;
        return *this;
    };
    bool operator==(Note const& other) {
        return this->pitch == other.pitch &&
        this->velocity == other.velocity &&
        this->accented == other.accented &&
        this->ornamented == other.ornamented &&
        this->isOrnament == other.isOrnament;
    };
    
    Note displace(Duration toDisplaceBy, bool forwards = true);
    Note accent() {
        this->accented = 1.0;
        this->velocity = accentVelocity;
        return *this;
    };

    Note withAccent() const {
        Note modified = Note(*this);
        return modified.accent();
    };

    Note ornament() const {
        Note modified = Note(*this);
        modified.ornamented = 1.0;
        return modified;
    }
    
    
    Pitch pitch;
    int velocity;
    Probability accented;
    Probability ornamented;
    bool isOrnament;
    // todo: other expressions for ccs
    
    // bool operator< (const Note &other) { return startTime < other.startTime; }
    // bool operator> (const Note &other) { return startTime > other.startTime; }
    
};






#endif /* Note_hpp */

