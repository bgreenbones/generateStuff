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
    
    juce::String cascaraName = "random cascara";
    juce::String claveName = "random clave";
    juce::String claveFromCascaraName = "clave from cascara";
    juce::String cascaraFromClaveName = "cascara from clave";
    juce::String fillClaveName = "fill clave";
    randomCascaraButton.setTitle(cascaraName);
    randomCascaraButton.setName(cascaraName);
    addAndMakeVisible (&randomCascaraButton);
    randomCascaraButton.addListener(this);
    
    randomClaveButton.setTitle(claveName);
    randomClaveButton.setName(claveName);
    addAndMakeVisible (&randomClaveButton);
    randomClaveButton.addListener(this);
    
    claveFromCascaraButton.setTitle(claveFromCascaraName);
    claveFromCascaraButton.setName(claveFromCascaraName);
    addAndMakeVisible (&claveFromCascaraButton);
    claveFromCascaraButton.addListener(this);
    
    cascaraFromClaveButton.setTitle(cascaraFromClaveName);
    cascaraFromClaveButton.setName(cascaraFromClaveName);
    addAndMakeVisible (&cascaraFromClaveButton);
    cascaraFromClaveButton.addListener(this);
    
    fillClaveButton.setTitle(fillClaveName);
    fillClaveButton.setName(fillClaveName);
    addAndMakeVisible (&fillClaveButton);
    fillClaveButton.addListener(this);


    for (float subdivisionDenominator = 1; subdivisionDenominator <= 9; subdivisionDenominator++) {
        int subdivisionIndex = subdivisionDenominator - 1;
        auto subdivisionButton = subdivisionButtons[subdivisionIndex];
        int subdivisionGroupId = 1832; // just some number
        subdivisionButton->setRadioGroupId(subdivisionGroupId);
        subdivisionButton->setClickingTogglesState(true);
        addAndMakeVisible(subdivisionButton);
        subdivisionButton->setToggleState(false, juce::dontSendNotification);
        subdivisionButton->onClick = [=] { updateSubdivisionState (1.f / subdivisionDenominator); };
    }
    int defaultSubdivisionIndex = (int) (1.0 / audioProcessor.generator.subdivision.asBeats()) - 1; // todo: set this
    subdivisionButtons[defaultSubdivisionIndex]->setToggleState(true, juce::dontSendNotification);
    
    phraseLengthBars.setJustification (juce::Justification::centred);
    phraseLengthBeats.setJustification (juce::Justification::centred);
    phraseLengthBars.setInputRestrictions (4, juce::String {".1234567890"});
    phraseLengthBeats.setInputRestrictions (4, juce::String {".1234567890"});
    addAndMakeVisible (&phraseLengthBars);
    addAndMakeVisible (&phraseLengthBeats);
    phraseLengthBars.onTextChange = [this] { updatePhraseLengthState(); };
    phraseLengthBeats.onTextChange = [this] { updatePhraseLengthState(); };
    juce::String barsString = juce::String::formatted("%.2f", Generator::defaultBars);
    juce::String beatsString = juce::String::formatted("%.2f", Generator::defaultBeats);
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
    audioProcessor.generator.setSubdivision(subdivision);
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
    randomCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    claveFromCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    randomClaveButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    cascaraFromClaveButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    fillClaveButton.setBounds (xCursor, yCursor, buttonWidth, height);
    xCursor += buttonWidth + spaceBetweenControls;
    
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
    auto randomCascaraName = randomCascaraButton.getName();
    auto claveFromCascaraName = claveFromCascaraButton.getName();
    auto randomClaveName = randomClaveButton.getName();
    auto cascaraFromClaveName = cascaraFromClaveButton.getName();
    auto fillClaveName = fillClaveButton.getName();
    
    
    auto isCascaraButton = buttonName.equalsIgnoreCase(randomCascaraName);
    auto isClaveButton = buttonName.equalsIgnoreCase(randomClaveName);
    auto isClaveFromCascaraButton = buttonName.equalsIgnoreCase(claveFromCascaraName);
    auto isCascaraFromClaveButton = buttonName.equalsIgnoreCase(cascaraFromClaveName);
    auto isFillClaveButton = buttonName.equalsIgnoreCase(fillClaveName);
    
    if (isCascaraButton) {
        audioProcessor.queuePlayable(audioProcessor.generator.cascara());
    } else if (isClaveButton) {
        audioProcessor.queuePlayable(audioProcessor.generator.clave());
    } else if (isClaveFromCascaraButton) {
        audioProcessor.queuePlayable(audioProcessor.generator.claveFromCascara());
    } else if (isCascaraFromClaveButton) {
        audioProcessor.queuePlayable(audioProcessor.generator.cascaraFromClave());
    } else if (isFillClaveButton) {
        audioProcessor.queuePlayable(audioProcessor.generator.fillClave());
    }
}

