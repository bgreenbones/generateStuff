//
//  Rhythm.hpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/4/22.
//

#pragma once

#ifndef Rhythm_hpp
#define Rhythm_hpp

#include <stdio.h>
#include <vector>
#include <random>
#include "../Note.hpp"
#include "../Sequence.hpp"

using namespace std;

class Rhythm
{
public:
    Rhythm(float subdivision = 1.0 / 2.0); // todo: args structs
    Rhythm(Rhythm const&);
    Rhythm& operator=(Rhythm other);
    static Rhythm defaultRhythm;
    
    random_device rd; // todo: pull this stuff out into a singleton class that everyone uses?
    mt19937 gen;

    float subdivision;

    // todo:
    // approach ideas:
    // 1. maybe these generate methods can all just be static.
    // just take a Rhythm as input, Rhythm can take all arguments to any function as members
    // 2. do a chainable interface -  but return a new rhythm each time.
    
    Sequence pulseAndDisplace(
        float pulse = 0.5, // in quarter notes
        float displacement = 0.25, // in quarter notes, later.
        float pDisplace = 0.5,
        float pDouble = 0.75); // (for forward displacement, set pDouble = 1 and displacement = 1 - amount to displace forward)
    Sequence fillInGaps(Sequence rhythmWithGaps);
    Sequence chooseAccents(Sequence monotoneRhythm);
    
    // Latin.cpp
    Sequence randomCascara(float pDisplace = 0.5, float pDouble = 0.75);
    Sequence randomClave();
    Sequence cascaraFromClave(Sequence clave);
    Sequence claveFromCascara(Sequence cascara);
    
    
};

#endif /* Rhythm_hpp */
