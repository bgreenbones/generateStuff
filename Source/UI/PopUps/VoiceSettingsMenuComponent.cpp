/*
  ==============================================================================

    VoiceSettingsMenuComponent.cpp
    Created: 29 Mar 2023 12:13:58am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceSettingsMenuComponent.h"


VoiceSettingsMenuComponent::VoiceSettingsMenuComponent(VoiceName voiceName,
                                                       shared_ptr<GenerateStuffEditorState> editorState):
                                                            VoiceEditor(voiceName, editorState)
{
    if (editorState->voiceSettingsStates.find(voiceName) == editorState->voiceSettingsStates.end()) {
        editorState->voiceSettingsStates.emplace(voiceName, VoiceSettingsEditorState());
    }
}

int VoiceSettingsMenuComponent::placeWorkspace()
{
    int cursorY = margin * 2;
    return cursorY;
}

void VoiceSettingsMenuComponent::updateMenuState() { }
