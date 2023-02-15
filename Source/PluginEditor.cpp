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
    : AudioProcessorEditor (&p),
    audioProcessor (p),
    generator(p.generator),
    editorState(p.editorState),
    voiceManager(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 500);
    voiceManager.configure(this);

    
    
//    probabilityOfDouble.setSliderStyle (juce::Slider::LinearBarVertical);
//    probabilityOfDouble.setRange (0.0, 1.0, 0.01);
//    probabilityOfDouble.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
//    probabilityOfDouble.setPopupDisplayEnabled (true, false, this);
//    probabilityOfDouble.setTextValueSuffix (" p(double)");
//    probabilityOfDouble.setValue(0.2);
//    addAndMakeVisible (&probabilityOfDouble);
//
    
    addRollsButton.onClick = [this]() {
        double rollProb = Probability(rollProbability.getValue());
        double associationProb = Probability(rollAssociation.getValue());
        double rollLengthProb = Probability(rollLength.getValue());

        generator.roll(voiceManager.selectedPhraseKeyState, rollProb, associationProb, rollLengthProb);
        string id = generator.rollsKey(voiceManager.selectedPhraseKeyState);
        function<void()> task = [=]() { generator.roll(voiceManager.selectedPhraseKeyState, rollProb, associationProb, rollLengthProb); };
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
//        subdivisionButton->onClick = [=] { updateSubdivisionState (1.f / subdivisionDenominator); };
    }
    int defaultSubdivisionIndex = (int) (1.0 / editorState->subdivision) - 1; // todo: set this
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
    phraseLengthBars.onTextChange = [this] { updateEditorState(); };
    phraseLengthBeats.onTextChange = [this] { updateEditorState(); };
    juce::String barsString = juce::String::formatted("%.2f", editorState->phraseLengthBars);
    juce::String beatsString = juce::String::formatted("%.2f", editorState->phraseLengthBeats);
    phraseLengthBars.setText(barsString);
    phraseLengthBeats.setText(beatsString);
    phraseLengthBars.onFocusLost = [this] {
        juce::String phraseLengthString = juce::String::formatted("%.2f", editorState->phraseLengthBars);
        phraseLengthBars.setText(phraseLengthString);
    };
    phraseLengthBeats.onFocusLost = [this] {
        juce::String phraseLengthString = juce::String::formatted("%.2f", editorState->phraseLengthBeats);
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
//    displace.onTextChange = [this] { updateDisplacementState(); };
//    startBar.onTextChange = [this] { updateStartingBarState(); };
//    stopBar.onTextChange = [this] { updateStoppingBarState(); };
    displace.onTextChange = [this] { updateEditorState(); };
    startBar.onTextChange = [this] { updateEditorState(); };
    stopBar.onTextChange = [this] { updateEditorState(); };
    displace.onFocusLost = [this] {
        juce::String displaceString = juce::String::formatted("%.2f", editorState->displace);
        displace.setText(displaceString); // TODO: do this kind of stuff after every call to updateEditorState instead of on focus lost?
    };
    startBar.onFocusLost = [this] {
        juce::String startBarString = juce::String::formatted("%.2f", editorState->startBar);
        startBar.setText(startBarString);
    };
    stopBar.onFocusLost = [this] {
        juce::String stopBarString = juce::String::formatted("%.2f", editorState->stopBar);
        stopBar.setText(stopBarString);
    };
    
    addAndMakeVisible (&displace);
    addAndMakeVisible (&startBar);
    addAndMakeVisible (&stopBar);
    juce::String displaceString = juce::String::formatted("%.2f", editorState->displace);
    juce::String startBarString = juce::String::formatted("%.2f", editorState->startBar);
    juce::String stopBarString = juce::String::formatted("%.2f", editorState->stopBar);
    displace.setText(displaceString);
    startBar.setText(startBarString);
    stopBar.setText(stopBarString);

    regenerateRolls.setClickingTogglesState(true);
    regenerateRolls.setToggleState(editorState->improviseRolls, juce::dontSendNotification);
    regenerateOrnaments.setClickingTogglesState(true);
    regenerateOrnaments.setToggleState(editorState->improviseOrnaments, juce::dontSendNotification);
    regenerateRolls.onClick = [this]() {
        vector<const string> rollKeys = voiceManager.getRollKeys();
        bool improviseRolls = regenerateRolls.getToggleState();
        improviseRolls
            ? audioProcessor.loopTasks.activate(rollKeys)
            : audioProcessor.loopTasks.deactivate(rollKeys);
    };
    regenerateOrnaments.onClick = [this]() {
        vector<const string> ornamentKeys = voiceManager.getOrnamentKeys();
        bool improviseOrnaments = regenerateOrnaments.getToggleState();
        improviseOrnaments
            ? audioProcessor.loopTasks.activate(ornamentKeys)
            : audioProcessor.loopTasks.deactivate(ornamentKeys);
        
        editorState->improviseOrnaments = regenerateOrnaments.getToggleState();
    };
    addAndMakeVisible (&regenerateRolls);
    addAndMakeVisible (&regenerateOrnaments);
    
    clearRollsButton.onClick = [this]() {
        string rollsKey = generator.rollsKey(voiceManager.selectedPhraseKeyState);
        generator.toggleMutePlayable(rollsKey);
    };
    
    addAndMakeVisible(&clearRollsButton);
    clearOrnamentsButton.onClick = [this]() {
        string ornamentsKey = generator.ornamentsKey(voiceManager.selectedPhraseKeyState);
        generator.toggleMutePlayable(ornamentsKey);
    };
    addAndMakeVisible(&clearOrnamentsButton);

    voiceManager.updateState();
        
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
        
        generator.ornament(voiceManager.selectedPhraseKeyState, prob, breadth, flams, drags, ruffs);
        
        string id = generator.ornamentsKey(voiceManager.selectedPhraseKeyState);
        function<void()> task = [=]() { generator.ornament(voiceManager.selectedPhraseKeyState, prob, breadth, flams, drags, ruffs); };
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
        generator.flipClave(voiceManager.selectedPhraseKeyState);
        string ornamentsKey = generator.ornamentsKey(voiceManager.selectedPhraseKeyState);
        if (generator.hasPhrase(ornamentsKey)) { generator.flipClave(ornamentsKey); }
        string rollsKey = generator.rollsKey(voiceManager.selectedPhraseKeyState);
        if (generator.hasPhrase(rollsKey)) { generator.flipClave(rollsKey); }
    };
    addAndMakeVisible(&flipButton);
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
    int buttonColumns = voiceManager.getNumberOfColumns() + 7;
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
    
