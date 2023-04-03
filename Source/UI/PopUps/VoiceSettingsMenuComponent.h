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

using std::shared_ptr;

class VoiceSettingsMenuComponent : public VoiceEditor {
    GenerateStuffAudioProcessor &processor;
public:
    VoiceSettingsMenuComponent(VoiceName voiceName, GenerateStuffAudioProcessor& processor);
    int placeWorkspace() override;
    void updateMenuState() override;
};
