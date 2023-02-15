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
//#include <JuceHeader.h>
#include "VoiceControls.h"

using std::string, std::vector;

class VoiceManager {
private:
    map<string, VoiceControls*> voices;
public:
    GenerateStuffAudioProcessor& processor;
    
    Generator& generator;
    shared_ptr<PlayQueue> playQueue;
    VoiceControls cascaraControls; // TODO: these things existences should come from the playqueue
    VoiceControls claveControls;
//    VoiceControls harmonyControls;
//    VoiceControls bassControls;
    
    VoiceManager(GenerateStuffAudioProcessor& processor):
        processor(processor),
        generator(processor.generator),
        playQueue(processor.playQueue),
        cascaraControls(cascaraKey, cascaraChannel),
        claveControls(claveKey, claveChannel)
//        harmonyControls(harmonyKey),
//        bassControls(bassKey)
    {
        voices.emplace(cascaraKey, &cascaraControls);
        voices.emplace(claveKey, &claveControls);
//        voices.emplace(harmonyKey, &harmonyControls);
//        voices.emplace(bassKey, &bassControls);
    }
    
    ~VoiceManager()
    {
        voices.clear();
    }
    
    
    void midiChannelChanged(string voiceName) {
        int midiChannel = voices.at(voiceName)->midiChannel.getSelectedId();
        playQueue->setMidiChannel(voiceName, midiChannel);
    }
    
    void setOnClicks() {
        for (auto voicePair : voices) {
            VoiceControls *voice = voicePair.second;
            string voiceName = voice->voiceName;
            voice->midiChannel.onChange = [this, voiceName]() { midiChannelChanged(voiceName); };
            voice->generateButton.onClick = [this, voiceName]() { generator.generate(voiceName); };
            voice->generateFromButton.onClick = [this, voiceName]() { generator.generateFrom(voiceName, useAsSourcePhraseKeyState); };
            voice->muteButton.onClick = [voice, this, voiceName]() {
                bool muted = playQueue->toggleMuteVoice(voiceName);
                voice->muteButton.setToggleState(muted, juce::dontSendNotification);
            };
            voice->selectButton.onClick = [this]() { updateSelectedPhraseState(); };
            voice->useAsSourceButton.onClick = [this]() { updateUseAsSourceState(); };
            voice->improviseButton.onClick = [voice, this, voiceName]() {
                bool improvise = !(voice->improviseButton.getToggleState());
                if (improvise) {
                    processor.loopTasks.queue(voiceName, [](){}); // do this actually.
                } else {
                    processor.loopTasks.deactivate({ voiceName });
                }
                voice->improviseButton.setToggleState(improvise, juce::dontSendNotification);
            };
        }
    }
    
    void callAddAndMakeVisible(Component *editor) {
        for (auto voicePair : voices) {
            voicePair.second->callAddAndMakeVisible(editor);
        }
    }
    
    void configureButtons() {
        for (auto voicePair : voices) {
            voicePair.second->configureButtons();
        }
    }
    
    void configure(Component *editor) {
        configureButtons();
        callAddAndMakeVisible(editor);
        setOnClicks();
    }
    
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
        auto incrementCursor = [&]() { yCursor += buttonHeight + spaceBetweenControls; };
        for (auto voicePair : voices) {
            voicePair.second->setBounds (xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);
            incrementCursor();
        }
    }
    
    size_t getNumberOfButtons() {
        return cascaraControls.getNumberOfButtons();
    }
    
    size_t getNumberOfColumns() {
        return cascaraControls.getNumberOfColumns();
    }
    
    size_t getNumberOfVoices() {
        return voices.size();
    }
    
    
    string selectedPhraseKeyState;
    void updateSelectedPhraseState() {
        cascaraControls.selectButton.getToggleState() ?
        this->selectedPhraseKeyState = cascaraControls.voiceName :
        this->selectedPhraseKeyState = claveKey;
    };
    string useAsSourcePhraseKeyState;
    void updateUseAsSourceState() {
        cascaraControls.useAsSourceButton.getToggleState() ?
        this->useAsSourcePhraseKeyState = cascaraControls.voiceName :
        this->useAsSourcePhraseKeyState = claveKey;
    }
    
    void updateState() {
        updateSelectedPhraseState();
        updateUseAsSourceState();
    }
    
    
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

};





