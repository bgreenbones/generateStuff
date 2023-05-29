/*
  ==============================================================================

    GenerateStuffEditorState.h
    Created: 12 Feb 2023 10:25:30pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"
#include "Subdivision.h"



class TransformEditorState {
    
};

class ExpressionEditorState {
    
};

class VoiceSettingsEditorState {
    
};

class OrnamentationEditorState {
public:
    // connecting / runs
    bool muteConnecting = false;
    bool muteOrnaments = false;
    double connectingProbability = 0.5;
    double connectingAssociation = 0.5;
    double connectingLength = 0.5;
    bool improviseConnecting = false;
    bool improviseOrnaments = false;
    
    // ornaments
    bool allowflams = true;
    bool allowDrags = false;
    bool allowRuffs = false;
    double ornamentProbability = 0.5;
    double ornamentBreadth = 0.5;
};

// static const Position defaultPhraseStartTime = Bars(0);

class GenerateStuffEditorState {
public:
    GenerateStuffEditorState(juce::AudioProcessorValueTreeState const& apvts): apvts(apvts) {}
    // general
    beats subdivision = 1./4.;
    bars phraseLengthBars = 2.;
    beats phraseLengthBeats = 0.;
    beats displace = 0.;
    bars startBar = 1.;
    bars stopBar = 3.;
    bars phraseStartTime = 0;
    
    typedef string VoiceName;
    VoiceName useAsSourcePhraseKey = "";
    map<VoiceName, OrnamentationEditorState> ornamentationStates;
    map<VoiceName, TransformEditorState> transformStates;
    map<VoiceName, ExpressionEditorState> expressionStates;
    map<VoiceName, VoiceSettingsEditorState> voiceSettingsStates;
    juce::AudioProcessorValueTreeState const& apvts;

    double probabilityOfDouble; // not yet using for 'cascara' abstraction
    
    Bars getStartTime() const { return Bars(startBar - 1., true); }
    Bars getStopTime() const { return Bars(startBar - 1., true); }
    
    Duration getPhraseLength() const {
        Duration result = Bars(phraseLengthBars, true) + Beats(phraseLengthBeats, true);
        return result;
    }
    
    Subdivision getSubdivision() const {
        return Subdivision(Beats(subdivision), getStartTime(), getPhraseLength());
    }
    
    Beats getDisplacement() const { return Beats(displace, true); }
    
    
    float getKnobValue(string parameterId) const {
        juce::RangedAudioParameter *parameter = apvts.getParameter(parameterId);
        auto floatParameter = dynamic_cast<juce::AudioParameterFloat*>(parameter);
        jassert (floatParameter);  // If you get an error, the parameter doesn't exist or is of different type
        return floatParameter->get();
    }

    bool getButtonValue(string parameterId) const {
        juce::RangedAudioParameter *parameter = apvts.getParameter(parameterId);
        auto boolParameter = dynamic_cast<juce::AudioParameterBool*>(parameter);
        jassert (boolParameter);  // If you get an error, the parameter doesn't exist or is of different type
        return boolParameter->get();
    }

    juce::String getChoiceValue(string parameterId) const {
        juce::RangedAudioParameter *parameter = apvts.getParameter(parameterId);
        auto choiceParameter = dynamic_cast<juce::AudioParameterChoice*>(parameter);
        jassert (choiceParameter);  // If you get an error, the parameter doesn't exist or is of different type
        return choiceParameter->getCurrentChoiceName();
    }

    
};
