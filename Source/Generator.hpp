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
#include "Playable.hpp"
#include "Syncopation.h"
#include "HostSettings.h"
#include "GenerateStuffEditorState.h"

using std::shared_ptr, std::map, std::string;

static const string cascaraKey = "cascara";
static const string claveKey = "clave";
//    static const string harmonyKey = "harmony";
//    static const string bassKey = "bass";


// TODO: someday we want generator to be a place for all your phrase types which might not even exist at compile time
// TODO: actually - maybe we have another layer called "Voice" which manages a bunch of phrases intended for one instrument or sound.
// then generator becomes a sort of "Choir" or conductor which will keep manage each voice and relate them with each other.
class Generator
{
public:
    Generator(): settings(HostSettings::instance()) {}
    Generator(shared_ptr<map<string, Playable>> playQueue,
              shared_ptr<GenerateStuffEditorState> editorState):
                settings(HostSettings::instance()),
                playQueue(playQueue),
                editorState(editorState) {}

    HostSettings &settings;
    shared_ptr<map<string, Playable>> playQueue;
    shared_ptr<GenerateStuffEditorState> editorState;
    
    int cascaraChannel = 1; // TODO: make configurable from UI
    int claveChannel = 2; // and then of course move to editorState
    int chordChannel = 3;
    
    Playable cascara();
    Playable clave();
    Playable claveFromCascara();
    Playable cascaraFromClave();
    Playable flipClave(string phraseKey);
    Playable chords();
    Playable generate(string phraseKey) { // TODO: get selected phrase key from editor state instead of passing it in
        if (phraseKey == cascaraKey) { return cascara(); }
        if (phraseKey == claveKey) { return clave(); }
        return Playable(Phrase(), -1);
    }
    Playable generateFrom(string generatePhraseKey, string generateFromPhraseKey) {
        if (generatePhraseKey == cascaraKey) { // TODO: truly look inward and evaluate how we do this...
            if (generateFromPhraseKey == cascaraKey) { return cascara(); } // TODO: implement variations of a cascara
            if (generateFromPhraseKey == claveKey) { return cascaraFromClave(); }
        }
        if (generatePhraseKey == claveKey) {
            if (generateFromPhraseKey == cascaraKey) { return claveFromCascara(); }
            if (generateFromPhraseKey == claveKey) { return clave(); } // TODO: implement variations of a clave
        }
        return Playable(Phrase(), -1);
    }
    
    // TODO: play queue stuff should maybe be its own class?
    bool hasPhrase(string phraseKey);
    void removePlayable(string id);
    void toggleMutePlayable(string id);
    void queuePlayable(string id, Playable playable);

    void roll(string phraseKey, Probability rollProb, Probability associationProb, Probability rollLengthProb);
    void ornament(string phraseKey, Probability prob, double breadth, bool flams, bool drags, bool ruffs);
    
    string rollsKey(string phraseKey);
    string ornamentsKey(string phraseKey);
private:
};

#endif /* Generator_hpp */
