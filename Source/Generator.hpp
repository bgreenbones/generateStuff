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
#include "HostSettings.h"

using std::shared_ptr, std::map, std::string;

// TODO: someday we want generator to be a place for all your phrase types which might not even exist at compile time
class Generator
{
public:
    Generator(): settings(HostSettings::instance()) {}
    Generator(shared_ptr<map<string, Playable>> playQueue): settings(HostSettings::instance()), playQueue(playQueue) {}
    Generator& operator=(Generator const& other) {
        if (this == &other) {
            return *this;
        }
        playQueue = other.playQueue;
        return *this;
    }

    HostSettings &settings;
    shared_ptr<map<string, Playable>> playQueue;

    constexpr static float const defaultSubdivision = 1./2.;
    constexpr static float const defaultBars = 2;
    constexpr static float const defaultBeats = 0;
    Bars phraseLengthBars = Bars(defaultBars, true);
    Beats phraseLengthBeats = Beats(defaultBeats, true);
    Position phraseStartTime = 0;
    Subdivision subdivision = Subdivision(Beats(defaultSubdivision), phraseStartTime, phraseLength());
    Duration phraseLength() {
//        Duration result = Bars(phraseLengthBars) + Beats(phraseLengthBeats);
        Duration result = phraseLengthBars + phraseLengthBeats;
        return result;
    }
    
    int cascaraChannel = 1;
    int claveChannel = 2;
    
    Playable cascara();
    Playable clave();
    Playable claveFromCascara();
    Playable cascaraFromClave();
    
    bool hasPhrase(string phraseKey);
    
    void removePlayable(string id);
    void toggleMutePlayable(string id);
    void queuePlayable(string id, Playable playable);

    void roll(string phraseKey, Probability rollProb, Probability associationProb, Probability rollLengthProb);
    void ornament(string phraseKey, Probability prob, double breadth, bool flams, bool drags, bool ruffs);

    bool setSubdivision(const float subdivision);
    bool setPhraseLength(const float bars, const float beats);
//    void updateTimeSignature() {
//        subdivision.updateTimeSignature();
//        cascaraPhrase.updateTimeSignature();
//        clavePhrase.updateTimeSignature();
//    }
    
    string rollsKey(string phraseKey);
    string ornamentsKey(string phraseKey);
private:
};

#endif /* Generator_hpp */
