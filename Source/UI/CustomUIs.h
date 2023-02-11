/*
  ==============================================================================

    CustomUIs.h
    Created: 31 Dec 2022 11:29:40pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include <string>
#include <vector>
#include <JuceHeader.h>
using std::string, std::vector;



/*
 ### Interface for making quick UIs really easily
 
 class parameterGroup:
    list parameters
    enum knobType (knob, slider, input, group of buttons)
    layout layout (rows x (parameters.length / rows))
 
 class parameter:
    string name
    reference value
    enum step-size/steps scheme
 
 ### UI for creating UIs
 -- list of parameters to choose from
    -- make an easy way to expose 
 -- can assign any param to parameterGroups
 -- can arrange parameterGroups on the UI (that we're creating)
 
 */


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
    juce::TextButton generateButton; // randomCascaraButton { "casc "};
    juce::TextButton generateFromButton; // cascaraFromClaveButton { "clv->cas" };
    juce::TextButton useAsSourceButton;
    juce::TextButton muteButton; // clear...
    juce::TextButton selectButton; // selectCascaraButton { "casc" };
    juce::TextButton improviseButton; // regenerateRolls { "regen rolls" }; // re-gen rolls on loop
//    juce::TextButton flipButton { "flip" };
    
    
    VoiceControls(string name):
        voiceName(name),
        generateButton(juce::TextButton("new " + name)),
        generateFromButton(juce::TextButton("new " + name + " from source")),
        useAsSourceButton(juce::TextButton("use " + name + " as source")),
        muteButton(juce::TextButton("mute " + name)),
        selectButton(juce::TextButton("select " + name)),
        improviseButton("improvise " + name)
    {
        buttons = { &generateButton, &generateFromButton, &useAsSourceButton, &muteButton, &selectButton, &improviseButton };
        for (auto button : buttons) {
            button->setLookAndFeel (&lookAndFeel);
        }
    }
    
    std::size_t getNumberOfButtons() {
        return buttons.size();
    }

    void callAddAndMakeVisible(Component *editor) {
        for (auto button : buttons) {
            editor->addAndMakeVisible (button);
        }
    }
    
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
        auto incrementCursor = [&]() { xCursor += buttonWidth + spaceBetweenControls; };
        for (auto button : buttons) {
            button->setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
            incrementCursor();
        }
    }
    
};

class VoiceManager {
    
//    const vector<const string> phraseKeys = { cascaraKey, claveKey };
//    vector<const string> getRollKeys() {
//        vector<const string> rollKeys;
//        transform (phraseKeys.begin(), phraseKeys.end(), back_inserter(rollKeys), [&](string phraseKey) {
//            return generator.rollsKey(phraseKey);
//        });
//        return rollKeys;
//    }
//    vector<const string> getOrnamentKeys() {
//        vector<const string> ornamentKeys;
//        transform (phraseKeys.begin(), phraseKeys.end(), back_inserter(ornamentKeys), [&](string phraseKey) {
//            return generator.ornamentsKey(phraseKey);
//        });
//        return ornamentKeys;
//    }
//    string selectedPhraseKeyState;
};





