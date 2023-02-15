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

//class Playable {
//public:
//    Playable(Phrase phrase): phrase(phrase), mute(false) { };
//    Phrase phrase;
//    bool mute;
//};

template class Sequence<Phrase>;
typedef string VoiceName;


struct PhraseAssociation {
    Phrase base;
    Phrase ornaments;
    Phrase rolls;
};

class Voice: TimedEvent {
public:
    VoiceName name;
    int midiChannel;
    bool mute;
    Sequence<Phrase> phrases;
    bool muteOrnamentation;
    Sequence<Phrase> ornamentation;
    bool muteRolls;
    Sequence<Phrase> rolls; // TODO: rolls are a form of ornamentation? and should be combined, on UI too

    Voice(VoiceName name, int midiChannel, bool mute):
        TimedEvent(0, Bars(512)),
        name(name), midiChannel(midiChannel), mute(mute),
        phrases(*this), ornamentation(*this), rolls(*this) { };
    
    PhraseAssociation getAssociatedPhrases(Position startTime, Duration phraseLength);
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
};

#endif /* Playable_hpp */
