/*
  ==============================================================================

    Voice.h
    Created: 4 Mar 2023 11:05:06pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <stdio.h>
#include "Phrase.hpp"

typedef string VoiceName;

class Voice {
public:
    VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation; // TODO: decouple this class from any idea of ornamentation/rolls. there should just be a base and a collection of related phrases. or just a collection.
    bool muteRolls;
    Phrase base;
    Phrase ornamentation;
    Phrase rolls; // TODO: rolls are a form of ornamentation? and should be combined, on UI too
    vector<Phrase> phrases; // TODO: maybe this  needs to be a dictionary
    
    Voice(VoiceName name, int midiChannel, bool mute):
//        TimedEvent(0, Bars(512)),
        name(name), midiChannel(midiChannel), mute(mute) {
            initPhraseVector();
        };
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
    
    void initPhraseVector() {
//        phrases = { base, ornamentation, rolls };
        phrases.clear();
        phrases.push_back(base);
        phrases.push_back(ornamentation);
        phrases.push_back(rolls);
    }
};


