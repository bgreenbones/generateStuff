/*
  ==============================================================================

    Ensemble.h
    Created: 15 Feb 2023 12:29:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once

#include "Schedule.h"
#include "Voice.h"
#include "Clave.h"
#include "Cascara.h"
#include "Shaker.h"
#include "Chords.h"
#include "Bass.h"
#include "Lead.h"

class Ensemble {
    // Schedule schedule;
public:
    map<VoiceName, Voice&> queue;
    GenerateStuffEditorState& editorState;

    Clave clave;
    Cascara cascara;
    Shaker shaker;
    Chords chords;
    Bass bass;
    Lead lead;

    Ensemble(GenerateStuffEditorState& editorState): editorState(editorState),
      clave(Clave(*this)),
      cascara(Cascara(*this)),
      shaker(Shaker(*this)),
      chords (Chords(*this)),
      bass (Bass(*this)),
      lead (Lead(*this))
    {
      queue.emplace(clave.name, clave);
      queue.emplace(cascara.name, cascara);
      queue.emplace(shaker.name, shaker);
      queue.emplace(chords.name, chords);
      queue.emplace(bass.name, bass);
      queue.emplace(lead.name, lead);
    }

    void writeSong();
    Phrase emptyPhrase(VoiceName name) {
      Phrase phrase = Phrase(editorState.getSubdivision(),
                              editorState.getStartTime(),
                              editorState.getPhraseLength());
      phrase.voice = name;
      return phrase;
    }

    bool hasVoice(VoiceName voiceName);
    Voice& getVoice(VoiceName voiceName);
    bool hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    bool doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength);
    void clearVoice(VoiceName voiceName);
    bool toggleMuteVoice(VoiceName voiceName);
    bool toggleMuteConnecting(VoiceName voiceName);
    bool toggleMuteConnecting(VoiceName voiceName, bool muteState);
    bool toggleMuteOrnamentation(VoiceName voiceName); // TODO: decouple this class from concept of connecting/ornamentation.
    bool toggleMuteOrnamentation(VoiceName voiceName, bool muteState); // TODO: decouple this class from concept of connecting/ornamentation.
    void queuePhrase(Timed phraseTime, Phrase phrase);
    vector<Phrase> at(Quarters ppqPosition);
    Phrase at(Quarters ppqPosition, VoiceName voiceName);
    
    void setMidiChannel(VoiceName voiceName, int newMidiChannel);
    int getMidiChannel(VoiceName voiceName);
    
    auto begin() const { return queue.begin(); }
    auto begin() { return queue.begin(); }
    auto end() const { return queue.end(); }
    auto end() { return queue.end(); }
};


