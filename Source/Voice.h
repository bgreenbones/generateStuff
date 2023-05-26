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
#include "Melody.h"
#include "VoiceParameters.h"
#include "Schedule.h"
#include <JuceHeader.h>

typedef string VoiceName;
using std::vector, std::unique_ptr;

// TODO: change the key and the channel data into some other enum or struct type or something
static const VoiceName claveKey = "clave";
static const VoiceName cascaraKey = "cascara";
static const VoiceName subdivisionsKey = "subdivisions";
static const VoiceName harmonyKey = "harmony";
static const VoiceName bassKey = "bass";
static const VoiceName melodyKey = "melody";
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

static const GenerationFunction placeholderGenerationFunction = [](Phrase phrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) { return phrase; };

static const vector<VoiceBindings> voiceBindings = {
    VoiceBindings {
        .voiceName = claveKey,
        .midiChannel = claveChannel,
        .generate = rhythm::claveFunction,
        .generateFromOther = rhythm::claveFromFunction,
    },
    VoiceBindings {
        .voiceName = cascaraKey,
        .midiChannel = cascaraChannel,
        .generate = rhythm::cascaraFunction,
        .generateFromOther = rhythm::cascaraFromFunction,
    },
    VoiceBindings {
        .voiceName = subdivisionsKey,
        .midiChannel = subdivisionsChannel,
        .generate = rhythm::fillSubdivisionsFunction,
        .generateFromOther = rhythm::fillSubdivisionsFunction,
    },
    VoiceBindings {
        .voiceName = harmonyKey,
        .midiChannel = harmonyChannel,
        .generate = harmony::chordsFunction,
        .generateFromOther = harmony::chordsFromFunction,
    },
    VoiceBindings {
        .voiceName = bassKey,
        .midiChannel = bassChannel,
        .generate = melody::bassFunction,
        .generateFromOther = melody::bassFromFunction,
    },
    VoiceBindings {
        .voiceName = melodyKey,
        .midiChannel = melodyChannel,
        .generate = melody::melodyFunction,
        .generateFromOther = melody::melodyFromFunction,
    }
};



class Voice {
public:
    VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation; // TODO: decouple this class from any idea of ornamentation/connecting. there should just be a base and a collection of related phrases. or just a collection.
    bool muteConnecting;
    // Phrase base;
    // vector<Phrase> phrases;
    VoiceSchedule schedule;

    // schedule of phrases to play
    // that lives in the play queue


    
    Voice(VoiceName name, int midiChannel, bool mute):
        name(name), midiChannel(midiChannel), mute(mute) {
            // initPhraseVector();
        };
    
    // void initPhraseVector() {
    //     phrases.clear();
    //     phrases.push_back(base);
    // }
};


