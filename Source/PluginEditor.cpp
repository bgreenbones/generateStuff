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
    : AudioProcessorEditor (&p), audioProcessor (p), generator(p.generator)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 300);
        
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

    cascaraControls.generateButton.setTitle(cascaraName); // TODO: all the voice controls stuff in a function contained in VoiceControls.
    cascaraControls.generateButton.setName(cascaraName);
    addAndMakeVisible (&cascaraControls.generateButton);
    cascaraControls.generateButton.addListener(this);
    
    claveControls.generateButton.setTitle(claveName);
    claveControls.generateButton.setName(claveName);
    addAndMakeVisible (&claveControls.generateButton);
    claveControls.generateButton.addListener(this);
    
    claveControls.generateFromButton.setTitle(claveFromCascaraName);
    claveControls.generateFromButton.setName(claveFromCascaraName);
    addAndMakeVisible (&claveControls.generateFromButton);
    claveControls.generateFromButton.addListener(this);
    
    cascaraControls.generateFromButton.setTitle(cascaraFromClaveName);
    cascaraControls.generateFromButton.setName(cascaraFromClaveName);
    addAndMakeVisible (&cascaraControls.generateFromButton);
    cascaraControls.generateFromButton.addListener(this);
    
    addAndMakeVisible (&cascaraControls.useAsSourceButton);
    addAndMakeVisible(&claveControls.useAsSourceButton);
    
    addRollsButton.onClick = [this]() {
        double rollProb = Probability(rollProbability.getValue());
        double associationProb = Probability(rollAssociation.getValue());
        double rollLengthProb = Probability(rollLength.getValue());

        generator.roll(selectedPhraseKeyState, rollProb, associationProb, rollLengthProb);
        string id = generator.rollsKey(selectedPhraseKeyState);
        function<void()> task = [=]() { generator.roll(selectedPhraseKeyState, rollProb, associationProb, rollLengthProb); };
        audioProcessor.loopTasks.queue(id, task, regenerateRolls.getToggleState());
    };
    addAndMakeVisible (&addRollsButton);

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
    
    phraseLengthBarsLabel.setText("bars", juce::dontSendNotification);
    phraseLengthBeatsLabel.setText("beats", juce::dontSendNotification);
    phraseLengthBarsLabel.attachToComponent(&phraseLengthBars, true);
    phraseLengthBeatsLabel.attachToComponent(&phraseLengthBeats, true);
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
    phraseLengthBars.onFocusLost = [this] {
        juce::String phraseLengthString = juce::String::formatted("%.2f", this->audioProcessor.generator.phraseLengthBars.asBars());
        phraseLengthBars.setText(phraseLengthString);
    };
    phraseLengthBeats.onFocusLost = [this] {
        juce::String phraseLengthString = juce::String::formatted("%.2f", this->audioProcessor.generator.phraseLengthBeats.asBeats());
        phraseLengthBeats.setText(phraseLengthString);
    };
    
    displaceLabel.setText("displace (beats)", juce::dontSendNotification);
    startBarLabel.setText("start (bar)", juce::dontSendNotification);
    stopBarLabel.setText("stop (bar)", juce::dontSendNotification);
    displaceLabel.attachToComponent(&displace, true);
    startBarLabel.attachToComponent(&startBar, true);
    stopBarLabel.attachToComponent(&stopBar, true);
    
    displace.setJustification (juce::Justification::centred);
    startBar.setJustification (juce::Justification::centred);
    stopBar.setJustification (juce::Justification::centred);
    displace.setInputRestrictions (4, juce::String {".1234567890"});
    startBar.setInputRestrictions (4, juce::String {".1234567890"});
    stopBar.setInputRestrictions (4, juce::String {".1234567890"});
    displace.onTextChange = [this] { updateDisplacementState(); };
    startBar.onTextChange = [this] { updateStartingBarState(); };
    stopBar.onTextChange = [this] { updateStoppingBarState(); };
    displace.onFocusLost = [this] {
        juce::String displaceString = juce::String::formatted("%.2f", this->audioProcessor.displacement.asBeats());
        displace.setText(displaceString);
    };
    startBar.onFocusLost = [this] {
        juce::String startBarString = juce::String::formatted("%.2f", this->audioProcessor.startingBar);
        startBar.setText(startBarString);
    };
    stopBar.onFocusLost = [this] {
        juce::String stopBarString = juce::String::formatted("%.2f", this->audioProcessor.stoppingBar);
        stopBar.setText(stopBarString);
    };
    
    addAndMakeVisible (&displace);
    addAndMakeVisible (&startBar);
    addAndMakeVisible (&stopBar);
    juce::String displaceString = juce::String::formatted("%.2f", audioProcessor.displacement.asBeats());
    juce::String startBarString = juce::String::formatted("%.2f", audioProcessor.startingBar);
    juce::String stopBarString = juce::String::formatted("%.2f", audioProcessor.stoppingBar);
    displace.setText(displaceString);
    startBar.setText(startBarString);
    stopBar.setText(stopBarString);

    regenerateRolls.setClickingTogglesState(true);
    regenerateRolls.setToggleState(this->audioProcessor.improviseRolls, juce::dontSendNotification);
    regenerateOrnaments.setClickingTogglesState(true);
    regenerateOrnaments.setToggleState(this->audioProcessor.improviseOrnaments, juce::dontSendNotification);
    regenerateRolls.onClick = [this]() {
        vector<const string> rollKeys = getRollKeys();
        bool improviseRolls = regenerateRolls.getToggleState();
        improviseRolls
            ? audioProcessor.loopTasks.activate(rollKeys)
            : audioProcessor.loopTasks.deactivate(rollKeys);
    };
    regenerateOrnaments.onClick = [this]() {
        vector<const string> ornamentKeys = getOrnamentKeys();
        bool improviseOrnaments = regenerateOrnaments.getToggleState();
        improviseOrnaments
            ? audioProcessor.loopTasks.activate(ornamentKeys)
            : audioProcessor.loopTasks.deactivate(ornamentKeys);
        
        this->audioProcessor.improviseOrnaments = regenerateOrnaments.getToggleState();
    };
    addAndMakeVisible (&regenerateRolls);
    addAndMakeVisible (&regenerateOrnaments);
    
