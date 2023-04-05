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

//static juce::AudioProcessorParameterGroup getVoiceParameters(VoiceName voiceName, vector<Parameter> voiceParameters) {
//    juce::AudioProcessorParameterGroup parameterGroup(voiceName, voiceName, ":");
//    for (auto parameter : voiceParameters) {
//        switch (parameter.type) {
//            case ParameterType::knob:
//                parameterGroup.addChild(make_unique<juce::AudioParameterFloat>(parameter.key, parameter.name, parameter.knobRange, parameter.defaultKnobValue, parameter.units));
//                break;
//            case ParameterType::button:
//                parameterGroup.addChild(make_unique<juce::AudioParameterBool>(parameter.key, parameter.name, parameter.defaultButtonValue, parameter.units));
//                break;
//            case ParameterType::choice:
//                parameterGroup.addChild(make_unique<juce::AudioParameterChoice>(parameter.key, parameter.name, parameter.choices, parameter.defaultChoiceIndex, parameter.units));
//                break;
//        }
//    }
//    return parameterGroup;
//}
//
//static juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
//    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
//    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getVoiceParameters(claveKey, claveParameters)));
////    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getCascaraParameters()));
//    return parameterLayout;
//}

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

static const GenerationFunction placeholderGenerationFunction = [](Phrase phrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) { return phrase; };

static const vector<VoiceBindings> voiceBindings = {
    VoiceBindings {
        .voiceName = claveKey,
        .midiChannel = claveChannel,
        .generate = rhythm::claveFunction,
        .generateFromOther = rhythm::claveFromFunction,
//        .getParameters = getClaveParameters
    },
    VoiceBindings {
        .voiceName = cascaraKey,
        .midiChannel = cascaraChannel,
        .generate = rhythm::cascaraFunction,
        .generateFromOther = rhythm::cascaraFromFunction,
//        .getParameters = getCascaraParameters
    },
    VoiceBindings {
        .voiceName = subdivisionsKey,
        .midiChannel = subdivisionsChannel,
        .generate = rhythm::fillSubdivisionsFunction,
        .generateFromOther = rhythm::fillSubdivisionsFunction,
//        .getParameters = getClaveParameters
    },
    VoiceBindings {
        .voiceName = harmonyKey,
        .midiChannel = harmonyChannel,
        .generate = harmony::chordsFunction,
        .generateFromOther = harmony::chordsFromFunction,
//        .getParameters = getClaveParameters
    },
    VoiceBindings {
        .voiceName = bassKey,
        .midiChannel = bassChannel,
        .generate = melody::bassFunction,
        .generateFromOther = melody::bassFromFunction,
//        .getParameters = getClaveParameters
    },
    VoiceBindings {
        .voiceName = melodyKey,
        .midiChannel = melodyChannel,
        .generate = melody::melodyFunction,
        .generateFromOther = melody::melodyFromFunction,
//        .getParameters = getClaveParameters
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


