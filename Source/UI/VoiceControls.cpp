/*
  ==============================================================================

    VoiceControls.cpp
    Created: 21 Mar 2023 11:15:43pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceControls.h"
#include "CustomUIs.h"


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
    for (auto slider : sliders) {
        editor->addAndMakeVisible (slider);
    }
    editor->addAndMakeVisible (&settingsRow);
    editor->addAndMakeVisible (&voiceLabel);
}

void VoiceControls::setBounds(int xCursor, int yCursor, int buttonWidth, int buttonHeight, int spaceBetweenControls) {
    // auto incrementCursor = [&]() { xCursor += buttonWidth + spaceBetweenControls; };
    int comboBoxHeight = 40;
    int comboBoxY = yCursor + buttonHeight / 2 - comboBoxHeight / 2;
    midiChannel.setBounds(xCursor, comboBoxY, buttonWidth, comboBoxHeight);
    // incrementCursor();
    // for (auto button : buttons) {
    //     button->setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    //     incrementCursor();
    // }
    // settingsRow.setBounds(xCursor, yCursor, 700, buttonHeight);
    auto incrementCursor = [&]() {yCursor += buttonHeight + spaceBetweenControls;    };
    incrementCursor();
    highPitchKnob->setBounds(xCursor, yCursor, buttonWidth, buttonHeight);
    incrementCursor();
    lowPitchKnob->setBounds(xCursor, yCursor, buttonWidth, buttonHeight);
    incrementCursor();
    densitySlider->setBounds(xCursor, yCursor, buttonWidth, 2. * buttonHeight);    
    incrementCursor();
    incrementCursor();
    voiceLabel.setBounds(xCursor, yCursor, buttonWidth, buttonHeight);    
}



VoiceSettingsRowComponent::VoiceSettingsRowComponent(VoiceName voiceName, GenerateStuffAudioProcessor& processor):
        voiceName(voiceName), editorState(processor.editorState), ensemble(processor.ensemble), processor(processor)
{
    if (editorState.voiceSettingsStates.find(voiceName) == editorState.voiceSettingsStates.end()) {
        editorState.voiceSettingsStates.emplace(voiceName, VoiceSettingsEditorState());
    }
}



void VoiceSettingsRowComponent::resized()
{
    sliders.clear();
    sliderLabels.clear();
    sliderAttachments.clear();
    
    buttons.clear();
    buttonAttachments.clear();
    
    comboBoxes.clear();
    comboBoxLabels.clear();
    comboBoxAttachments.clear();
    
    int cursorY = 0;
    // int cursorY = margin * 2;
    int maxNameLength = 20;
    
    auto groups = processor.getParameterTree().getSubgroups(true);
    for (auto group : groups) { // TODO: maybe do this iteration in constructor and save our group for this method instead.
        if (group->getName().compare(voiceName) == 0) {
            for (auto parameter_it = group->begin(); parameter_it != group->end(); ++parameter_it) {
                auto parameter = (*parameter_it)->getParameter();
                if (parameter != nullptr) {
                    if (parameter->isBoolean()) {
                        // use a button
                        juce::AudioParameterBool* boolean = nullptr;
                        boolean = dynamic_cast<juce::AudioParameterBool*>(parameter);
                        jassert (boolean);  // If you get an error, the parameter doesn't exist or is of different type
                        
                        buttons.add(new juce::TextButton(parameter->getName(maxNameLength)));
                        buttons.getLast()->setClickingTogglesState(true);
                        buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(processor.apvts, boolean->getParameterID(), *(buttons.getLast())));
                        addAndMakeVisible(buttons.getLast());
                    } else if (parameter->isDiscrete()) {
                        // use a dropdown or something? how to know names of selections??
                        juce::AudioParameterChoice* choice = nullptr;
                        choice = dynamic_cast<juce::AudioParameterChoice*>(parameter);
                        jassert (choice);  // If you get an error, the parameter doesn't exist or is of different type

                        comboBoxes.add(new juce::ComboBox());
                        comboBoxes.getLast()->addItemList(choice->choices, 1);
                        
                        comboBoxLabels.add(new juce::Label());
                        addAndMakeVisible (comboBoxLabels.getLast());
                        comboBoxLabels.getLast()->setText (parameter->getName(maxNameLength), juce::dontSendNotification);
                        comboBoxLabels.getLast()->setJustificationType(juce::Justification::centred);
                        comboBoxLabels.getLast()->attachToComponent (comboBoxes.getLast(), true);
                        
                        comboBoxAttachments.add(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(processor.apvts, choice->getParameterID(), *(comboBoxes.getLast())));
                        addAndMakeVisible(comboBoxes.getLast());
                    } else {
                        // all else use a knob
                        juce::AudioParameterFloat* floatParameter = nullptr;
                        floatParameter = dynamic_cast<juce::AudioParameterFloat*>(parameter);
                        jassert (floatParameter);  // If you get an error, the parameter doesn't exist or is of different type
                        
                        auto sliderStyle = juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag;
                        auto textBoxStyle = juce::Slider::TextBoxBelow;
                        sliders.add(new juce::Slider(sliderStyle, textBoxStyle));
                        sliders.getLast()->setTextValueSuffix(parameter->getLabel());
                        
                        sliderLabels.add(new juce::Label());
                        addAndMakeVisible (sliderLabels.getLast());
                        sliderLabels.getLast()->setText (parameter->getName(maxNameLength), juce::dontSendNotification);
                        sliderLabels.getLast()->setJustificationType(juce::Justification::centred);
                        sliderLabels.getLast()->attachToComponent (sliders.getLast(), true);
                        
                        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(processor.apvts, floatParameter->getParameterID(), *(sliders.getLast())));
                        addAndMakeVisible(sliders.getLast());
                    }
                }
            }
        }
    }
    
    // PLACE BUTTONS
    int buttonWidth = 70;
    int buttonHeight = 70;
    
    int labelHeight = 0;
    int buttonRowHeight = ui::spaceBetweenControls + labelHeight + ui::spaceBetweenControls + buttonHeight;
    // int buttonsPerRow = workspaceWidth / (buttonWidth + ui::spaceBetweenControls);
    // int centerX = getWidth() / 2;

    // int rowWiseI_0 = 0;
    int rowWiseI = 0;
    for (int i = 0; i < buttons.size(); i++) {
        // int rowWiseI = i % buttonsPerRow;
        // rowWiseI_0 = rowWiseI == 0 ? i : rowWiseI_0;
        // int buttonsInThisRow = min(buttons.size() - rowWiseI_0, buttonsPerRow);
        // int rowTotalWidth = buttonsInThisRow * buttonWidth + ui::spaceBetweenControls * (buttonsInThisRow - 1);

        int buttonX = rowWiseI * (buttonWidth + ui::spaceBetweenControls);
        // cursorY = rowWiseI == 0 && i > 0 ? cursorY + buttonRowHeight : cursorY;

        buttons[i]->setBounds(buttonX, cursorY + labelHeight + ui::spaceBetweenControls, buttonWidth, buttonHeight);
        rowWiseI++;
    }
    // cursorY += buttons.size() > 0 ? buttonRowHeight + ui::spaceBetweenControls : 0;
    
    // PLACE DROPDOWNS
    int comboBoxWidth = buttonWidth;
    int comboBoxHeight = 40;
    labelHeight = comboBoxLabels.size() == 0 ? 0 : comboBoxLabels.getLast()->getFont().getHeight();
    // int comboBoxRowHeight = ui::spaceBetweenControls + labelHeight + ui::spaceBetweenControls + comboBoxHeight;
    // int comboBoxesPerRow = workspaceWidth / (comboBoxWidth + ui::spaceBetweenControls);
//    int centerX = getWidth() / 2;
    
    // rowWiseI_0 = 0;
    for (int i = 0; i < comboBoxes.size(); i++) {
        // int rowWiseI = i % comboBoxesPerRow;
        // rowWiseI_0 = rowWiseI == 0 ? i : rowWiseI_0;
        // int comboBoxesInThisRow = min(comboBoxes.size() - rowWiseI_0, comboBoxesPerRow);
        // int rowTotalWidth = comboBoxesInThisRow * comboBoxWidth + ui::spaceBetweenControls * (comboBoxesInThisRow - 1);
        
        int comboBoxX = rowWiseI * (comboBoxWidth + ui::spaceBetweenControls);
        // cursorY = rowWiseI == 0 && i > 0 ? cursorY + comboBoxRowHeight : cursorY;
        
        comboBoxes[i]->setBounds(comboBoxX, cursorY + labelHeight + ui::spaceBetweenControls, comboBoxWidth, comboBoxHeight);
        comboBoxLabels[i]->setBounds(comboBoxX, cursorY, comboBoxWidth, labelHeight);
        rowWiseI++;
    }
    // cursorY += comboBoxes.size() > 0 ? comboBoxRowHeight + ui::spaceBetweenControls : 0;
    
    // PLACE KNOBS
    int knobWidth = buttonWidth;
    int knobHeight = buttonHeight;
    labelHeight = sliderLabels.size() == 0 ? 0 : sliderLabels.getLast()->getFont().getHeight();
    // int knobRowHeight = ui::spaceBetweenControls + labelHeight + ui::spaceBetweenControls + knobHeight;
    // int knobsPerRow = workspaceWidth / (knobWidth + ui::spaceBetweenControls);
//    int centerX = getWidth() / 2;
    
    // rowWiseI_0 = 0;
    for (int i = 0; i < sliders.size(); i++) {
        // int rowWiseI = i % knobsPerRow;
        // rowWiseI_0 = rowWiseI == 0 ? i : rowWiseI_0;
        // int knobsInThisRow = min(sliders.size() - rowWiseI_0, knobsPerRow);
        // int rowTotalWidth = knobsInThisRow * knobWidth + ui::spaceBetweenControls * (knobsInThisRow - 1);
        
        int knobX = rowWiseI * (knobWidth + ui::spaceBetweenControls);
//        cursorY = rowWiseI == 0 && i > 0 ? cursorY + knobRowHeight : cursorY;
        
        sliders[i]->setBounds(knobX, cursorY + labelHeight + ui::spaceBetweenControls, knobWidth, knobHeight);
        sliderLabels[i]->setBounds(knobX, cursorY, knobWidth, labelHeight);
        rowWiseI++;
    }
    // cursorY += sliders.size() > 0 ? knobRowHeight + ui::spaceBetweenControls : 0;
    
}

