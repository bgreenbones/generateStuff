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
