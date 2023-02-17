/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "Playable.hpp"



// TODO: change the key and the channel data into some other enum or struct type or something
static const string cascaraKey = "cascara";
static const string claveKey = "clave";
static const string harmonyKey = "harmony";
//    static const string bassKey = "bass";

static int cascaraChannel = 1;
static int claveChannel = 2;
static int chordChannel = 3;


class PlayQueue {
    unordered_map<VoiceName, Voice> queue;
public:
    
    PlayQueue() {
        queue.emplace(cascaraKey, Voice(cascaraKey, cascaraChannel, false));
        queue.emplace(claveKey, Voice(claveKey, claveChannel, false));
        queue.emplace(harmonyKey, Voice(harmonyKey, chordChannel, false));
    }
    
    bool hasVoice(VoiceName voiceName);
    Voice getVoice(VoiceName voiceName);
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
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


