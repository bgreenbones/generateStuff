//
//  Generator.hpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//
#pragma once

#ifndef Generator_hpp
#define Generator_hpp

#include <stdio.h>
#include <vector>
#include <functional>
#include "Sequence.hpp"
#include "Rhythm/Rhythm.hpp"
#include "Phrase.hpp"
#include "Playable.hpp"

using namespace std;

class Generator
{
public:
    vector<reference_wrapper<Sequence>> sequences;
    vector<reference_wrapper<Rhythm>> rhythms;
    vector<reference_wrapper<Phrase>> phrases;
    
    Generator() {
        sequences = initialSequences();
        Rhythm rhythm = Rhythm();
        rhythms.push_back(rhythm);
        Phrase phrase = Phrase();
        phrases.push_back(phrase);
    }
    
    bool initialized();
    bool initialize();
    
    Sequence cascaraSequence, claveSequence;
    Playable cascara();
    Playable claveFromCascara();
private:
    vector<reference_wrapper<Sequence>> initialSequences();
};

#endif /* Generator_hpp */
