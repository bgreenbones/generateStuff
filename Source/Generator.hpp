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
#include "Phrase.hpp"
#include "Playable.hpp"

using namespace std;

class Generator
{
public:
    Generator() {}

    Phrase cascaraPhrase, clavePhrase;
    constexpr static float const defaultSubdivision = 1./2.;
    constexpr static float const defaultBars = 2;
    constexpr static float const defaultBeats = 0;
    Bars phraseLengthBars = defaultBars;
    Beats phraseLengthBeats = defaultBeats;
    Position phraseStartTime = 0;
    Subdivision subdivision = Subdivision(Beats(defaultSubdivision), phraseStartTime, phraseLength());
    Duration phraseLength() { return Bars(phraseLengthBars) + Beats(phraseLengthBeats); }
    
    int cascaraChannel = 1;
    int claveChannel = 2;
    
    Playable cascara();
    Playable clave();
    Playable claveFromCascara();
    Playable cascaraFromClave();
    Playable rollCascara();
    Playable rollClave();
    Playable ornamentCascara(bool flams, bool drags, bool ruffs);
    Playable ornamentClave(bool flams, bool drags, bool ruffs);

    bool setSubdivision(const float subdivision);
    bool setPhraseLengthBars(const float bars);
    bool setPhraseLengthBeats(const float beats);
    void updateTimeSignature() {
        subdivision.updateTimeSignature();
        cascaraPhrase.updateTimeSignature();
        clavePhrase.updateTimeSignature();
    }
private:
};

#endif /* Generator_hpp */
