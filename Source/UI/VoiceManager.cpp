/*
  ==============================================================================

    VoiceManager.cpp
    Created: 21 Mar 2023 11:15:31pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceManager.h"


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
