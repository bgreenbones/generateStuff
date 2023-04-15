/*
  ==============================================================================

    ExpressionMenuComponent.h
    Created: 29 Mar 2023 12:15:04am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "PopUpComponent.h"
#include "GenerateStuffEditorState.h"

using std::shared_ptr;

class ExpressionMenuComponent : public VoiceEditor {
public:
    ExpressionMenuComponent(VoiceName voiceName, GenerateStuffAudioProcessor &processor);
    int placeWorkspace() override;
    void updateMenuState() override;
};
