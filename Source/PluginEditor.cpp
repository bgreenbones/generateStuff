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
    setSize (600, 300);
    
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
    
    addRollsButton.setTitle(fillClaveName);
    addRollsButton.setName(fillClaveName);
    addAndMakeVisible (&addRollsButton);
    addRollsButton.addListener(this);

    int subdivisionGroupId = 1832; // just some number
    for (float subdivisionDenominator = 1; subdivisionDenominator <= 9; subdivisionDenominator++) {
        int subdivisionIndex = subdivisionDenominator - 1;
        auto subdivisionButton = subdivisionButtons[subdivisionIndex];
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
    
    auto testOnClick = [] { return; };
//    auto testOnClick = [&] { audioProcessor.queuePlayable(audioProcessor.generator.claveFromCascara()); };
    int selectRhythmGroupId = 98374; // random
    selectCascaraButton.setRadioGroupId(selectRhythmGroupId);
    selectCascaraButton.setClickingTogglesState(true);
    selectCascaraButton.setToggleState(false, juce::dontSendNotification);
    selectCascaraButton.onClick = [this]() { audioProcessor.queuePlayable(audioProcessor.generator.cascara()); };
    addAndMakeVisible (&selectCascaraButton);
    
    selectClaveButton.setRadioGroupId(selectRhythmGroupId);
    selectClaveButton.setClickingTogglesState(true);
    selectClaveButton.setToggleState(false, juce::dontSendNotification);
    selectClaveButton.onClick = testOnClick;
    addAndMakeVisible(&selectClaveButton);
    
    rollProbability.onValueChange = testOnClick;
    addAndMakeVisible(&rollProbability);
    rollProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    rollProbability.setRange (0.0, 1.0, 0.01);
    rollProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollProbability.setPopupDisplayEnabled (true, false, this);
    rollProbability.setTextValueSuffix ("p(roll)");
    rollProbability.setValue(1.0);
    
    rollAssociation.onValueChange = testOnClick;
    addAndMakeVisible(&rollAssociation);
    rollAssociation.setSliderStyle (juce::Slider::LinearBarVertical);
    rollAssociation.setRange (0.0, 1.0, 0.01);
    rollAssociation.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollAssociation.setPopupDisplayEnabled (true, false, this);
    rollAssociation.setTextValueSuffix ("roll association");
    rollAssociation.setValue(0.5);
    
    rollLength.onValueChange = testOnClick;
    addAndMakeVisible(&rollLength);
    rollLength.setSliderStyle (juce::Slider::LinearBarVertical);
    rollLength.setRange (0.0, 1.0, 0.01);
    rollLength.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollLength.setPopupDisplayEnabled (true, false, this);
    rollLength.setTextValueSuffix ("roll length");
    rollLength.setValue(0.5);
    
    addOrnamentsButton.onClick = testOnClick;
    addAndMakeVisible(&addOrnamentsButton);
    flamButton.setClickingTogglesState(true);
    flamButton.setToggleState(false, juce::dontSendNotification);
    flamButton.onClick = testOnClick;
    addAndMakeVisible(&flamButton);
    dragButton.setClickingTogglesState(true);
    dragButton.setToggleState(false, juce::dontSendNotification);
    dragButton.onClick = testOnClick;
    addAndMakeVisible(&dragButton);
    ruffButton.setClickingTogglesState(true);
    ruffButton.setToggleState(false, juce::dontSendNotification);
    ruffButton.onClick = testOnClick;
    addAndMakeVisible(&ruffButton);

    ornamentProbability.onValueChange = testOnClick;
    addAndMakeVisible(&ornamentProbability);
    ornamentProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentProbability.setRange (0.0, 1.0, 0.01);
    ornamentProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentProbability.setPopupDisplayEnabled (true, false, this);
    ornamentProbability.setTextValueSuffix ("ornament probability");
    ornamentProbability.setValue(0.3);
    
    ornamentBreadth.onValueChange = testOnClick;
    addAndMakeVisible(&ornamentBreadth);
    ornamentBreadth.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentBreadth.setRange (0.0, 1.0, 0.01);
    ornamentBreadth.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentBreadth.setPopupDisplayEnabled (true, false, this);
    ornamentBreadth.setTextValueSuffix ("ornament breadth");
    ornamentBreadth.setValue(0.2);
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
    
    if (decorationDividerX != -1) {
        g.setColour (juce::Colours::orange);
        juce::Line<float> line (juce::Point<float> (decorationDividerX, getHeight() - yPadding),
                                juce::Point<float> (decorationDividerX, yPadding));
        g.drawLine (line, 2.0f);
    }
}

void GenerateStuffAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int height = getHeight() - 2 * yPadding;
    int spaceBetweenControls = 10;
    int sliderWidth = 20;
    int buttonWidth = 40;
    
    int xCursor = xPadding;
    int yCursor = yPadding;
    
    short spaceBetweenSubDivButtons = 2;
    int numberOfPhraseLengthInputs = 2;
    int totalSpaceBetweenSubDivButtons = spaceBetweenSubDivButtons * ((int) subdivisionButtons.size() - 1);
    int subDivButtonHeight = (height - totalSpaceBetweenSubDivButtons) / (subdivisionButtons.size() + numberOfPhraseLengthInputs);
    
    int numberInputWidth = buttonWidth;
    int numberInputHeight = subDivButtonHeight;
    phraseLengthBars.setBounds (xCursor, yCursor, numberInputWidth, numberInputHeight);
    yCursor += numberInputHeight + spaceBetweenSubDivButtons;
    phraseLengthBeats.setBounds (xCursor, yCursor, numberInputWidth, numberInputHeight);
    yCursor += numberInputHeight + spaceBetweenSubDivButtons;
    
//    buttonHeight = (height - totalSpaceBetweenSubDivButtons) / subdivisionButtons.size();
    for (auto i = 0;
         i < subdivisionButtons.size();
         i++) {
        subdivisionButtons[i]->setBounds (xCursor, yCursor, buttonWidth, subDivButtonHeight);
        yCursor += subDivButtonHeight + spaceBetweenSubDivButtons;
    }
    
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    probabilityOfDouble.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    
    auto getButtonHeight = [height, spaceBetweenControls] (int rows) { return (height - spaceBetweenControls * (rows - 1)) / rows; };
    int buttonHeight = getButtonHeight(2);
    randomCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    randomClaveButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    cascaraFromClaveButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    claveFromCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    decorationDividerX = xCursor;
    xCursor += spaceBetweenControls;

    selectCascaraButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    selectClaveButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    rollProbability.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    rollAssociation.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    rollLength.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    
    addRollsButton.setBounds (xCursor, yCursor, buttonWidth, height);
    xCursor += buttonWidth + spaceBetweenControls;
    
    buttonHeight = getButtonHeight(3);
    flamButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    dragButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    ruffButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    ornamentProbability.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    ornamentBreadth.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    addOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, height);
    xCursor += buttonWidth + spaceBetweenControls;
    
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
    auto fillClaveName = addRollsButton.getName();
    
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