//    probabilityOfDouble.setBounds (xCursor, yCursor, sliderWidth, height);
//    xCursor += sliderWidth + spaceBetweenControls;
    
    auto getButtonHeight = [height, spaceBetweenControls] (int rows) { return (height - spaceBetweenControls * (rows - 1)) / rows; };
    int buttonHeight = getButtonHeight(voiceManager.getNumberOfVoices());

    voiceManager.setBounds(xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);
    xCursor += (buttonWidth + spaceBetweenControls) * voiceManager.getNumberOfColumns();
    
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

void GenerateStuffAudioProcessorEditor::updateEditorState() {
    double subDiv = 1./2.;
    for (float subdivisionDenominator = 1; subdivisionDenominator <= 9; subdivisionDenominator++) {
        int subdivisionIndex = subdivisionDenominator - 1;
        auto subdivisionButton = subdivisionButtons[subdivisionIndex];
        if (subdivisionButton->getToggleState()) {
            subDiv = 1./subdivisionDenominator;
            break;
        }
    }
    
    double bars, beats, dis, start, stop;
    try { bars = stof(phraseLengthBars.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    try { beats = stof(phraseLengthBeats.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    try { dis = stof(displace.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    try { start = stof(startBar.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    try { stop = stof(stopBar.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    
    start = max(start, 1.);
    stop = max(start, stop);
    
    
    // general
    editorState->subdivision = subDiv;
    editorState->phraseLengthBars = bars;
    editorState->phraseLengthBeats = beats;
    editorState->displace = dis;
    editorState->startBar = start;
    editorState->stopBar = stop;
//        double probabilityOfDouble; // not yet using for 'cascara' abstraction

    // rolls / runs
    editorState->muteRolls = clearRollsButton.getToggleState();
    editorState->rollProbability = rollProbability.getValue();
    editorState->rollAssociation = rollAssociation.getValue();
    editorState->rollLength = rollLength.getValue();
    editorState->improviseRolls = regenerateRolls.getToggleState();
    editorState->improviseOrnaments = regenerateOrnaments.getToggleState();

    // ornaments
    editorState->muteOrnaments = clearOrnamentsButton.getToggleState();
    editorState->allowflams = flamButton.getToggleState();
    editorState->allowDrags = dragButton.getToggleState();
    editorState->allowRuffs = ruffButton.getToggleState();
    editorState->ornamentProbability = ornamentProbability.getValue();
    editorState->ornamentBreadth = ornamentBreadth.getValue();

}

