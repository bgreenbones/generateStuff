/*
  ==============================================================================

    VoiceManager.h
    Created: 12 Feb 2023 10:25:48pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once



#include <string>
#include <vector>
#include "VoiceControls.h"
#include "PluginProcessor.h"

using std::string, std::vector, std::map;

class VoiceManager {
private:
    map<string, VoiceControls> voices;
public:
    GenerateStuffAudioProcessor& processor;    
    Ensemble& ensemble;
    juce::Component* mainEditor;
    juce::Label midiChannelLabel = juce::Label("midiChannelLabel", "MIDI Channel:");
    juce::Label highPitchLabel = juce::Label("highPitchLabel", "Highest Pitch:");
    juce::Label lowPitchLabel = juce::Label("lowPitchLabel", "Lowest Pitch:");
    juce::Label densityLabel = juce::Label("densityLabel", "Density:");
    vector<juce::Label*> labels = { &midiChannelLabel, &highPitchLabel, &lowPitchLabel, &densityLabel };
    
    // string useAsSourcePhraseKeyState;
    
    VoiceManager(GenerateStuffAudioProcessor& processor);
    ~VoiceManager();

    void midiChannelChanged(string voiceName);
    void setOnClicks();
    void callAddAndMakeVisible(juce::Component *editor);
    void configureButtons();
    void configure(juce::Component *editor);
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls);
    size_t getNumberOfButtons();
    size_t getNumberOfColumns();
    size_t getNumberOfVoices();
    void updateUseAsSourceState();
    void updateState();
    // vector<const string> getConnectingKeys();
    // vector<const string> getOrnamentKeys();

};





