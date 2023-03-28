/*
  ==============================================================================

    VoiceManager.cpp
    Created: 21 Mar 2023 11:15:31pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceManager.h"
#include "CustomUIs.h"


VoiceManager::VoiceManager(GenerateStuffAudioProcessor& processor):
    processor(processor),
    generator(processor.generator),
    playQueue(processor.playQueue)
{
    for (VoiceBindings vb : voiceBindings) {
        voices.emplace(vb.voiceName, VoiceControls(vb.voiceName, vb.midiChannel));
    }
}

VoiceManager::~VoiceManager()
{
    voices.clear();
}


void VoiceManager::midiChannelChanged(string voiceName) {
    int midiChannel = voices.at(voiceName).midiChannel.getSelectedId();
    playQueue->setMidiChannel(voiceName, midiChannel);
}


void VoiceManager::callAddAndMakeVisible(Component *editor) {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        voiceIt->second.callAddAndMakeVisible(editor);
    }
}

void VoiceManager::configureButtons() {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        voiceIt->second.configureButtons();
    }
}

void VoiceManager::configure(Component *editor) {
    mainEditor = editor;
    configureButtons();
    callAddAndMakeVisible(editor);
    setOnClicks();
}

void VoiceManager::setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
    auto incrementCursor = [&]() { yCursor += buttonHeight + spaceBetweenControls; };
    for (auto voiceIt = voiceBindings.begin(); voiceIt != voiceBindings.end(); voiceIt++) {
        voices.at(voiceIt->voiceName).setBounds (xCursor, yCursor, buttonWidth, buttonHeight, spaceBetweenControls);
        incrementCursor();
    }
}

size_t VoiceManager::getNumberOfButtons() {
    return voices.begin()->second.getNumberOfButtons();
}

size_t VoiceManager::getNumberOfColumns() {
    return voices.begin()->second.getNumberOfColumns();
}

size_t VoiceManager::getNumberOfVoices() {
    return voices.size();
}

//void VoiceManager::updateSelectedPhraseState() {
//    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
//        bool voiceSelected = voiceIt->second.selectButton.getToggleState();
//        if (voiceSelected) {
//            this->selectedPhraseKeyState = voiceIt->second.voiceName;
//            break;
//        }
//    }
//};

void VoiceManager::updateUseAsSourceState() {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        bool voiceSelected = voiceIt->second.useAsSourceButton.getToggleState();
        if (voiceSelected) {
            this->useAsSourcePhraseKeyState = voiceIt->second.voiceName;
            break;
        }
    }
}

void VoiceManager::updateState() {
//    updateSelectedPhraseState();
    updateUseAsSourceState();
}

vector<const string> VoiceManager::getRollKeys() {
    vector<const string> rollKeys;
    transform (voices.begin(), voices.end(), back_inserter(rollKeys), [&](auto voicePair) {
        return generator.rollsKey(voicePair.first);
    });
    return rollKeys;
}
vector<const string> VoiceManager::getOrnamentKeys() {
    vector<const string> ornamentKeys;
    transform (voices.begin(), voices.end(), back_inserter(ornamentKeys), [&](auto voicePair) {
        return generator.ornamentsKey(voicePair.first);
    });
    return ornamentKeys;
}

void VoiceManager::setOnClicks() {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        VoiceControls &voice = voiceIt->second;
        string voiceName = voice.voiceName;
        voice.midiChannel.onChange = [this, voiceName]() { midiChannelChanged(voiceName); };
        voice.generateButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(playQueue->getMidiChannel(voiceName));
            generator.generate(voiceName);
        };
        voice.generateFromButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(playQueue->getMidiChannel(voiceName));
            generator.generateFrom(voiceName, useAsSourcePhraseKeyState);
        };
        voice.muteButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(playQueue->getMidiChannel(voiceName));
            bool muted = playQueue->toggleMuteVoice(voiceName);
            voices.at(voiceName).muteButton.setToggleState(muted, juce::dontSendNotification);
        };
//        voice.selectButton.onClick = [this]() { updateSelectedPhraseState(); };
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
        
        voice.settingsButton.onClick = [this]() {
            VoiceSettingsMenuComponent *voiceSettings = new VoiceSettingsMenuComponent();
            this->mainEditor->addAndMakeVisible(voiceSettings);
            voiceSettings->setBounds(0, 0, editorWidth, editorHeight);
        };
        
        voice.transformButton.onClick = [this]() {
            TransformPhraseMenuComponent *voiceSettings = new TransformPhraseMenuComponent();
            this->mainEditor->addAndMakeVisible(voiceSettings);
            voiceSettings->setBounds(0, 0, editorWidth, editorHeight);
        };
    }
}
