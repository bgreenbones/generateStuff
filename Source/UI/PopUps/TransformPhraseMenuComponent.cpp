/*
  ==============================================================================

    TransformPhraseMenuComponent.cpp
    Created: 29 Mar 2023 12:13:36am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "TransformPhraseMenuComponent.h"



TransformPhraseMenuComponent::TransformPhraseMenuComponent(VoiceName voiceName,
                                                           shared_ptr<GenerateStuffEditorState> editorState):
                                                                VoiceEditor(voiceName, editorState)
{
    if (editorState->transformStates.find(voiceName) == editorState->transformStates.end()) {
        editorState->transformStates.emplace(voiceName, TransformEditorState());
    }

    flipButton.onClick = [this]() {
//            generator.flipClave(voiceManager.selectedPhraseKeyState);
    };
    addAndMakeVisible(&flipButton);

}

int TransformPhraseMenuComponent::placeWorkspace() {
    int xCursor = margin * 2;
    int yCursor = margin * 2;
    int yCursorReset = yCursor;
    
    int menuWidth = editorWidth - margin * 2;
    int menuHeight = editorHeight - margin * 2;
    workspaceHeight = menuHeight - margin * 2 - finalizeButtonHeight - spaceBetweenControls * 2;
    int buttonWidth = menuWidth / 8;
    int buttonHeight = menuHeight / 6;
    int sliderHeight = workspaceHeight;
    
    flipButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(1, workspaceHeight));
    xCursor += buttonWidth + spaceBetweenControls;
    
    return 2 * margin + workspaceHeight + spaceBetweenControls;
}

void TransformPhraseMenuComponent::updateMenuState() { }
