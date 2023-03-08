/*
  ==============================================================================

    Voice.h
    Created: 4 Mar 2023 11:05:06pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <stdio.h>
#include "Phrase.hpp"
#include "Rhythm.h"
#include "Harmony.h"

typedef string VoiceName;

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

struct VoiceBindings {
    VoiceName voiceName;
    int midiChannel;
    GenerationFunction generate;
    GenerationFunction generateFromOther;
};

static const GenerationFunction placeholderGenerationFunction = [](Phrase phrase) { return phrase; };

static const vector<VoiceBindings> voiceBindings = {
    VoiceBindings {
        .voiceName = claveKey,
        .midiChannel = claveChannel,
        .generate = rhythm::claveFunction,
        .generateFromOther = rhythm::claveFromFunction
    },
    VoiceBindings {
        .voiceName = cascaraKey,
        .midiChannel = cascaraChannel,
        .generate = rhythm::cascaraFunction,
        .generateFromOther = rhythm::cascaraFromFunction
    },
    VoiceBindings {
        .voiceName = subdivisionsKey,
        .midiChannel = subdivisionsChannel,
        .generate = rhythm::fillSubdivisionsFunction,
        .generateFromOther = rhythm::fillSubdivisionsFunction
    },
    VoiceBindings {
        .voiceName = harmonyKey,
        .midiChannel = harmonyChannel,
        .generate = chordsFunction,
        .generateFromOther = chordsFromFunction
    },
    VoiceBindings {
        .voiceName = bassKey,
        .midiChannel = bassChannel,
        .generate = placeholderGenerationFunction,
        .generateFromOther = placeholderGenerationFunction
    },
    VoiceBindings {
        .voiceName = melodyKey,
        .midiChannel = melodyChannel,
        .generate = placeholderGenerationFunction,
        .generateFromOther = placeholderGenerationFunction
    }
};

class Voice {
public:
    VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation; // TODO: decouple this class from any idea of ornamentation/rolls. there should just be a base and a collection of related phrases. or just a collection.
    bool muteRolls;
    Phrase base;
    Phrase ornamentation;
    Phrase rolls; // TODO: rolls are a form of ornamentation? and should be combined, on UI too
    vector<Phrase> phrases; // TODO: maybe this  needs to be a dictionary
    
    Voice(VoiceName name, int midiChannel, bool mute):
        name(name), midiChannel(midiChannel), mute(mute) {
            initPhraseVector();
        };
    
    void initPhraseVector() {
        phrases.clear();
        phrases.push_back(base);
        phrases.push_back(ornamentation);
        phrases.push_back(rolls);
    }
};


