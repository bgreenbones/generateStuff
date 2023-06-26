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
#include "PluginProcessor.h"
 
using std::string, std::vector;



class GenerateStuffLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GenerateStuffLookAndFeel()
    {
    }
    
    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/) {
        juce::Font font (getTextButtonFont (button, button.getHeight()));
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
        const int cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
        const int leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              juce::Justification::centred, 6);
    }
};


class VoiceSettingsRowComponent : public juce::Component {
public:
    
    VoiceSettingsRowComponent(VoiceName voiceName, GenerateStuffAudioProcessor& processor); 
    ~VoiceSettingsRowComponent() override {
        for (auto attachment : buttonAttachments) {
            delete attachment;
        }
        for (auto attachment : sliderAttachments) {
            delete attachment;
        }
        for (auto attachment : comboBoxAttachments) {
            delete attachment;
        }
    }
    // void paint (juce::Graphics& g) override;
    void resized() override;
    
    GenerateStuffAudioProcessor &processor;
    VoiceName voiceName;
    GenerateStuffEditorState& editorState;
    Ensemble& ensemble;
    
    juce::OwnedArray<juce::Label> sliderLabels;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    
    juce::OwnedArray<juce::TextButton> buttons;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;
    
    juce::OwnedArray<juce::Label> comboBoxLabels;
    juce::OwnedArray<juce::ComboBox> comboBoxes;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachments;
    
private:
    int workspaceWidth = 500; // todo: figure out
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceSettingsRowComponent)
};

class VoiceControls {
    GenerateStuffLookAndFeel lookAndFeel;
    vector<juce::TextButton*> buttons;
    // vector<juce::Slider*> sliders;
    // juce::OwnedArray<juce::Label> sliderLabels;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachments;
    
public:
    const string voiceName;
    juce::ComboBox midiChannel;
    juce::TextButton generateButton;
    juce::TextButton generateFromButton;
    juce::TextButton useAsSourceButton;
    juce::TextButton muteButton;
    juce::TextButton improviseButton;
    juce::TextButton settingsButton;
    juce::TextButton transformButton;
    juce::TextButton expressionButton; // TODO: make these menu!! and move the other pop ups to their own files...
    juce::TextButton ornamentButton;
    function<void()> improviseFunction;
    VoiceSettingsRowComponent settingsRow;
    juce::Slider *densitySlider;
    juce::Slider *highPitchKnob;
    juce::Slider *lowPitchKnob;
    
    
    
    juce::Label voiceLabel;
    static const int selectVoiceGroupId = 98374; // random
    static const int useAsSourceGroupId = 29384;
    static const int midiChannelLowerBound = 1; // TODO: find a place for these?
    static const int midiChannelUpperBound = 15;
    
    VoiceControls(Voice const& voice, GenerateStuffAudioProcessor &processor): VoiceControls(voice.name, voice.midiChannel, processor) {}
    VoiceControls(string name, int defaultMidiChannel, GenerateStuffAudioProcessor& processor):
        voiceName(name),
        settingsRow(name, processor),
        generateButton(juce::TextButton(name)),
        generateFromButton(juce::TextButton("from source")),
        useAsSourceButton(juce::TextButton("use as source")),
        muteButton(juce::TextButton("mute " + name)),
        improviseButton("improvise"),
        settingsButton("settings"),
        transformButton("transform"),
        expressionButton("expression"),
        ornamentButton("ornament"),
        voiceLabel(name + "Label", name)
    {
        buttons = { &generateButton, &generateFromButton, &useAsSourceButton, &muteButton, &improviseButton,
                    &settingsButton, &expressionButton, &ornamentButton, &transformButton };
//        sliders = { &highPitchKnob, &lowPitchKnob, &densitySlider };
        for (auto button : buttons) { button->setLookAndFeel (&lookAndFeel); }
        for (auto slider : sliders) { slider->setLookAndFeel (&lookAndFeel); }
        for (int channel = midiChannelLowerBound; channel <= midiChannelUpperBound; channel++) { midiChannel.addItem(juce::String(channel), channel); }
//        defaultMidiChannel = max(midiChannelLowerBound, defaultMidiChannel); // TODO: why does this cause undefined symbol
//        defaultMidiChannel = min(midiChannelUpperBound, defaultMidiChannel);
        midiChannel.setSelectedId(defaultMidiChannel);


        densitySlider = new juce::Slider(juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextEntryBoxPosition::TextBoxBelow);
        highPitchKnob = new juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxAbove);
        lowPitchKnob = new juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxAbove);
        sliders.add(densitySlider);
        sliders.add(highPitchKnob);
        sliders.add(lowPitchKnob);

        auto parameter = processor.apvts.getParameter(voiceName + "Density");
        juce::AudioParameterFloat* floatParameter = nullptr;
        floatParameter = dynamic_cast<juce::AudioParameterFloat*>(parameter);
        jassert (floatParameter);  // If you get an error, the parameter doesn't exist or is of different type
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(processor.apvts, floatParameter->getParameterID(), *densitySlider));
        parameter = processor.apvts.getParameter(voiceName + "HighPitch");
        floatParameter = nullptr;
        floatParameter = dynamic_cast<juce::AudioParameterFloat*>(parameter);
        jassert (floatParameter);  // If you get an error, the parameter doesn't exist or is of different type
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(processor.apvts, floatParameter->getParameterID(), *highPitchKnob));
        parameter = processor.apvts.getParameter(voiceName + "LowPitch");
        floatParameter = nullptr;
        floatParameter = dynamic_cast<juce::AudioParameterFloat*>(parameter);
        jassert (floatParameter);  // If you get an error, the parameter doesn't exist or is of different type
        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(processor.apvts, floatParameter->getParameterID(), *lowPitchKnob));
    }
    
    VoiceControls(VoiceControls const& other): VoiceControls(other.voiceName, other.midiChannel.getSelectedId(), other.settingsRow.processor) {};

    ~VoiceControls()
    {
        buttons.clear();
        buttons.shrink_to_fit();
    }
    
    void configureButtons();
    std::size_t getNumberOfButtons();
    std::size_t getNumberOfColumns();
    void callAddAndMakeVisible(juce::Component *editor);
    void setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls);
};


