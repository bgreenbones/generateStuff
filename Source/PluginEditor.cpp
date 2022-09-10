/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <string>
#include <stdexcept>

using namespace std;

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

    for (float subdivisionDenominator = 1; subdivisionDenominator <= 9; subdivisionDenominator++) {
        int subdivisionIndex = subdivisionDenominator - 1;
        auto subdivisionButton = subdivisionButtons[subdivisionIndex];
        int subdivisionGroupId = 1832; // just some number
        subdivisionButton->setRadioGroupId(subdivisionGroupId);
        subdivisionButton->setClickingTogglesState(true);
        addAndMakeVisible(subdivisionButton);
        subdivisionButton->setToggleState(false, juce::dontSendNotification);
        subdivisionButtons[subdivisionIndex]->onClick = [this, subdivisionDenominator] { updateSubdivisionState (1.0 / subdivisionDenominator); };
    }
    int defaultSubdivisionIndex = (int) (1.0 / audioProcessor.generator.rhythms[0].subdivision) - 1;
    subdivisionButtons[defaultSubdivisionIndex]->setToggleState(true, juce::dontSendNotification);
    
    phraseLengthBars.setJustification (juce::Justification::centred);
    phraseLengthBeats.setJustification (juce::Justification::centred);
    phraseLengthBars.setInputRestrictions (4, juce::String {".1234567890"});
    phraseLengthBeats.setInputRestrictions (4, juce::String {".1234567890"});
    addAndMakeVisible (&phraseLengthBars);
    addAndMakeVisible (&phraseLengthBeats);
    phraseLengthBars.onTextChange = [this] { updatePhraseLengthState(); };
    phraseLengthBeats.onTextChange = [this] { updatePhraseLengthState(); };
    juce::String barsString = juce::String::formatted("%.2f", audioProcessor.generator.activePhrase.bars);
    juce::String beatsString = juce::String::formatted("%.2f", audioProcessor.generator.activePhrase.beats);
    phraseLengthBars.setText(barsString);
    phraseLengthBeats.setText(beatsString);
}

void GenerateStuffAudioProcessorEditor::updatePhraseLengthState() {
    float bars = -1;
    float beats = -1;
    try {
        auto barsString = phraseLengthBars.getTextValue().toString().toStdString();
        bars = stof(barsString);
    } catch (const invalid_argument& ia) { }
    try {
        auto beatsString = phraseLengthBeats.getTextValue().toString().toStdString();
        beats = stof(beatsString);
    } catch (const invalid_argument& ia) { }

    if (bars >= 0) {
        audioProcessor.generator.setPhraseLengthBars(bars);
    }
    if (beats >= 0) {
        audioProcessor.generator.setPhraseLengthBeats(beats);
    }
}


void GenerateStuffAudioProcessorEditor::updateSubdivisionState(float subdivision) {
    audioProcessor.generator.activeRhythm.subdivision = subdivision;
}

GenerateStuffAudioProcessorEditor::~GenerateStuffAudioProcessorEditor()
{
    subdivisionButtons.clear();
    subdivisionButtons.shrink_to_fit();
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
    int totalSpaceBetweenSubDivButtons = spaceBetweenSubDivButtons * ((int) subdivisionButtons.size() - 1);
    buttonHeight = (height - totalSpaceBetweenSubDivButtons) / subdivisionButtons.size();
    for (auto i = 0;
         i < subdivisionButtons.size();
         i++) {
        subdivisionButtons[i]->setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        yCursor += buttonHeight + spaceBetweenSubDivButtons;
    }
    
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    int numberInputWidth = buttonWidth;
    int numberInputHeight = buttonHeight;
    phraseLengthBars.setBounds (xCursor, yCursor, numberInputWidth, numberInputHeight);
    yCursor += numberInputHeight + spaceBetweenControls;
    phraseLengthBeats.setBounds (xCursor, yCursor, numberInputWidth, numberInputHeight);
    
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

