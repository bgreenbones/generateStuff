/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CustomUIs.h"
#include <string>
#include <stdexcept>

using namespace std;


//==============================================================================
GenerateStuffAudioProcessorEditor::GenerateStuffAudioProcessorEditor (GenerateStuffAudioProcessor& p)
    : AudioProcessorEditor (&p),
    audioProcessor (p),
    ensemble (p.ensemble),
    editorState(p.editorState),
    voiceManager(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (ui::editorWidth, ui::editorHeight);
    voiceManager.configure(this);

    addAndMakeVisible(generateButton);
    generateButton.onClick = [&] { ensemble.writeSong(); };


    int subdivisionGroupId = 1832; // just some number
    for (float subdivisionDenominator = 1; subdivisionDenominator <= 9; subdivisionDenominator++) {
        int subdivisionIndex = subdivisionDenominator - 1;
        auto subdivisionButton = subdivisionButtons[subdivisionIndex];
        subdivisionButton->setRadioGroupId(subdivisionGroupId);
        subdivisionButton->setClickingTogglesState(true);
        addAndMakeVisible(subdivisionButton);
        subdivisionButton->setToggleState(false, juce::dontSendNotification);
        subdivisionButton->onClick = [subdivisionDenominator, this] { editorState.subdivision = 1. / subdivisionDenominator; };
    }
    int defaultSubdivisionIndex = (int) (1.0 / editorState.subdivision) - 1; // todo: set this
    subdivisionButtons[defaultSubdivisionIndex]->setToggleState(true, juce::dontSendNotification);
    
    subdivisionLabel.setText("subdivision", juce::dontSendNotification);
    addAndMakeVisible(&subdivisionLabel);
    phraseLengthBarsLabel.setText("phrase length (bars)", juce::dontSendNotification);
    phraseLengthBarsLabel.attachToComponent(&phraseLengthBars, true);
    phraseLengthBars.setJustification (juce::Justification::centred);
    phraseLengthBars.setInputRestrictions (4, juce::String {".1234567890"});
    addAndMakeVisible (&phraseLengthBars);
    phraseLengthBars.onTextChange = [this] { updateEditorState(); };
    juce::String barsString = juce::String::formatted("%.2f", editorState.phraseLengthBars);
    phraseLengthBars.setText(barsString);
    phraseLengthBars.onFocusLost = [this] {
        juce::String phraseLengthString = juce::String::formatted("%.2f", editorState.phraseLengthBars);
        phraseLengthBars.setText(phraseLengthString);
    };
    
    startBarLabel.setText("start (bar)", juce::dontSendNotification);
    startBarLabel.attachToComponent(&startBar, true);
    
    startBar.setJustification (juce::Justification::centred);
    startBar.setInputRestrictions (4, juce::String {".1234567890"});
    startBar.onTextChange = [this] { updateEditorState(); };
    startBar.onFocusLost = [this] {
        juce::String startBarString = juce::String::formatted("%.2f", editorState.startBar);
        startBar.setText(startBarString);
    };
    
    addAndMakeVisible (&startBar);
    juce::String startBarString = juce::String::formatted("%.2f", editorState.startBar);
    startBar.setText(startBarString);

    
    voiceManager.updateState();
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

    int numSliders = 0;
    // int numSliders = 6;
    int globalColumns = 3;
    int voiceColumns = voiceManager.getNumberOfColumns();
    int columns = globalColumns + voiceColumns;
    // int buttonWidth = (width - numSliders * ui::sliderWidth - ui::spaceBetweenControls * (numSliders + buttonColumns)) / buttonColumns;
    int buttonWidth = (width - ui::spaceBetweenControls * columns) / columns;
    
    int xCursor = xPadding;
    int yCursor = yPadding;
    
    short spaceBetweenSubDivButtons = 4;
    int totalSpaceBetweenSubDivButtons = spaceBetweenSubDivButtons * ((int) subdivisionButtons.size() - 1);
    int subDivButtonHeight = (height - totalSpaceBetweenSubDivButtons) / (subdivisionButtons.size() + 1);
    
    int numberInputWidth = buttonWidth;
    short spaceBetween1stRowElements = 4;
    int firstColumnRows = 9;
    int totalSpaceBetweenElements1stRow = spaceBetween1stRowElements * (firstColumnRows - 1);
    int firstColumnElementHeight = (height - totalSpaceBetweenElements1stRow) / firstColumnRows;
    int generateButtonHeight = height - (firstColumnElementHeight + spaceBetween1stRowElements) * 2;
    
    phraseLengthBarsLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    startBarLabel.setBounds (xCursor - numberInputWidth, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    
    generateButton.setBounds(xCursor, yCursor, numberInputWidth * 2, generateButtonHeight);
    xCursor += numberInputWidth + ui::spaceBetweenControls;
    yCursor = yPadding;
    
    phraseLengthBars.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    yCursor += firstColumnElementHeight + spaceBetween1stRowElements;
    
    startBar.setBounds (xCursor, yCursor, numberInputWidth, firstColumnElementHeight);
    
    xCursor += numberInputWidth + ui::spaceBetweenControls;
    yCursor = yPadding;
    
//    buttonHeight = (height - totalSpaceBetweenSubDivButtons) / subdivisionButtons.size();
    for (auto i = 0; i < subdivisionButtons.size(); i++) {
        subdivisionButtons[i]->setBounds (xCursor, yCursor, buttonWidth, subDivButtonHeight);
        yCursor += subDivButtonHeight + spaceBetweenSubDivButtons;
    }    
    subdivisionLabel.setBounds(xCursor, yCursor, buttonWidth, subDivButtonHeight);
    
    xCursor += buttonWidth + ui::spaceBetweenControls;
    yCursor = yPadding;
    
//    probabilityOfDouble.setBounds (xCursor, yCursor, sliderWidth, height);
//    xCursor += sliderWidth + spaceBetweenControls;
    
    int buttonHeight = ui::getButtonHeight(voiceManager.getNumberOfVoices(), height);
    int voiceTileHeight = ui::getVoiceTileHeight(height);

    voiceManager.setBounds(xCursor, yCursor, buttonWidth, voiceTileHeight, ui::spaceBetweenControls);
    xCursor += (buttonWidth + ui::spaceBetweenControls) * voiceManager.getNumberOfColumns();
    
//    decorationDividerX = xCursor;
//    xCursor += spaceBetweenControls;

    
    
    
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
    
//    double bars, beats, dis, start, stop;
    double bars, start;
    try { bars = stof(phraseLengthBars.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    try { start = stof(startBar.getTextValue().toString().toStdString()); } catch (const invalid_argument& ia) {}
    
    start = max(start, 1.);
//    stop = max(start, stop);
    
    
    // general
    editorState.subdivision = subDiv;
    editorState.phraseLengthBars = bars;
//    editorState.phraseLengthBeats = beats;
//    editorState.displace = dis;
    editorState.startBar = start;
//    editorState.stopBar = stop;
//        double probabilityOfDouble; // not yet using for 'cascara' abstraction
}

