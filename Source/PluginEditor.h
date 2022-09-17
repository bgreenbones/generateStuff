/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GenerateStuffAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           private juce::Slider::Listener,
                                           private juce::Button::Listener
{
public:
    GenerateStuffAudioProcessorEditor (GenerateStuffAudioProcessor&);
    ~GenerateStuffAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked (juce::Button* button) override;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GenerateStuffAudioProcessor& audioProcessor;
    
    juce::Slider probabilityOfDouble;
    juce::TextButton randomCascaraButton;
    juce::TextButton randomClaveButton;
    juce::TextButton cascaraFromClaveButton;
    juce::TextButton claveFromCascaraButton;
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
    void updateSubdivisionState(float subdivision);
    juce::TextEditor phraseLengthBars;
    juce::TextEditor phraseLengthBeats;
    void updatePhraseLengthState();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenerateStuffAudioProcessorEditor)
};
