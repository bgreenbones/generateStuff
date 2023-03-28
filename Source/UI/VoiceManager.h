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
#include "Generator.hpp"

using std::string, std::vector, std::map;

class VoiceManager {
private:
    map<string, VoiceControls> voices;
public:
    GenerateStuffAudioProcessor& processor;    
    Generator& generator;
    shared_ptr<PlayQueue> playQueue;
    Component* mainEditor;
    
//    string selectedPhraseKeyState;
    string useAsSourcePhraseKeyState;
    
    VoiceManager(GenerateStuffAudioProcessor& processor);
    ~VoiceManager();

    void midiChannelChanged(string voiceName);
    void setOnClicks();
    void callAddAndMakeVisible(Component *editor);
    void configureButtons();
    void configure(Component *editor);
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls);
    size_t getNumberOfButtons();
    size_t getNumberOfColumns();
    size_t getNumberOfVoices();
//    void updateSelectedPhraseState();
    void updateUseAsSourceState();
    void updateState();
    vector<const string> getRollKeys();
    vector<const string> getOrnamentKeys();

};





