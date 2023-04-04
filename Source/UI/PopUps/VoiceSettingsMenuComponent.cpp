/*
  ==============================================================================

    VoiceSettingsMenuComponent.cpp
    Created: 29 Mar 2023 12:13:58am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "VoiceSettingsMenuComponent.h"


VoiceSettingsMenuComponent::VoiceSettingsMenuComponent(VoiceName voiceName, GenerateStuffAudioProcessor& processor):
                                                            VoiceEditor(voiceName, processor.editorState),
                                                            processor(processor)
{
    if (editorState->voiceSettingsStates.find(voiceName) == editorState->voiceSettingsStates.end()) {
        editorState->voiceSettingsStates.emplace(voiceName, VoiceSettingsEditorState());
    }
}

int VoiceSettingsMenuComponent::placeWorkspace()
{
    sliders.clear();
    sliderLabels.clear();
    buttons.clear();
    
    int cursorY = margin * 2;
    int maxNameLength = 20;
    
    // TODO: use apvts to put parameter controls on the menu
    auto groups = processor.getParameterTree().getSubgroups(true);
    for (auto group : groups) { // TODO: maybe do this iteration in constructor and save our group for this method instead.
        if (group->getName().compare(voiceName) == 0) {
            for (auto parameter_it = group->begin(); parameter_it != group->end(); ++parameter_it) {
                auto parameter = (*parameter_it)->getParameter();
                if (parameter != nullptr) {
                    if (parameter->isBoolean()) {
                        // use a button
                    } else if (parameter->isDiscrete()) {
                        // use a dropdown or something? how to know names of selections??
                    } else {
                        // all else use a knob
                        auto sliderStyle = juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag;
                        auto textBoxStyle = juce::Slider::TextBoxBelow;
                        sliders.add(new juce::Slider(sliderStyle, textBoxStyle));
                        sliders.getLast()->setTextValueSuffix(parameter->getLabel());
                        
                        sliderLabels.add(new juce::Label());
                        addAndMakeVisible (sliderLabels.getLast());
                        sliderLabels.getLast()->setText (parameter->getName(maxNameLength), juce::dontSendNotification);
                        sliderLabels.getLast()->setJustificationType(juce::Justification::centred);
                        sliderLabels.getLast()->attachToComponent (sliders.getLast(), true);
                        
                        sliderAttachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(processor.apvts, parameter->getName(maxNameLength), *(sliders.getLast())));
                        addAndMakeVisible(sliders.getLast());
                    }
//                    parameter->getName(maxNameLength); // for label
//                    parameter->getLabel(); // for units
                }
            }
        }
    }
    
    // PLACE KNOBS
    int knobWidth = 100;
    int knobHeight = 100;
    int labelHeight = sliderLabels.size() == 0 ? 0 : sliderLabels.getLast()->getFont().getHeight();
    int knobRowHeight = spaceBetweenControls + labelHeight + spaceBetweenControls + knobHeight;
    int knobsPerRow = workspaceWidth / (knobWidth + spaceBetweenControls);
    int centerX = getWidth() / 2;
    
    int rowWiseI_0 = 0;
    for (int i = 0; i < sliders.size(); i++) {
        int rowWiseI = i % knobsPerRow;
        rowWiseI_0 = rowWiseI == 0 ? i : rowWiseI_0;
        int knobsInThisRow = min(sliders.size() - rowWiseI_0, knobsPerRow);
        int rowTotalWidth = knobsInThisRow * knobWidth + spaceBetweenControls * (knobsInThisRow - 1);
        
        int knobX = centerX - (rowTotalWidth / 2) + rowWiseI * (knobWidth + spaceBetweenControls);
        cursorY = rowWiseI == 0 && i > 0 ? cursorY + knobRowHeight : cursorY;
        
        sliders[i]->setBounds(knobX, cursorY + labelHeight + spaceBetweenControls, knobWidth, knobHeight);
        sliderLabels[i]->setBounds(knobX, cursorY, knobWidth, labelHeight);
    }
    cursorY += knobRowHeight + spaceBetweenControls;
    
    return cursorY;
}

void VoiceSettingsMenuComponent::updateMenuState() { }
