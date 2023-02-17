//
//  Playable.hpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/16/22.
//
#pragma once

#ifndef Playable_hpp
#define Playable_hpp

#include <stdio.h>
#include "Phrase.hpp"

typedef string VoiceName;

//class Voice: TimedEvent {
class Voice {
public:
    VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation;
    bool muteRolls;
    Phrase base;
    Phrase ornamentation;
    Phrase rolls; // TODO: rolls are a form of ornamentation? and should be combined, on UI too
    vector<Phrase> phrases;

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
        phrases = { base, ornamentation, rolls };
    }
};



#endif /* Playable_hpp */
