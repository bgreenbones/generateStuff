/*
  ==============================================================================

    OrnamentationMenuComponent.h
    Created: 29 Mar 2023 12:14:45am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "PopUpComponent.h"
#include "Probability.h"
#include "GenerateStuffEditorState.h"

using std::shared_ptr;


class OrnamentationMenuComponent : public VoiceEditor {
    juce::TextButton addConnectingButton { "connecting" };
    juce::TextButton muteConnectingButton { "mute connecting" };
    juce::Slider connectingProbability;
    juce::Slider connectingAssociation;
    juce::Slider connectingLength;
    juce::TextButton addOrnamentsButton { "orns" };
    juce::TextButton muteOrnamentsButton { "mute ornaments" };
    juce::TextButton flamButton { "flam" };
    juce::TextButton dragButton { "drag" };
    juce::TextButton ruffButton { "ruff" };
    juce::Slider ornamentProbability;
    juce::Slider ornamentBreadth;
    juce::TextButton regenerateConnecting { "regen connecting" }; // re-gen connecting on loop
    juce::TextButton regenerateOrnaments { "regen orns" }; // re-gen ornaments on loop
    GenerateStuffAudioProcessor &audioProcessor;
public:
    
    OrnamentationMenuComponent(VoiceName voiceName,
                               GenerateStuffAudioProcessor &processor);
    
//    void resized() override {
    int placeWorkspace() override;
    void updateMenuState() override;
};
