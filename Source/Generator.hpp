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

// #include "Harmony.h"
// #include "Rhythm.h"

using std::shared_ptr, std::map, std::string;



// Todo: destroys this class and just do all of this in the voice class
class Generator
{
private:
    unordered_map<VoiceName, VoiceBindings> voiceFunctions;
public:
    Generator(PlayQueue& playQueue,
              GenerateStuffEditorState& editorState):
                settings(HostSettings::instance()),
                playQueue(playQueue),
                editorState(editorState)
    {
        for (VoiceBindings vb : voiceBindings) { voiceFunctions.emplace(vb.voiceName, vb); }
    }

    HostSettings &settings;
    PlayQueue& playQueue;
    GenerateStuffEditorState& editorState;
    
    Phrase fromNothing(string phraseKey, GenerationFunction phraseFunction);
    Phrase from(string generatePhraseKey, string generateFromPhraseKey, GenerationFunction phraseFunction);
    Phrase flipClave(string phraseKey);

    Phrase generate(string phraseKey) { // TODO: get selected phrase key from editor state instead of passing it in?
        return fromNothing(phraseKey, voiceFunctions.at(phraseKey).generate);
    }
    Phrase generateFrom(string generatePhraseKey, string generateFromPhraseKey) {
        return from(generatePhraseKey, generateFromPhraseKey, voiceFunctions.at(generatePhraseKey).generateFromOther);
    }

    void connecting(string phraseKey, Probability connectingProb, Probability associationProb, Probability connectingLengthProb);
    void ornament(string phraseKey, Probability prob, double breadth, bool flams, bool drags, bool ruffs);
    
    string connectingKey(string phraseKey);
    string ornamentsKey(string phraseKey);
};

#endif /* Generator_hpp */
