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
#include <JuceHeader.h>
#include "Note.hpp"

using namespace std;

class Phrase // todo: rhythms really are just phrases aren't they
{
public:
    Phrase(float bars = 2, float beats = 0, juce::AudioPlayHead::TimeSignature timeSignature = juce::AudioPlayHead::TimeSignature(),
           float offset = 0, short bar = 0);
    Phrase& operator=(Phrase other);
    static Phrase defaultPhrase;
    
    float bars;
    float beats;
    juce::AudioPlayHead::TimeSignature timeSignature;
    float offset; // in quarter notes
    short bar; // use this to queue the phrase up
    
    float length();
    Note firstNote();
    double ppqUntilBarStart (double ppqPosition);
};

#endif /* Phrase_hpp */
