/*
  ==============================================================================

    ExpressionMenuComponent.cpp
    Created: 29 Mar 2023 12:15:04am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "ExpressionMenuComponent.h"



ExpressionMenuComponent::ExpressionMenuComponent(VoiceName voiceName,
                                                 GenerateStuffAudioProcessor &processor):
                                                            VoiceEditor(voiceName, processor)
{
    if (editorState->expressionStates.find(voiceName) == editorState->expressionStates.end()) {
        editorState->expressionStates.emplace(voiceName, ExpressionEditorState());
    }
}

int ExpressionMenuComponent::placeWorkspace()
{
    int cursorY = margin * 2;
    return cursorY;
}


void ExpressionMenuComponent::updateMenuState() { }
