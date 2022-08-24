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
#include <JuceHeader.h>
#include "Sequence.hpp"
#include "Rhythm/Rhythm.hpp"
#include "Phrase.hpp"
#include "Playable.hpp"

using namespace std;

class Generator
{
public:
//    vector<reference_wrapper<Sequence>> sequences;
//    vector<reference_wrapper<Rhythm>> rhythms;
//    vector<reference_wrapper<Phrase>> phrases;
    vector<Sequence> sequences;
    vector<Rhythm> rhythms;
    vector<Phrase> phrases;
    
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
    
    bool setPhraseLengthBars(const float bars);
    bool setPhraseLengthBeats(const float beats);
    bool setTimeSignature(const int numerator, const int denominator);
    juce::AudioPlayHead::TimeSignature getTimeSignature();
private:
//    vector<reference_wrapper<Sequence>> initialSequences();
    vector<Sequence> initialSequences();
};

#endif /* Generator_hpp */
