/*
  ==============================================================================

    OrnamentationMenuComponent.cpp
    Created: 29 Mar 2023 12:14:45am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "OrnamentationMenuComponent.h"



OrnamentationMenuComponent::OrnamentationMenuComponent(VoiceName voiceName,
                                                       GenerateStuffAudioProcessor &processor):
                                                                VoiceEditor(voiceName, processor),
                                                                audioProcessor(processor)
{
    if (editorState->ornamentationStates.find(voiceName) == editorState->ornamentationStates.end()) {
        editorState->ornamentationStates.emplace(voiceName, OrnamentationEditorState());
    }
                                                                    
    addConnectingButton.onClick = [this, voiceName]() {
        double connectingProb = Probability(connectingProbability.getValue());
        double associationProb = Probability(connectingAssociation.getValue());
        double connectingLengthProb = Probability(connectingLength.getValue());

        string id = audioProcessor.generator.connectingKey(voiceName);
        function<void()> task = [=]() {
          audioProcessor.generator.connecting(voiceName, connectingProb, associationProb, connectingLengthProb);
        };
        task();
        audioProcessor.loopTasks.queue(id, task, regenerateConnecting.getToggleState());
    };
    addAndMakeVisible (&addConnectingButton);
    
    muteConnectingButton.setClickingTogglesState(true);
    muteConnectingButton.setToggleState(false, juce::dontSendNotification);
    muteConnectingButton.onClick = [this, voiceName]() {
       playQueue->toggleMuteConnecting(voiceName, muteConnectingButton.getToggleState());
    };

    addAndMakeVisible(&muteConnectingButton);
    muteOrnamentsButton.setClickingTogglesState(true);
    muteOrnamentsButton.setToggleState(false, juce::dontSendNotification);
    muteOrnamentsButton.onClick = [this, voiceName]() {
       playQueue->toggleMuteOrnamentation(voiceName, muteOrnamentsButton.getToggleState());
    };
    addAndMakeVisible(&muteOrnamentsButton);
    
    addAndMakeVisible(&connectingProbability);
    connectingProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    connectingProbability.setRange (0.0, 1.0, 0.01);
    connectingProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    connectingProbability.setPopupDisplayEnabled (true, false, this);
    connectingProbability.setTextValueSuffix (" p(connecting)");
    connectingProbability.setValue(1.0);

    addAndMakeVisible(&connectingAssociation);
    connectingAssociation.setSliderStyle (juce::Slider::LinearBarVertical);
    connectingAssociation.setRange (0.0, 1.0, 0.01);
    connectingAssociation.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    connectingAssociation.setPopupDisplayEnabled (true, false, this);
    connectingAssociation.setTextValueSuffix (" connecting association / swing");
    connectingAssociation.setValue(0.5);

    addAndMakeVisible(&connectingLength);
    connectingLength.setSliderStyle (juce::Slider::LinearBarVertical);
    connectingLength.setRange (0.0, 1.0, 0.01);
    connectingLength.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    connectingLength.setPopupDisplayEnabled (true, false, this);
    connectingLength.setTextValueSuffix (" connecting length");
    connectingLength.setValue(0.5);

    flamButton.setClickingTogglesState(true);
    flamButton.setToggleState(true, juce::dontSendNotification);
    addAndMakeVisible(&flamButton);
    dragButton.setClickingTogglesState(true);
    dragButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&dragButton);
    ruffButton.setClickingTogglesState(true);
    ruffButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&ruffButton);
    addOrnamentsButton.onClick = [this, voiceName]() {
        Probability prob = ornamentProbability.getValue();
        double breadth = ornamentBreadth.getValue();
        bool flams = flamButton.getToggleState();
        bool drags = dragButton.getToggleState();
        bool ruffs = ruffButton.getToggleState();

        string id = audioProcessor.generator.ornamentsKey(voiceName);
        function<void()> task = [=]() { audioProcessor.generator.ornament(voiceName, prob, breadth, flams, drags, ruffs); };
        task();
        audioProcessor.loopTasks.queue(id, task, regenerateOrnaments.getToggleState());
    };
    addAndMakeVisible(&addOrnamentsButton);

    addAndMakeVisible(&ornamentProbability);
    ornamentProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentProbability.setRange (0.0, 1.0, 0.01);
    ornamentProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentProbability.setPopupDisplayEnabled (true, false, this);
    ornamentProbability.setTextValueSuffix (" ornament probability");
    ornamentProbability.setValue(0.3);

    addAndMakeVisible(&ornamentBreadth);
    ornamentBreadth.setSliderStyle (juce::Slider::LinearBarVertical);
    ornamentBreadth.setRange (0.0, 4.0, 0.01);
    ornamentBreadth.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    ornamentBreadth.setPopupDisplayEnabled (true, false, this);
    ornamentBreadth.setTextValueSuffix (" ornament breadth");
    ornamentBreadth.setValue(1.0);
    
    
    regenerateConnecting.setClickingTogglesState(true);
    regenerateOrnaments.setClickingTogglesState(true);
    regenerateConnecting.onClick = [this, voiceName]() {
        const string connectingKey = audioProcessor.generator.connectingKey(voiceName);
        bool improviseConnecting = regenerateConnecting.getToggleState();
        improviseConnecting
            ? audioProcessor.loopTasks.activate({connectingKey})
            : audioProcessor.loopTasks.deactivate({connectingKey});
    };
    regenerateOrnaments.onClick = [this, voiceName]() {
        const string ornamentKey = audioProcessor.generator.ornamentsKey(voiceName);
        bool improviseOrnaments = regenerateOrnaments.getToggleState();
        improviseOrnaments
            ? audioProcessor.loopTasks.activate({ornamentKey})
            : audioProcessor.loopTasks.deactivate({ornamentKey});
    };
    addAndMakeVisible (&regenerateConnecting);
    addAndMakeVisible (&regenerateOrnaments);
}

int OrnamentationMenuComponent::placeWorkspace() {
    int xCursor = margin * 2;
    int yCursor = margin * 2;
    int yCursorReset = yCursor;
    
    int menuWidth = ui::editorWidth - margin * 2;
    int menuHeight = ui::editorHeight - margin * 2;
    workspaceHeight = menuHeight - margin * 2 - finalizeButtonHeight - ui::spaceBetweenControls * 2;
    int buttonWidth = menuWidth / 8;
    int buttonHeight = menuHeight / 6;
    int sliderHeight = workspaceHeight;
    
    connectingProbability.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    connectingAssociation.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    connectingLength.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    
    addConnectingButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + ui::spaceBetweenControls;
    muteConnectingButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + ui::spaceBetweenControls;
    yCursor = yCursorReset;
    
    buttonHeight = ui::getButtonHeight(3, workspaceHeight);
    flamButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + ui::spaceBetweenControls;
    dragButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + ui::spaceBetweenControls;
    ruffButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    xCursor += buttonWidth + ui::spaceBetweenControls;
    yCursor = yCursorReset;
    
    ornamentProbability.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    ornamentBreadth.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    
    addOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(2, workspaceHeight));
    yCursor += ui::getButtonHeight(2, workspaceHeight) + ui::spaceBetweenControls;
    muteOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(2, workspaceHeight));
    xCursor += buttonWidth + ui::spaceBetweenControls;
    yCursor = yCursorReset;
    
    regenerateConnecting.setBounds (xCursor + ui::spaceBetweenControls, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + ui::spaceBetweenControls;
    regenerateOrnaments.setBounds (xCursor + ui::spaceBetweenControls, yCursor, buttonWidth, buttonHeight);
    
    return 2 * margin + workspaceHeight + ui::spaceBetweenControls;
}

void OrnamentationMenuComponent::updateMenuState() {
    // connecting / runs
    editorState->ornamentationStates.at(voiceName).muteConnecting = muteConnectingButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).connectingProbability = connectingProbability.getValue();
    editorState->ornamentationStates.at(voiceName).connectingAssociation = connectingAssociation.getValue();
    editorState->ornamentationStates.at(voiceName).connectingLength = connectingLength.getValue();
//    editorState->ornamentationStates.at(voiceName).improviseConnecting = regenerateConnecting.getToggleState();
//    editorState->ornamentationStates.at(voiceName).improviseOrnaments = regenerateOrnaments.getToggleState();

    // ornaments
    editorState->ornamentationStates.at(voiceName).muteOrnaments = muteOrnamentsButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowflams = flamButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowDrags = dragButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowRuffs = ruffButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).ornamentProbability = ornamentProbability.getValue();
    editorState->ornamentationStates.at(voiceName).ornamentBreadth = ornamentBreadth.getValue();
}
