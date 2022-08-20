/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GenerateStuffAudioProcessorEditor::GenerateStuffAudioProcessorEditor (GenerateStuffAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    probabilityOfDouble.setSliderStyle (juce::Slider::LinearBarVertical);
    probabilityOfDouble.setRange (0.0, 1.0, 0.01);
    probabilityOfDouble.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    probabilityOfDouble.setPopupDisplayEnabled (true, false, this);
    probabilityOfDouble.setTextValueSuffix (" p(double)");
    probabilityOfDouble.setValue(0.2);
    addAndMakeVisible (&probabilityOfDouble);
    probabilityOfDouble.addListener(this);
    
    generatePhrase.setTitle("Cascara");
    generatePhrase.setName("randomCascara"); // todo: use vars
    addAndMakeVisible (&generatePhrase);
    generatePhrase.addListener(this);
    
    claveFromCascaraButton.setTitle("Clave");
    claveFromCascaraButton.setName("claveFromCascara");
    addAndMakeVisible (&claveFromCascaraButton);
    claveFromCascaraButton.addListener(this);
}

GenerateStuffAudioProcessorEditor::~GenerateStuffAudioProcessorEditor()
{
}

//==============================================================================
void GenerateStuffAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void GenerateStuffAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    probabilityOfDouble.setBounds (40, 30, 20, getHeight() - 60);
    generatePhrase.setBounds (70, 30, 40, getHeight() - 60);
    claveFromCascaraButton.setBounds (120, 30, 40, getHeight() - 60);
    
}

void GenerateStuffAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.probabilityOfDouble = probabilityOfDouble.getValue();
}

void GenerateStuffAudioProcessorEditor::buttonClicked (juce::Button *button)
{
    auto buttonName = button->getName();
    auto generatePhraseName = generatePhrase.getName();
    auto claveFromCascaraButtonName = claveFromCascaraButton.getName();
    
    auto buttonEqualsGen = button->getName().equalsIgnoreCase(generatePhrase.getName());
    auto buttonEqualsClave = button->getName().equalsIgnoreCase(claveFromCascaraButton.getName());
    
    if (buttonEqualsGen) {
        audioProcessor.cascara();
    } else if (buttonEqualsClave) {
        audioProcessor.clave();
    }
    
}