//    clearCascaraButton.onClick = [this]() { generator.toggleMutePlayable(cascaraKey); };
//    addAndMakeVisible(&clearCascaraButton);
    cascaraControls.muteButton.onClick = [this]() { generator.toggleMutePlayable(cascaraKey); };
    addAndMakeVisible(&cascaraControls.muteButton);
    
    claveControls.muteButton.onClick = [this]() { generator.toggleMutePlayable(claveKey); };
    addAndMakeVisible(&claveControls.muteButton);
    clearRollsButton.onClick = [this]() {
        string rollsKey = generator.rollsKey(selectedPhraseKeyState);
        generator.toggleMutePlayable(rollsKey);
    };
    
    addAndMakeVisible(&clearRollsButton);
    clearOrnamentsButton.onClick = [this]() {
        string ornamentsKey = generator.ornamentsKey(selectedPhraseKeyState);
        generator.toggleMutePlayable(ornamentsKey);
    };
    addAndMakeVisible(&clearOrnamentsButton);

    auto updateSelectedPhraseState = [this]() {
//        selectCascaraButton.getToggleState() ?
//        this->selectedPhraseKeyState = this->cascaraKey :
//        this->selectedPhraseKeyState = this->claveKey;
        cascaraControls.selectButton.getToggleState() ?
        this->selectedPhraseKeyState = cascaraControls.voiceName :
        this->selectedPhraseKeyState = this->claveKey;
    };
    auto testOnClick = [] { return; };
//    auto testOnClick = [&] { audioProcessor.queuePlayable(audioProcessor.generator.claveFromCascara()); };
    int selectRhythmGroupId = 98374; // random
