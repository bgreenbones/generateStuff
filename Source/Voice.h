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

//class PlayQueue;
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
    GenerationFunction2 variation;
};

static const GenerationFunction placeholderGenerationFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
  return phrase;
};
static const GenerationFunction2 placeholderGenerationFunction2 = [](Voice const& v) {
  return Phrase();
};

static const vector<VoiceBindings> voiceBindings = {
    VoiceBindings {
        .voiceName = claveKey,
        .midiChannel = claveChannel,
        .generate = rhythm::claveFunction,
        .generateFromOther = rhythm::claveFromFunction,
        .variation = rhythm::rhythmicVariation,
    },
    VoiceBindings {
        .voiceName = cascaraKey,
        .midiChannel = cascaraChannel,
        .generate = rhythm::cascaraFunction,
        .generateFromOther = rhythm::cascaraFromFunction,
        .variation = placeholderGenerationFunction2,
    },
    VoiceBindings {
        .voiceName = subdivisionsKey,
        .midiChannel = subdivisionsChannel,
        .generate = rhythm::fillSubdivisionsFunction,
        .generateFromOther = rhythm::fillSubdivisionsFunction,
        .variation = placeholderGenerationFunction2,
    },
    VoiceBindings {
        .voiceName = harmonyKey,
        .midiChannel = harmonyChannel,
        .generate = harmony::chordsFunction,
        .generateFromOther = harmony::chordsFromFunction,
        .variation = placeholderGenerationFunction2,
    },
    VoiceBindings {
        .voiceName = bassKey,
        .midiChannel = bassChannel,
        .generate = melody::bassFunction,
        .generateFromOther = melody::bassFromFunction,
        .variation = placeholderGenerationFunction2,
    },
    VoiceBindings {
        .voiceName = melodyKey,
        .midiChannel = melodyChannel,
        .generate = melody::melodyFunction,
        .generateFromOther = melody::melodyFromFunction,
        .variation = placeholderGenerationFunction2,
    }
};



class Voice {
public:
    PlayQueue& playQueue;
    VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation; // TODO: decouple this class from any idea of ornamentation/connecting. there should just be a base and a collection of related phrases. or just a collection.
    bool muteConnecting;
    // Phrase base;
    // vector<Phrase> phrases;
    VoiceSchedule schedule;
    
    GenerationFunction2 variationFunction;

    void variation();

    // schedule of phrases to play
    // that lives in the play queue


    
    Voice(VoiceName name, int midiChannel, bool mute, PlayQueue& playQueue):
    // Voice(VoiceName name, int midiChannel, bool mute, shared_ptr<PlayQueue> playQueue):
        playQueue(playQueue), name(name), midiChannel(midiChannel), mute(mute) {
            // initPhraseVector();
        };

    Voice(PlayQueue& playQueue, VoiceBindings vb) : playQueue(playQueue), name(vb.voiceName), midiChannel(vb.midiChannel), mute(false), variationFunction(vb.variation) {};
    // Voice(shared_ptr<PlayQueue> playQueue, VoiceBindings vb) : playQueue(playQueue), name(vb.voiceName), midiChannel(vb.midiChannel), mute(false), variationFunction(vb.variation) {};
    
    // void initPhraseVector() {
    //     phrases.clear();
    //     phrases.push_back(base);
    // }
};


