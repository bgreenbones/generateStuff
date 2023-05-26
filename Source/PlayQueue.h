/*
  ==============================================================================

    PlayQueue.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once


#include "Schedule.h"
#include "Voice.h"


class PlayQueue {
    map<VoiceName, Voice> queue;
    // Schedule schedule;
public:
    GenerateStuffEditorState& editorState;
    // shared_ptr<PlayQueue> qp;
    PlayQueue(GenerateStuffEditorState& editorState): editorState(editorState) {
    }

    // void initQueue(shared_ptr<PlayQueue> pq) {
    void initQueue() {
      // qp = pq;
      for (VoiceBindings vb : voiceBindings) {
          queue.emplace(vb.voiceName, Voice(*this, vb));
          // queue.emplace(vb.voiceName, Voice(pq, vb));
          // queue.emplace(vb.voiceName, Voice(vb.voiceName, vb.midiChannel, false));
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
    void queuePhrase(TimedEvent phraseTime, Phrase phrase);
    vector<Phrase> at(Quarters ppqPosition);
    Phrase at(Quarters ppqPosition, VoiceName voiceName);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


