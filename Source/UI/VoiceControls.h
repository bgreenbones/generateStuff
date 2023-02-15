/*
  ==============================================================================

    VoiceControls.h
    Created: 12 Feb 2023 10:25:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include <string>
#include <vector>
#include <JuceHeader.h>

using std::string, std::vector;


using namespace juce;

class GenerateStuffLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GenerateStuffLookAndFeel()
    {
    }
    
    void drawButtonText (Graphics& g, TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) {
        Font font (getTextButtonFont (button, button.getHeight()));
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                                                : TextButton::textColourOffId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = roundToInt (font.getHeight() * 0.6f);
        const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              Justification::centred, 6);
    }
};


class VoiceControls {
    GenerateStuffLookAndFeel lookAndFeel;
    vector<juce::TextButton*> buttons;
public:
    const string voiceName; // cascaraKey = "cascara";
    juce::ComboBox midiChannel;
    juce::TextButton generateButton; // randomCascaraButton { "casc "};
    juce::TextButton generateFromButton; // cascaraFromClaveButton { "clv->cas" };
    juce::TextButton useAsSourceButton;
    juce::TextButton muteButton; // clear...
    juce::TextButton selectButton; // selectCascaraButton { "casc" };
    juce::TextButton improviseButton; // regenerateRolls { "regen rolls" }; // re-gen rolls on loop
//    juce::TextButton flipButton { "flip" };
    static const int selectVoiceGroupId = 98374; // random
    static const int useAsSourceGroupId = 29384;
    static const int midiChannelLowerBound = 1; // TODO: find a place for these?
    static const int midiChannelUpperBound = 15;
    
    
    VoiceControls(string name, int defaultMidiChannel):
        voiceName(name),
        generateButton(juce::TextButton("new " + name)),
        generateFromButton(juce::TextButton("new " + name + " from source")),
        useAsSourceButton(juce::TextButton("use " + name + " as source")),
        muteButton(juce::TextButton("mute " + name)),
        selectButton(juce::TextButton("select " + name)),
        improviseButton("improvise " + name)
    {
        buttons = { &generateButton, &generateFromButton, &useAsSourceButton, &muteButton, &selectButton, &improviseButton };
        for (auto button : buttons) { button->setLookAndFeel (&lookAndFeel); }
        for (int channel = midiChannelLowerBound; channel <= midiChannelUpperBound; channel++) { midiChannel.addItem(juce::String(channel), channel); }
//        defaultMidiChannel = max(midiChannelLowerBound, defaultMidiChannel); // TODO: why does this cause undefined symbol
//        defaultMidiChannel = min(midiChannelUpperBound, defaultMidiChannel);
        midiChannel.setSelectedId(defaultMidiChannel);
    }
    
    ~VoiceControls()
    {
        buttons.clear();
        buttons.shrink_to_fit();
    }
    
    void configureButtons() {
        selectButton.setRadioGroupId(selectVoiceGroupId);
        selectButton.setClickingTogglesState(true);
        selectButton.setToggleState(false, juce::dontSendNotification);
        
        useAsSourceButton.setRadioGroupId(useAsSourceGroupId);
        useAsSourceButton.setClickingTogglesState(true);
        useAsSourceButton.setToggleState(false, juce::dontSendNotification);
    }
    
    std::size_t getNumberOfButtons() {
        return buttons.size();
    }
    
    std::size_t getNumberOfColumns() {
        return getNumberOfButtons() + 1;
    }

    void callAddAndMakeVisible(Component *editor) {
        editor->addAndMakeVisible (midiChannel);
        for (auto button : buttons) {
            editor->addAndMakeVisible (button);
        }
    }
    
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
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
    
};
