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

using std::shared_ptr;

class VoiceSettingsMenuComponent : public VoiceEditor {
public:
    VoiceSettingsMenuComponent(VoiceName voiceName, shared_ptr<GenerateStuffEditorState> editorState);
    int placeWorkspace() override;
    void updateMenuState() override;
};
