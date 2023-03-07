/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "Voice.h"

// TODO: change the key and the channel data into some other enum or struct type or something
static const string claveKey = "clave";
static const string cascaraKey = "cascara";
static const string subdivisionsKey = "subdivisions";
static const string harmonyKey = "harmony";
static const string bassKey = "bass";
static const string melodyKey = "melody";
//static const string pulseAndDisplaceKey = "pulseAndDisplace";

static int claveChannel = 1;
static int cascaraChannel = 2;
static int subdivisionsChannel = 3;
static int harmonyChannel = 4;
static int bassChannel = 5;
static int melodyChannel = 6;
//static int pulseAndDisplaceChannel = 4;

struct VoiceAndChannel {
    string voiceName;
    int midiChannel;
};

static const vector<VoiceAndChannel> voicesAndChannels = {
    VoiceAndChannel {
        .voiceName = claveKey,
        .midiChannel = claveChannel
    },
    VoiceAndChannel {
        .voiceName = cascaraKey,
        .midiChannel = cascaraChannel
    },
    VoiceAndChannel {
        .voiceName = subdivisionsKey,
        .midiChannel = subdivisionsChannel
    },
    VoiceAndChannel {
        .voiceName = harmonyKey,
        .midiChannel = harmonyChannel
    },
    VoiceAndChannel {
        .voiceName = bassKey,
        .midiChannel = bassChannel
    },
    VoiceAndChannel {
        .voiceName = melodyKey,
        .midiChannel = melodyChannel
    }
};

class PlayQueue {
    map<VoiceName, Voice> queue;
public:
    PlayQueue() {
        for (VoiceAndChannel vc : voicesAndChannels) {
            queue.emplace(vc.voiceName, Voice(vc.voiceName, vc.midiChannel, false));
        }
    }
    
    bool hasVoice(VoiceName voiceName);
    Voice getVoice(VoiceName voiceName);
    bool hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    bool doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    void clearVoice(VoiceName voiceName);
    bool toggleMuteVoice(VoiceName voiceName);
    bool toggleMuteRolls(VoiceName voiceName);
    bool toggleMuteOrnamentation(VoiceName voiceName); // TODO: decouple this class from concept of rolls/ornamentation.
    void queuePhrase(VoiceName voiceName, Phrase phrase);
    void queueRoll(VoiceName voiceName, Phrase phrase);
    void queueOrnamentation(VoiceName voiceName, Phrase phrase);
    //    string rollsKey(string phraseKey);
    //    string ornamentsKey(string phraseKey);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


