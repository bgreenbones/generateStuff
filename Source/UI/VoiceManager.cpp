/*
  ==============================================================================

    VoiceManager.cpp
    Created: 21 Mar 2023 11:15:31pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceManager.h"
#include "VoiceSettingsMenuComponent.h"
#include "TransformPhraseMenuComponent.h"
#include "OrnamentationMenuComponent.h"
#include "ExpressionMenuComponent.h"


VoiceManager::VoiceManager(GenerateStuffAudioProcessor& processor):
    processor(processor),
    generator(processor.generator),
    ensemble(processor.ensemble)
{
    for (VoiceBindings vb : voiceBindings) {
        voices.emplace(vb.voiceName, VoiceControls(vb));
    }
}

VoiceManager::~VoiceManager()
{
    voices.clear();
}


void VoiceManager::midiChannelChanged(string voiceName) {
    int midiChannel = voices.at(voiceName).midiChannel.getSelectedId();
    ensemble.setMidiChannel(voiceName, midiChannel);
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

void VoiceManager::updateUseAsSourceState() {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        bool voiceSelected = voiceIt->second.useAsSourceButton.getToggleState();
        if (voiceSelected) {
            // this->useAsSourcePhraseKeyState = voiceIt->second.voiceName;
            processor.editorState.useAsSourcePhraseKey = voiceIt->second.voiceName;
            break;
        }
    }
}

void VoiceManager::updateState() {
    updateUseAsSourceState();
}


void VoiceManager::setOnClicks() {
    for (auto voiceIt = voices.begin(); voiceIt != voices.end(); voiceIt++) {
        VoiceControls &voiceControls = voiceIt->second;
        VoiceName voiceName = voiceControls.voiceName;
        voiceControls.midiChannel.onChange = [this, voiceName]() { midiChannelChanged(voiceName); };
        voiceControls.generateButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(ensemble.getMidiChannel(voiceName));
            // generator.generate(voiceName);
            Voice& voice = ensemble.getVoice(voiceName);
            Phrase phrase = voice.newPhrase();
            voice.schedule.schedulePhrase(Form(), phrase);
            // VoiceControls &voice = voices.at(voiceName);
            // voice.improviseFunction = [&]() { generator.generate(voiceName); };
        };
        voiceControls.generateFromButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(ensemble.getMidiChannel(voiceName));
            Voice& voice = ensemble.getVoice(voiceName);
            Phrase phrase = voice.phraseFrom();
            voice.schedule.schedulePhrase(Form(), phrase);
            // ensemble.getVoice(voiceName).variation();
            // generator.gen∫erateFrom(voiceName, useAsSourcePhraseKeyState);
            // VoiceControls &voice = voices.at(voiceName);
            // voice.improviseFunction = [&]() { generator.generateFrom(voiceName, useAsSourcePhraseKeyState); };
        };
        voiceControls.muteButton.onClick = [this, voiceName]() {
            processor.issueNoteOff(ensemble.getMidiChannel(voiceName));
            bool muted = ensemble.toggleMuteVoice(voiceName);
            voices.at(voiceName).muteButton.setToggleState(muted, juce::dontSendNotification);
        };
        voiceControls.useAsSourceButton.onClick = [this]() { updateUseAsSourceState(); };
        voiceControls.improviseButton.onClick = [this, voiceName]() {
            VoiceControls &voice = voices.at(voiceName);
            bool improvise = !(voice.improviseButton.getToggleState());
            if (improvise) {
                processor.loopTasks.queue(voiceName, voice.improviseFunction);
            } else {
                processor.loopTasks.deactivate({ voiceName });
            }
            voice.improviseButton.setToggleState(improvise, juce::dontSendNotification);
        };
        
        voiceControls.settingsButton.onClick = [voiceControls, this]() {
            VoiceSettingsMenuComponent *voiceSettings = new VoiceSettingsMenuComponent(voiceControls.voiceName, processor);
            this->mainEditor->addAndMakeVisible(voiceSettings);
            voiceSettings->resized(); // TODO: super gross that i am needing to manually call resized() to get the actual subclass's implementation...
        };
        
        voiceControls.transformButton.onClick = [voiceControls, this]() {
            TransformPhraseMenuComponent *transformMenu = new TransformPhraseMenuComponent(voiceControls.voiceName, processor);
            this->mainEditor->addAndMakeVisible(transformMenu);
            transformMenu->resized();
        };
        
        voiceControls.ornamentButton.onClick = [voiceControls, this]() {
            OrnamentationMenuComponent *ornamentationMenu = new OrnamentationMenuComponent(voiceControls.voiceName, processor);
            this->mainEditor->addAndMakeVisible(ornamentationMenu);
            ornamentationMenu->resized();
        };
        
        voiceControls.expressionButton.onClick = [voiceControls, this]() {
            ExpressionMenuComponent *expressionMenu = new ExpressionMenuComponent(voiceControls.voiceName, processor);
            this->mainEditor->addAndMakeVisible(expressionMenu);
            expressionMenu->resized();
        };
    }
}
