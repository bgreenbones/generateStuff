/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VoiceManager.h"
#include "GenerateStuffEditorState.h"


//==============================================================================
/**
*/
class GenerateStuffAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GenerateStuffAudioProcessorEditor (GenerateStuffAudioProcessor&);
    ~GenerateStuffAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GenerateStuffAudioProcessor& audioProcessor;
    Ensemble& ensemble;
    GenerateStuffEditorState& editorState;
    VoiceManager voiceManager;
    
//    juce::Slider probabilityOfDouble;
    juce::TextButton generateButton { "generate" };
    juce::Label subdivisionLabel;
    juce::TextButton subDivButton1 { "1/1" };
    juce::TextButton subDivButton2 { "1/2" };
    juce::TextButton subDivButton3 { "1/3" };
    juce::TextButton subDivButton4 { "1/4" };
    juce::TextButton subDivButton5 { "1/5" };
    juce::TextButton subDivButton6 { "1/6" };
    juce::TextButton subDivButton7 { "1/7" };
    juce::TextButton subDivButton8 { "1/8" };
    juce::TextButton subDivButton9 { "1/9" };
    vector<juce::TextButton*> subdivisionButtons {
        &subDivButton1, &subDivButton2, &subDivButton3, &subDivButton4, &subDivButton5,
        &subDivButton6, &subDivButton7, &subDivButton8, &subDivButton9
    };
    // 1st column
    juce::Label phraseLengthBarsLabel;
    juce::TextEditor phraseLengthBars;
    juce::Label startBarLabel;
    juce::TextEditor startBar;
    

    void updateEditorState();

    int xPadding = 20;
    int yPadding = 20;
    int decorationDividerX = -1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenerateStuffAudioProcessorEditor)
};

