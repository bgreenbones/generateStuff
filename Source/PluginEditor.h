/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomUIs.h"


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
    Generator& generator;
    
    juce::Slider probabilityOfDouble;
//    juce::TextButton randomCascaraButton { "casc "};
//    juce::TextButton randomClaveButton { "clav" };
//    juce::TextButton cascaraFromClaveButton { "clv->cas" };
//    juce::TextButton claveFromCascaraButton { "cas->clv" };
//    juce::TextButton clearCascaraButton { "clear casc" };
//    juce::TextButton clearClaveButton { "clear clave" };
//    juce::TextButton selectCascaraButton { "casc" };
//    juce::TextButton selectClaveButton { "clav" };
    juce::TextButton addRollsButton { "rolls" };
    juce::TextButton clearRollsButton { "clearRolls" };
    juce::Slider rollProbability;
    juce::Slider rollAssociation;
    juce::Slider rollLength;
    juce::TextButton addOrnamentsButton { "orns" };
    juce::TextButton clearOrnamentsButton { "clear orns" };
    juce::TextButton flamButton { "flam" };
    juce::TextButton dragButton { "drag" };
    juce::TextButton ruffButton { "ruff" };
    juce::Slider ornamentProbability;
    juce::Slider ornamentBreadth;
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
    // 1st column
    juce::Label phraseLengthBarsLabel;
    juce::Label phraseLengthBeatsLabel;
    juce::TextEditor phraseLengthBars;
    juce::TextEditor phraseLengthBeats;
    juce::Label displaceLabel; // displace beats
    juce::TextEditor displace;
    juce::Label startBarLabel; // start bar
    juce::TextEditor startBar;
    juce::Label stopBarLabel; // stop bar / just go forever? / loop going forward in timeline?
    juce::TextEditor stopBar;
    juce::TextButton regenerateRolls { "regen rolls" }; // re-gen rolls on loop
    juce::TextButton regenerateOrnaments { "regen orns" }; // re-gen ornaments on loop
    juce::TextButton flipButton { "flip" };

    void updateSubdivisionState(float subdivision);
    void updatePhraseLengthState();
    void updateDisplacementState();
    void updateStartingBarState();
    void updateStoppingBarState();

    int xPadding = 20;
    int yPadding = 20;
    int decorationDividerX = -1;
    
    
    // TODO: all this stuff we repeat for each phrase type...how can we abstract it or separate it out to some other phrase type manager thing.
    const string cascaraKey = "cascara";
    const string claveKey = "clave";
    VoiceControls cascaraControls { cascaraKey };
    VoiceControls claveControls { claveKey };
    const vector<const string> phraseKeys = { cascaraKey, claveKey };
    vector<const string> getRollKeys() {
        vector<const string> rollKeys;
        transform (phraseKeys.begin(), phraseKeys.end(), back_inserter(rollKeys), [&](string phraseKey) {
            return generator.rollsKey(phraseKey);
        });
        return rollKeys;
    }
    vector<const string> getOrnamentKeys() {
        vector<const string> ornamentKeys;
        transform (phraseKeys.begin(), phraseKeys.end(), back_inserter(ornamentKeys), [&](string phraseKey) {
            return generator.ornamentsKey(phraseKey);
        });
        return ornamentKeys;
    }
    string selectedPhraseKeyState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenerateStuffAudioProcessorEditor)
};
