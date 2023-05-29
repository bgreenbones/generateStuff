/*
  ==============================================================================

    TransformPhraseMenuComponent.h
    Created: 29 Mar 2023 12:13:36am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "PopUpComponent.h"
#include "Probability.h"
#include "GenerateStuffEditorState.h"
#include "Ensemble.h"
#include "Rhythm.h"
#include "PluginProcessor.h"


using std::shared_ptr;


class TransformPhraseMenuComponent : public VoiceEditor {
    juce::TextButton flipButton { "flip" };
    juce::TextButton stabilizeRhythmButton { "stabilize rhythm" };
    juce::TextButton destabilizeRhythmButton { "destabilize rhythm" };
public:
    
    TransformPhraseMenuComponent(VoiceName voiceName,
                                 GenerateStuffAudioProcessor& processor);
    
//    void resized() override {
    int placeWorkspace() override;
    void updateMenuState() override;
};
