//
//  Generator.hpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//
#pragma once

#ifndef Generator_hpp
#define Generator_hpp

#include <stdio.h>
#include <vector>
#include <functional>
#include <JuceHeader.h>
#include "Phrase.hpp"
#include "Voice.h"
#include "Syncopation.h"
#include "HostSettings.h"
#include "GenerateStuffEditorState.h"
#include "PlayQueue.h"

#include "Harmony.h"
#include "Rhythm.h"

using std::shared_ptr, std::map, std::string;

// TODO: someday we want generator to be a place for all your phrase types which might not even exist at compile time
// TODO: actually - maybe we have another layer called "Voice" which manages a bunch of phrases intended for one instrument or sound.
// then generator becomes a sort of "Choir" or conductor which will keep manage each voice and relate them with each other.
class Generator
{
private:
    unordered_map<VoiceName, VoiceBindings> voiceFunctions;
public:
    Generator(shared_ptr<PlayQueue> playQueue,
              shared_ptr<GenerateStuffEditorState> editorState):
                settings(HostSettings::instance()),
                playQueue(playQueue),
                editorState(editorState)
    {
        for (VoiceBindings vb : voiceBindings) { voiceFunctions.emplace(vb.voiceName, vb); }
    }

    HostSettings &settings;
    shared_ptr<PlayQueue> playQueue;
    shared_ptr<GenerateStuffEditorState> editorState;
    
    Phrase fromNothing(string phraseKey, GenerationFunction phraseFunction);
    Phrase from(string generatePhraseKey, string generateFromPhraseKey, GenerationFunction phraseFunction);
    Phrase flipClave(string phraseKey);

    Phrase generate(string phraseKey) { // TODO: get selected phrase key from editor state instead of passing it in?
        return fromNothing(phraseKey, voiceFunctions.at(phraseKey).generate);
    }
    Phrase generateFrom(string generatePhraseKey, string generateFromPhraseKey) {
        return from(generatePhraseKey, generateFromPhraseKey, voiceFunctions.at(generatePhraseKey).generateFromOther);
    }

    void roll(string phraseKey, Probability rollProb, Probability associationProb, Probability rollLengthProb);
    void ornament(string phraseKey, Probability prob, double breadth, bool flams, bool drags, bool ruffs);
    
    string rollsKey(string phraseKey);
    string ornamentsKey(string phraseKey);
};

#endif /* Generator_hpp */
