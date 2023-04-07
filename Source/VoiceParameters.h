/*
  ==============================================================================

    VoiceParameters.h
    Created: 4 Apr 2023 8:59:10pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>
#include <JuceHeader.h>


using juce::NormalisableRange, juce::StringArray;
using juce::AudioParameterFloat, juce::AudioParameterChoice, juce::AudioParameterBool;
using std::vector;

enum ParameterType {
    knob, choice, button
};

class Parameter {
public:
    ParameterType type;
    string key;
    string name;
    string units;
    
    // button
    bool defaultButtonValue;
    
    // knob
    NormalisableRange<float> knobRange;
    float defaultKnobValue;
    
    // choice
    StringArray choices;
    int defaultChoiceIndex;

    Parameter(string key, string name, bool defaultButtonValue, string units = ""): type(ParameterType::button), key(key), name(name), units(units), defaultButtonValue(defaultButtonValue) {}
    Parameter(string key, string name, NormalisableRange<float> knobRange, float defaultKnobValue, string units = ""): type(ParameterType::knob), key(key), name(name), units(units), knobRange(knobRange), defaultKnobValue(defaultKnobValue) {}
    Parameter(string key, string name, StringArray choices, int defaultChoiceIndex, string units = ""): type(ParameterType::choice), key(key), name(name), units(units), choices(choices), defaultChoiceIndex(defaultChoiceIndex) {}
};

static const string claveMinNoteLengthKey = "claveMinNoteLength";
static const string claveMaxNoteLengthKey = "claveMaxNoteLength";
static const juce::NormalisableRange<float> claveNoteLengthRange(1.f, 8.f, 1.0);

static const vector<Parameter> claveParameters {
    Parameter(claveMinNoteLengthKey, "min note length", claveNoteLengthRange, 2, " subdivisions"),
    Parameter(claveMaxNoteLengthKey, "max note length", claveNoteLengthRange, 4, " subdivisions")
};


static const juce::NormalisableRange<float> probabilityRange(0.f, 1.f, 0.01);
static const string cascaraDisplaceProbabilityKey = "cascaraDisplaceProbability";
static const string cascaraDoubleProbabilityKey = "cascaraDoubleProbability";

static const vector<Parameter> cascaraParameters {
    Parameter(cascaraDisplaceProbabilityKey, "displacement probability", probabilityRange, 0.5, " probability"),
    Parameter(cascaraDoubleProbabilityKey, "probability of double", probabilityRange, 0.75, " probability")
};


static const string harmonyApproachKey = "harmonyApproach";
static const juce::String randomHarmonyApproachKey = "random";
static const juce::String diatonicHarmonyApproachKey = "diatonic";
static const juce::String smoothishModulationsHarmonyApprachKey = "smoothish modulations";
static const vector<Parameter> harmonyParameters {
    Parameter(harmonyApproachKey, "approach", { randomHarmonyApproachKey, diatonicHarmonyApproachKey, smoothishModulationsHarmonyApprachKey }, 1)
//    Probability chordProbabilityPerAccent = 0.6);
};

static const string bassBurstLengthMinKey = "bassBurstLengthMin";
static const string bassBurstLengthMaxKey = "bassBurstLengthMax";
static const juce::NormalisableRange<float> burstLengthRange(1.f, 8.f, 1.0);
static const string bassBurstGroupingOneKey = "bassBurstGroupingOne";
static const string bassBurstGroupingTwoKey = "bassBurstGroupingTwo";
static const string bassBurstGroupingThreeKey = "bassBurstGroupingThree";
static const vector<Parameter> bassParameters {
    // TODO: add bass parameters and use them
};
