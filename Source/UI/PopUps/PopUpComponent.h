/*
  ==============================================================================

    PopUpComponent.h
    Created: 29 Mar 2023 12:13:06am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CustomUIs.h"
#include "Voice.h"
#include "GenerateStuffEditorState.h"
#include "PlayQueue.h"
#include "PluginProcessor.h"

using std::unique_ptr;

class PopUpComponent : public juce::Component
{
protected:
    int margin = 20;
    int finalizeButtonWidth = 150;
    int finalizeButtonHeight = 30;
    int workspaceHeight = 0;
    int workspaceWidth = ui::editorWidth - margin * 4;
public:
    PopUpComponent();
    
    ~PopUpComponent() override;
    void paint (juce::Graphics& g) override;
    
    virtual int placeWorkspace();
    virtual void updateMenuState() = 0;
    void resized() override;
    void drawFinalizeButtons(int cursorY);
    
private:
    juce::TextButton okButton { "ok" };
    juce::TextButton cancelButton { "cancel" };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopUpComponent)
};

class VoiceEditor : public PopUpComponent {
public:
    VoiceName voiceName;
    GenerateStuffEditorState& editorState;
    PlayQueue& playQueue;
    VoiceEditor(VoiceName voiceName,
                GenerateStuffAudioProcessor &processor): voiceName(voiceName), editorState(processor.editorState), playQueue(processor.playQueue) {
        
    }
};
