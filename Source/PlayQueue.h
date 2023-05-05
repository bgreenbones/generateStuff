/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once


#include "Form.h"
#include "Voice.h"


class PlayQueue {
    Form form;
    map<VoiceName, Voice> queue;
public:
    Sequence<TimedEvent> scheduleTimes;
    map<TimedEvent, vector<Phrase>> schedule;
    PlayQueue() : scheduleTimes(form) {
        scheduleTimes.add(TimedEvent(form.startTime, form.duration));
        schedule[scheduleTimes.back()] = {};
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
    bool toggleMuteConnecting(VoiceName voiceName);
    bool toggleMuteConnecting(VoiceName voiceName, bool muteState);
    bool toggleMuteOrnamentation(VoiceName voiceName); // TODO: decouple this class from concept of connecting/ornamentation.
    bool toggleMuteOrnamentation(VoiceName voiceName, bool muteState); // TODO: decouple this class from concept of connecting/ornamentation.
    void queuePhrase(VoiceName voiceName, Phrase phrase);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


