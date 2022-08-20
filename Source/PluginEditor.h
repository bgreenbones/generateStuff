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
    juce::TextButton generatePhrase;
    juce::TextButton claveFromCascaraButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenerateStuffAudioProcessorEditor)
};
