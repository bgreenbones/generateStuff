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
    int cursorY = margin * 2;
    
    
    // TODO: use apvts to put parameter controls on the menu
    auto groups = processor.getParameterTree().getSubgroups(true);
    for (auto group : groups) { // TODO: maybe do this iteration in constructor and save our group for this method instead.
        if (group->getName().compare(voiceName) == 0) {
            for (auto parameter_it = group->begin(); parameter_it != group->end(); ++parameter_it) {
                auto parameter = (*parameter_it)->getParameter();
                if (parameter != nullptr) {
                    parameter->isBoolean(); // use a button
                    parameter->isDiscrete(); // use a dropdown or something? how to know names of selections??
                    // all else use a knob
                    parameter->getName(20); // for label
                    parameter->getLabel(); // for units
                }
            }
        }
    }
    
    
    return cursorY;
}

void VoiceSettingsMenuComponent::updateMenuState() { }
