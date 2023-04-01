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
#include "Voice.h"
 
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
    const string voiceName;
    juce::ComboBox midiChannel;
    ParameterLayoutFunction getParameters;
    juce::TextButton generateButton;
    juce::TextButton generateFromButton;
    juce::TextButton useAsSourceButton;
    juce::TextButton muteButton;
    juce::TextButton improviseButton;
    juce::TextButton settingsButton;
    juce::TextButton transformButton;
    juce::TextButton expressionButton; // TODO: make these menu!! and move the other pop ups to their own files...
    juce::TextButton ornamentButton;

    static const int selectVoiceGroupId = 98374; // random
    static const int useAsSourceGroupId = 29384;
    static const int midiChannelLowerBound = 1; // TODO: find a place for these?
    static const int midiChannelUpperBound = 15;
    
    VoiceControls(VoiceBindings vb): VoiceControls(vb.voiceName, vb.midiChannel, vb.getParameters) {}
    VoiceControls(string name, int defaultMidiChannel, ParameterLayoutFunction getParameters):
        voiceName(name),
        getParameters(getParameters),
        generateButton(juce::TextButton(name)),
        generateFromButton(juce::TextButton("from source")),
        useAsSourceButton(juce::TextButton("use as source")),
        muteButton(juce::TextButton("mute " + name)),
        improviseButton("improvise"),
        settingsButton("settings"),
        transformButton("transform"),
        expressionButton("expression"),
        ornamentButton("ornament")
    {
        buttons = { &generateButton, &generateFromButton, &useAsSourceButton, &muteButton, &improviseButton,
                    &settingsButton, &expressionButton, &ornamentButton, &transformButton };
        for (auto button : buttons) { button->setLookAndFeel (&lookAndFeel); }
        for (int channel = midiChannelLowerBound; channel <= midiChannelUpperBound; channel++) { midiChannel.addItem(juce::String(channel), channel); }
//        defaultMidiChannel = max(midiChannelLowerBound, defaultMidiChannel); // TODO: why does this cause undefined symbol
//        defaultMidiChannel = min(midiChannelUpperBound, defaultMidiChannel);
        midiChannel.setSelectedId(defaultMidiChannel);
    }
    
    VoiceControls(VoiceControls const& other): VoiceControls(other.voiceName, other.midiChannel.getSelectedId(), other.getParameters) {};

    ~VoiceControls()
    {
        buttons.clear();
        buttons.shrink_to_fit();
    }
    
    void configureButtons();
    std::size_t getNumberOfButtons();
    std::size_t getNumberOfColumns();
    void callAddAndMakeVisible(Component *editor);
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls);
};
