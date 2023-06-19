/*
  ==============================================================================

    VoiceControls.cpp
    Created: 21 Mar 2023 11:15:43pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceControls.h"



void VoiceControls::configureButtons() {    
    useAsSourceButton.setRadioGroupId(useAsSourceGroupId);
    useAsSourceButton.setClickingTogglesState(true);
    useAsSourceButton.setToggleState(false, juce::dontSendNotification);
}

std::size_t VoiceControls::getNumberOfButtons() {
    return buttons.size();
}

std::size_t VoiceControls::getNumberOfColumns() {
    return getNumberOfButtons() + 1;
}

void VoiceControls::callAddAndMakeVisible(juce::Component *editor) {
    editor->addAndMakeVisible (midiChannel);
    for (auto button : buttons) {
        editor->addAndMakeVisible (button);
    }
}

void VoiceControls::setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
    auto incrementCursor = [&]() { xCursor += buttonWidth + spaceBetweenControls; };
    int comboBoxHeight = 40;
    int comboBoxY = yCursor + buttonHeight / 2 - comboBoxHeight / 2;
    midiChannel.setBounds(xCursor, comboBoxY, buttonWidth, comboBoxHeight);
    incrementCursor();
    for (auto button : buttons) {
        button->setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        incrementCursor();
    }
}
