/*
  ==============================================================================

    VoiceSettingsMenuComponent.h
    Created: 29 Mar 2023 12:13:58am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "PopUpComponent.h"
#include "GenerateStuffEditorState.h"
#include "PluginProcessor.h"

using std::shared_ptr, std::unique_ptr;

class VoiceSettingsMenuComponent : public VoiceEditor {
    GenerateStuffAudioProcessor &processor;
    
    juce::OwnedArray<juce::Label> sliderLabels;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    
    juce::OwnedArray<juce::TextButton> buttons;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;
    
    juce::OwnedArray<juce::Label> comboBoxLabels;
    juce::OwnedArray<juce::ComboBox> comboBoxes;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachments;
    
public:
    VoiceSettingsMenuComponent(VoiceName voiceName, GenerateStuffAudioProcessor& processor);
    int placeWorkspace() override;
    void updateMenuState() override;
};
