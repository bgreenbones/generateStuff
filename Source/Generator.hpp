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
#include "Playable.hpp"

using namespace std;

class Generator
{
public:
    Sequence cascaraSequence, claveSequence;
    constexpr static float const defaultSubdivision = 1./2.;
    constexpr static float const defaultBars = 2;
    constexpr static float const defaultBeats = 0;
    Bars phraseLengthBars = defaultBars;
    Beats phraseLengthBeats = defaultBeats;
    Position phraseStartTime = 0;
    Subdivision subdivision = Subdivision(Beats(defaultSubdivision), phraseStartTime, phraseLength());
    
    Generator() {}
    
    Duration phraseLength() { return Bars(phraseLengthBars) + Beats(phraseLengthBeats); }
    Playable cascara();
    Playable claveFromCascara();
    bool setSubdivision(const float subdivision);
    bool setPhraseLengthBars(const float bars);
    bool setPhraseLengthBeats(const float beats);
private:
};

#endif /* Generator_hpp */
