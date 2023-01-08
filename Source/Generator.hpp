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
#include "Syncopation.h"

using namespace std;

// TODO: someday we want generator to be a place for all your phrase types which might not even exist at compile time
class Generator
{
public:
    Generator() {}
    Generator(shared_ptr<map<string, Playable>> playQueue): playQueue(playQueue) {}

    shared_ptr<map<string, Playable>> playQueue;
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
    
    void removePlayable(string id);
    void toggleMutePlayable(string id);
    void queuePlayable(string id, Playable playable);

    void roll(string phraseKey, Probability rollProb, Probability associationProb, Probability rollLengthProb);
    void ornament(string phraseKey, Probability prob, double breadth, bool flams, bool drags, bool ruffs);

    bool setSubdivision(const float subdivision);
    bool setPhraseLengthBars(const float bars);
    bool setPhraseLengthBeats(const float beats);
    void updateTimeSignature() {
        subdivision.updateTimeSignature();
        cascaraPhrase.updateTimeSignature();
        clavePhrase.updateTimeSignature();
    }
    
    string rollsKey(string phraseKey);
    string ornamentsKey(string phraseKey);
private:
};

#endif /* Generator_hpp */
