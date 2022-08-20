//
//  Phrase.hpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#pragma once

#ifndef Phrase_hpp
#define Phrase_hpp

#include <stdio.h>
#include <vector>
#include "Note.hpp"

using namespace std;

class Phrase // todo: rhythms really are just phrases aren't they
{
public:
    Phrase(float offset = 0, short bar = 0, float length = 8);
    Phrase& operator=(Phrase other);
    static Phrase defaultPhrase;
    
    float length;
    float offset; // in quarter notes
    short bar; // use this to queue the phrase up
    
    Note firstNote();
    double ppqUntilBarStart (double ppqPosition);
};

#endif /* Phrase_hpp */
