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


using std::shared_ptr;


class TransformPhraseMenuComponent : public VoiceEditor {
    juce::TextButton flipButton { "flip" };
public:
    
    TransformPhraseMenuComponent(VoiceName voiceName,
                                 shared_ptr<GenerateStuffEditorState> editorState);
    
//    void resized() override {
    int placeWorkspace() override;
    void updateMenuState() override;
};
