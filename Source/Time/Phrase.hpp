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
#include "../Note.hpp"
#include "TimeTypedefs.h"
#include "TimeSignature.h"
#include "Duration.h"

using namespace std;


class Phrase // todo: rhythms really are just phrases aren't they
{
public:
    Phrase();
    Phrase(float bars, float beats, TimeSignature timeSignature, float offset, short bar);
    Phrase& operator=(Phrase other);
//    static Phrase defaultPhrase;
    
    float bars;
    float beats;
    Duration duration;
    TimeSignature timeSignature;
    float offset; // in quarter notes
    short bar; // use this to queue the phrase up
    
    float length();
    Note firstNote();
    double ppqUntilBarStart (double ppqPosition);
};

#endif /* Phrase_hpp */