//    selectCascaraButton.setRadioGroupId(selectRhythmGroupId);
//    selectCascaraButton.setClickingTogglesState(true);
//    selectCascaraButton.setToggleState(false, juce::dontSendNotification);
//    selectCascaraButton.onClick = updateSelectedPhraseState;
//    addAndMakeVisible (&selectCascaraButton);
    cascaraControls.selectButton.setRadioGroupId(selectRhythmGroupId);
    cascaraControls.selectButton.setClickingTogglesState(true);
    cascaraControls.selectButton.setToggleState(false, juce::dontSendNotification);
    cascaraControls.selectButton.onClick = updateSelectedPhraseState;
    addAndMakeVisible (&cascaraControls.selectButton);
    
    claveControls.selectButton.setRadioGroupId(selectRhythmGroupId);
    claveControls.selectButton.setClickingTogglesState(true);
    claveControls.selectButton.setToggleState(true, juce::dontSendNotification);
    claveControls.selectButton.onClick = updateSelectedPhraseState;
    addAndMakeVisible(&claveControls.selectButton);
    
    updateSelectedPhraseState();
    
    addAndMakeVisible(&rollProbability);
    rollProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    rollProbability.setRange (0.0, 1.0, 0.01);
    rollProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollProbability.setPopupDisplayEnabled (true, false, this);
    rollProbability.setTextValueSuffix (" p(roll)");
    rollProbability.setValue(1.0);
    
    addAndMakeVisible(&rollAssociation);
    rollAssociation.setSliderStyle (juce::Slider::LinearBarVertical);
    rollAssociation.setRange (0.0, 1.0, 0.01);
    rollAssociation.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollAssociation.setPopupDisplayEnabled (true, false, this);
    rollAssociation.setTextValueSuffix (" roll association / swing");
    rollAssociation.setValue(0.5);
    
    rollLength.onValueChange = testOnClick;
    addAndMakeVisible(&rollLength);
    rollLength.setSliderStyle (juce::Slider::LinearBarVertical);
    rollLength.setRange (0.0, 1.0, 0.01);
    rollLength.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollLength.setPopupDisplayEnabled (true, false, this);
    rollLength.setTextValueSuffix (" roll length");
    rollLength.setValue(0.5);
    
    flamButton.setClickingTogglesState(true);
    flamButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&flamButton);
    dragButton.setClickingTogglesState(true);
    dragButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&dragButton);
    ruffButton.setClickingTogglesState(true);
    ruffButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&ruffButton);
    addOrnamentsButton.onClick = [this]() {
        Probability prob = ornamentProbability.getValue();
        double breadth = ornamentBreadth.getValue();
        bool flams = flamButton.getToggleState();
        bool drags = dragButton.getToggleState();
        bool ruffs = ruffButton.getToggleState();
        
        generator.ornament(selectedPhraseKeyState, prob, breadth, flams, drags, ruffs);
        
        string id = generator.ornamentsKey(selectedPhraseKeyState);
        function<void()> task = [=]() { generator.ornament(selectedPhraseKeyState, prob, breadth, flams, drags, ruffs); };
        audioProcessor.loopTasks.queue(id, task, regenerateOrnaments.getToggleState());
    };
    addAndMakeVisible(&addOrnamentsButton);

    addAndMakeVisible(&ornamentProbability);
    ornamentProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentProbability.setRange (0.0, 1.0, 0.01);
    ornamentProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentProbability.setPopupDisplayEnabled (true, false, this);
    ornamentProbability.setTextValueSuffix (" ornament probability");
    ornamentProbability.setValue(0.3);
    
    addAndMakeVisible(&ornamentBreadth);
    ornamentBreadth.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentBreadth.setRange (0.0, 4.0, 0.01);
    ornamentBreadth.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentBreadth.setPopupDisplayEnabled (true, false, this);
    ornamentBreadth.setTextValueSuffix (" ornament breadth");
    ornamentBreadth.setValue(1.0);
    
    flipButton.onClick = [this]() {
        generator.flipClave(selectedPhraseKeyState);
        string ornamentsKey = generator.ornamentsKey(selectedPhraseKeyState);
        if (generator.hasPhrase(ornamentsKey)) { generator.flipClave(ornamentsKey); }
        string rollsKey = generator.rollsKey(selectedPhraseKeyState);
        if (generator.hasPhrase(rollsKey)) { generator.flipClave(rollsKey); }
    };
    addAndMakeVisible(&flipButton);
}

void GenerateStuffAudioProcessorEditor::updatePhraseLengthState() {
    float bars = -1;
    float beats = -1;
    try {
        auto barsString = phraseLengthBars.getTextValue().toString().toStdString();
        bars = stof(barsString);
    } catch (const invalid_argument& ia) {
        return;
    }
    try {
        auto beatsString = phraseLengthBeats.getTextValue().toString().toStdString();
        beats = stof(beatsString);
    } catch (const invalid_argument& ia) {
        return;
    }

    if (!audioProcessor.generator.setPhraseLength(bars, beats)) {
        // TODO: keep editor's value in sync with generator's
    };
}

void GenerateStuffAudioProcessorEditor::updateDisplacementState() {
    Beats displacement;
    try {
        displacement = stof(displace.getText().toStdString());
    } catch (const invalid_argument& ia) {
        return;
    }
    audioProcessor.setDisplacement(displacement);
    return;
}

void GenerateStuffAudioProcessorEditor::updateStartingBarState() {
    bars startingBar;
    try {
        string startBarText = startBar.getText().toStdString();
        startingBar = stof(startBarText);
    } catch (const invalid_argument& ia) {
        return;
    }
    audioProcessor.setStartBar(startingBar);
    return;
}

