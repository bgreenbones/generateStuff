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

    for (float subDiv = 1; subDiv <= 9; subDiv++) {
        int subDivIndex = subDiv - 1;
        auto pSubDivButton = subDivButtons[subDivIndex];
        int subdivisionGroupId = 1832; // just some number
        pSubDivButton->setRadioGroupId(subdivisionGroupId);
        pSubDivButton->setClickingTogglesState(true);
        addAndMakeVisible(pSubDivButton);
        pSubDivButton->setToggleState(false, juce::dontSendNotification);
        subDivButtons[subDivIndex]->onClick = [this, subDiv] { updateSubdivisionState (1.0 / subDiv); };
    }
    int defaultSubdivisionIndex = (int) (1.0 / audioProcessor.generator.rhythms[0].subdivision) - 1;
    subDivButtons[defaultSubdivisionIndex]->setToggleState(true, juce::dontSendNotification);
}

void GenerateStuffAudioProcessorEditor::updateSubdivisionState(float subdivision) {
    audioProcessor.generator.rhythms[0].subdivision = subdivision;
}

GenerateStuffAudioProcessorEditor::~GenerateStuffAudioProcessorEditor()
{
    subDivButtons.clear();
    subDivButtons.shrink_to_fit();
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
    int xPadding = 40;
    int yPadding = 30;
//    int width = getWidth() - 2 * xPadding;
    int height = getHeight() - 2 * yPadding;
    int spaceBetweenControls = 10;
    int sliderWidth = 20;
    int buttonWidth = 40;
    
    int xCursor = xPadding;
    int yCursor = yPadding;
    
    probabilityOfDouble.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    
    int buttonHeight = (height - spaceBetweenControls) / 2;
    generatePhrase.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    claveFromCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    short spaceBetweenSubDivButtons = 5;
    int totalSpaceBetweenSubDivButtons = spaceBetweenSubDivButtons * ((int) subDivButtons.size() - 1);
    buttonHeight = (height - totalSpaceBetweenSubDivButtons) / subDivButtons.size();
    for (auto i = 0;
         i < subDivButtons.size();
         i++) {
        subDivButtons[i]->setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        yCursor += buttonHeight + spaceBetweenSubDivButtons;
    }
    
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

