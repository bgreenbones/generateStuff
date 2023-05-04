/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "Voice.h"

class PlayQueue {
    map<VoiceName, Voice> queue;
public:
    PlayQueue() {
        for (VoiceBindings vb : voiceBindings) {
            queue.emplace(vb.voiceName, Voice(vb.voiceName, vb.midiChannel, false));
        }
    }
    
    bool hasVoice(VoiceName voiceName);
    Voice getVoice(VoiceName voiceName);
    bool hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    bool doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    void clearVoice(VoiceName voiceName);
    bool toggleMuteVoice(VoiceName voiceName);
    bool toggleMuteRolls(VoiceName voiceName);
    bool toggleMuteRolls(VoiceName voiceName, bool muteState);
    bool toggleMuteOrnamentation(VoiceName voiceName); // TODO: decouple this class from concept of rolls/ornamentation.
    bool toggleMuteOrnamentation(VoiceName voiceName, bool muteState); // TODO: decouple this class from concept of rolls/ornamentation.
    void queuePhrase(VoiceName voiceName, Phrase phrase);
    void queueRoll(VoiceName voiceName, Phrase phrase);
    // void queueOrnamentation(VoiceName voiceName, Phrase phrase);
    //    string rollsKey(string phraseKey);
    //    string ornamentsKey(string phraseKey);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


