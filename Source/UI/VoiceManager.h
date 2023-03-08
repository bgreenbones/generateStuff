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

using std::string, std::vector;

class VoiceManager {
private:
    map<string, VoiceControls> voices;
public:
    GenerateStuffAudioProcessor& processor;
    
    Generator& generator;
    shared_ptr<PlayQueue> playQueue;
    
    VoiceManager(GenerateStuffAudioProcessor& processor):
        processor(processor),
        generator(processor.generator),
        playQueue(processor.playQueue)
    {
        for (VoiceBindings vb : voiceBindings) {
            voices.emplace(vb.voiceName, VoiceControls(vb.voiceName, vb.midiChannel));
        }
    }
    
    ~VoiceManager()
    {
        voices.clear();
    }
    
    
    void midiChannelChanged(string voiceName) {
        int midiChannel = voices.at(voiceName).midiChannel.getSelectedId();
        playQueue->setMidiChannel(voiceName, midiChannel);
    }
    
    void setOnClicks() {
        for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
            VoiceControls &voice = voiceIt->second;
            string voiceName = voice.voiceName;
            voice.midiChannel.onChange = [this, voiceName]() { midiChannelChanged(voiceName); };
            voice.generateButton.onClick = [this, voiceName]() { generator.generate(voiceName); };
            voice.generateFromButton.onClick = [this, voiceName]() { generator.generateFrom(voiceName, useAsSourcePhraseKeyState); };
            voice.muteButton.onClick = [this, voiceName]() {
                bool muted = playQueue->toggleMuteVoice(voiceName);
                voices.at(voiceName).muteButton.setToggleState(muted, juce::dontSendNotification);
            };
            voice.selectButton.onClick = [this]() { updateSelectedPhraseState(); };
            voice.useAsSourceButton.onClick = [this]() { updateUseAsSourceState(); };
            voice.improviseButton.onClick = [this, voiceName]() {
                VoiceControls &voice = voices.at(voiceName);
                bool improvise = !(voice.improviseButton.getToggleState());
                if (improvise) {
                    processor.loopTasks.queue(voiceName, [](){}); // TODO: do this actually.
                } else {
                    processor.loopTasks.deactivate({ voiceName });
                }
                voice.improviseButton.setToggleState(improvise, juce::dontSendNotification);
            };
        }
    }
    
    void callAddAndMakeVisible(Component *editor) {
        for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
            voiceIt->second.callAddAndMakeVisible(editor);
        }
    }
    
    void configureButtons() {
        for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
            voiceIt->second.configureButtons();
        }
    }
    
    void configure(Component *editor) {
        configureButtons();
        callAddAndMakeVisible(editor);
        setOnClicks();
    }
    
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
        auto incrementCursor = [&]() { yCursor += buttonHeight + spaceBetweenControls; };
        for (auto voiceIt = voiceBindings.begin(); voiceIt != voiceBindings.end(); voiceIt++) {
            voices.at(voiceIt->voiceName).setBounds (xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);
            incrementCursor();
        }
    }
    
    size_t getNumberOfButtons() {
        return voices.begin()->second.getNumberOfButtons();
    }
    
    size_t getNumberOfColumns() {
        return voices.begin()->second.getNumberOfColumns();
    }
    
    size_t getNumberOfVoices() {
        return voices.size();
    }
    
    
    string selectedPhraseKeyState;
    void updateSelectedPhraseState() {
        for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
            bool voiceSelected = voiceIt->second.selectButton.getToggleState();
            if (voiceSelected) {
                this->selectedPhraseKeyState = voiceIt->second.voiceName;
                break;
            }
        }
    };
    string useAsSourcePhraseKeyState;
    void updateUseAsSourceState() {
        for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
            bool voiceSelected = voiceIt->second.useAsSourceButton.getToggleState();
            if (voiceSelected) {
                this->useAsSourcePhraseKeyState = voiceIt->second.voiceName;
                break;
            }
        }
    }
    
    void updateState() {
        updateSelectedPhraseState();
        updateUseAsSourceState();
    }
    
    vector<const string> getRollKeys() {
        vector<const string> rollKeys;
        transform (voices.begin(), voices.end(), back_inserter(rollKeys), [&](auto voicePair) {
            return generator.rollsKey(voicePair.first);
        });
        return rollKeys;
    }
    vector<const string> getOrnamentKeys() {
        vector<const string> ornamentKeys;
        transform (voices.begin(), voices.end(), back_inserter(ornamentKeys), [&](auto voicePair) {
            return generator.ornamentsKey(voicePair.first);
        });
        return ornamentKeys;
    }

};