void GenerateStuffAudioProcessorEditor::updateStoppingBarState() {
    bars stoppingBar;
    try {
        string stopBarText = stopBar.getText().toStdString();
        stoppingBar = stof(stopBarText);
    } catch (const invalid_argument& ia) {
        return;
    }
    audioProcessor.setStopBar(stoppingBar);
    return;
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
    int width = getWidth() - 2 * xPadding;
    int spaceBetweenControls = 10;
    int sliderWidth = 20;
    int numSliders = 6;
    int buttonColumns = 12;
    int buttonWidth = (width - numSliders * sliderWidth - spaceBetweenControls * (numSliders + buttonColumns)) / buttonColumns;
    
    int xCursor = xPadding;
    int yCursor = yPadding;
    
    short spaceBetweenSubDivButtons = 4;
    int totalSpaceBetweenSubDivButtons = spaceBetweenSubDivButtons * ((int) subdivisionButtons.size() - 1);
    int subDivButtonHeight = (height - totalSpaceBetweenSubDivButtons) / (subdivisionButtons.size());
    
    int numberInputWidth = buttonWidth;
    short spaceBetween1stRowElements = 4;
    int firstColumnRows = 7;
    int totalSpaceBetweenElements1stRow = spaceBetween1stRowElements * (firstColumnRows - 1);
    int firstColumnElementHeight = (height - totalSpaceBetweenElements1stRow) / firstColumnRows;
    
    
    phraseLengthBarsLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    phraseLengthBeatsLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    displaceLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    startBarLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    stopBarLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    regenerateRolls.setBounds (xCursor + spaceBetweenControls, yCursor, numberInputWidth * 2, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    regenerateOrnaments.setBounds (xCursor + spaceBetweenControls, yCursor, numberInputWidth * 2, firstColumnElementHeight);
    xCursor += numberInputWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    phraseLengthBars.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    phraseLengthBeats.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    
    displace.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    startBar.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    stopBar.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    xCursor += numberInputWidth + spaceBetweenControls;
    yCursor = yPadding;
    
//    buttonHeight = (height - totalSpaceBetweenSubDivButtons) / subdivisionButtons.size();
    for (auto i = 0; i < subdivisionButtons.size(); i++) {
        subdivisionButtons[i]->setBounds (xCursor, yCursor, buttonWidth, subDivButtonHeight);
        yCursor += subDivButtonHeight + spaceBetweenSubDivButtons;
    }
    
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    probabilityOfDouble.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    
    auto getButtonHeight = [height, spaceBetweenControls] (int rows) { return (height - spaceBetweenControls * (rows - 1)) / rows; };
    int buttonHeight = getButtonHeight(2);
    
    cascaraControls.setBounds(xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);
    yCursor += buttonHeight + spaceBetweenControls;
    claveControls.setBounds(xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);

    xCursor += (buttonWidth + spaceBetweenControls) * 5; // TODO: better way of doing width of voice controls.
    yCursor = yPadding;
    
    decorationDividerX = xCursor;
    xCursor += spaceBetweenControls;

    flipButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(1));
    xCursor += buttonWidth + spaceBetweenControls;
    
    rollProbability.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    rollAssociation.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    rollLength.setBounds (xCursor, yCursor, sliderWidth, height);
    xCursor += sliderWidth + spaceBetweenControls;
    
    addRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + spaceBetweenControls;
    clearRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
    
    
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
    
    addOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(2));
    yCursor += getButtonHeight(2) + spaceBetweenControls;
    clearOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(2));
    xCursor += buttonWidth + spaceBetweenControls;
    yCursor = yPadding;
}

void GenerateStuffAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.probabilityOfDouble = probabilityOfDouble.getValue();
}

void GenerateStuffAudioProcessorEditor::buttonClicked (juce::Button *button)
{
    auto buttonName = button->getName();
    auto randomCascaraName = cascaraControls.generateButton.getName();
    auto claveFromCascaraName = claveControls.generateFromButton.getName();
    auto randomClaveName = claveControls.generateButton.getName();
    auto cascaraFromClaveName = cascaraControls.generateFromButton.getName();

    
    auto isCascaraButton = buttonName.equalsIgnoreCase(randomCascaraName);
    auto isClaveButton = buttonName.equalsIgnoreCase(randomClaveName);
    auto isClaveFromCascaraButton = buttonName.equalsIgnoreCase(claveFromCascaraName);
    auto isCascaraFromClaveButton = buttonName.equalsIgnoreCase(cascaraFromClaveName);
    
    if (isCascaraButton) {
        generator.cascara();
        generator.queuePlayable(cascaraKey, generator.cascara());
    } else if (isClaveButton) {
        generator.queuePlayable(claveKey, generator.clave());
    } else if (isClaveFromCascaraButton) {
        generator.queuePlayable(claveKey, generator.claveFromCascara());
    } else if (isCascaraFromClaveButton) {
        generator.queuePlayable(cascaraKey, generator.cascaraFromClave());
    }
}

