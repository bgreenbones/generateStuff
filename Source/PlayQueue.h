/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "Playable.hpp"


class PlayQueue {
    unordered_map<VoiceName, Voice> queue;
public:
    
    bool hasVoice(VoiceName voiceName);
    bool hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    bool doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    void clearVoice(VoiceName voiceName);
    bool toggleMuteVoice(VoiceName voiceName);
    bool toggleMuteRolls(VoiceName voiceName);
    bool toggleMuteOrnamentation(VoiceName voiceName);
    void queuePhrase(VoiceName voiceName, Phrase phrase);
    void queueRoll(VoiceName voiceName, Phrase phrase);
    void queueOrnamentation(VoiceName voiceName, Phrase phrase);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    PhraseAssociation getAssociatedPhrases(VoiceName voiceName, Position startTime, Duration phraseLength);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


