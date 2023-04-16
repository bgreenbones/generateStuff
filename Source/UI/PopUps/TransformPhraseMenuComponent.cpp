/*
  ==============================================================================

    TransformPhraseMenuComponent.cpp
    Created: 29 Mar 2023 12:13:36am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "TransformPhraseMenuComponent.h"



TransformPhraseMenuComponent::TransformPhraseMenuComponent(VoiceName voiceName,
                                                           GenerateStuffAudioProcessor &processor):
                                                                VoiceEditor(voiceName, processor)
{
    if (editorState->transformStates.find(voiceName) == editorState->transformStates.end()) {
        editorState->transformStates.emplace(voiceName, TransformEditorState());
    }

    flipButton.onClick = [this]() {
        Phrase flipped = rhythm::flip(playQueue->getVoice(this->voiceName).base);
        playQueue->queuePhrase(this->voiceName, flipped);
    };
    addAndMakeVisible(&flipButton);

    
    stabilizeRhythmButton.onClick = [this]() {
        Phrase stabilized = rhythm::stabilityFilter(playQueue->getVoice(this->voiceName).base, Direction::up);
        playQueue->queuePhrase(this->voiceName, stabilized);
    };
    destabilizeRhythmButton.onClick = [this]() {
        Phrase destabilized = rhythm::stabilityFilter(playQueue->getVoice(this->voiceName).base, Direction::down);
        playQueue->queuePhrase(this->voiceName, destabilized);
    };
    addAndMakeVisible(&stabilizeRhythmButton);
    addAndMakeVisible(&destabilizeRhythmButton);

}

int TransformPhraseMenuComponent::placeWorkspace() {
    int xCursor = margin * 2;
    int yCursor = margin * 2;
//    int yCursorReset = yCursor;
    
    int menuWidth = ui::editorWidth - margin * 2;
    int menuHeight = ui::editorHeight - margin * 2;
    workspaceHeight = menuHeight - margin * 2 - finalizeButtonHeight - ui::spaceBetweenControls * 2;
    int buttonWidth = menuWidth / 8;
//    int buttonHeight = menuHeight / 6;
//    int sliderHeight = workspaceHeight;
    
    flipButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(1, workspaceHeight));
    xCursor += buttonWidth + ui::spaceBetweenControls;
    
    stabilizeRhythmButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(1, workspaceHeight));
    xCursor += buttonWidth + ui::spaceBetweenControls;
    
    destabilizeRhythmButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(1, workspaceHeight));
    xCursor += buttonWidth + ui::spaceBetweenControls;
    
    return 2 * margin + workspaceHeight + ui::spaceBetweenControls;
}

void TransformPhraseMenuComponent::updateMenuState() { }
