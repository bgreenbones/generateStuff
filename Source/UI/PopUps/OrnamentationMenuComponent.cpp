/*
  ==============================================================================

    OrnamentationMenuComponent.cpp
    Created: 29 Mar 2023 12:14:45am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "OrnamentationMenuComponent.h"



OrnamentationMenuComponent::OrnamentationMenuComponent(VoiceName voiceName,
                                                           shared_ptr<GenerateStuffEditorState> editorState):
                                                                VoiceEditor(voiceName, editorState)
{
    if (editorState->ornamentationStates.find(voiceName) == editorState->ornamentationStates.end()) {
        editorState->ornamentationStates.emplace(voiceName, OrnamentationEditorState());
    }
                                                                    
    addRollsButton.onClick = [this]() {
        double rollProb = Probability(rollProbability.getValue());
        double associationProb = Probability(rollAssociation.getValue());
        double rollLengthProb = Probability(rollLength.getValue());

//            generator.roll(voiceManager.selectedPhraseKeyState, rollProb, associationProb, rollLengthProb);
//            string id = generator.rollsKey(voiceManager.selectedPhraseKeyState);
//            function<void()> task = [=]() { generator.roll(voiceManager.selectedPhraseKeyState, rollProb, associationProb, rollLengthProb); };
//            audioProcessor.loopTasks.queue(id, task, regenerateRolls.getToggleState());
    };
    addAndMakeVisible (&addRollsButton);
    
//        clearRollsButton.onClick = [this]() {
//            string rollsKey = generator.rollsKey(voiceManager.selectedPhraseKeyState);
//            playQueue->toggleMuteRolls(rollsKey);
//        };

    addAndMakeVisible(&clearRollsButton);
//        clearOrnamentsButton.onClick = [this]() {
//            string ornamentsKey = generator.ornamentsKey(voiceManager.selectedPhraseKeyState);
//            playQueue->toggleMuteOrnamentation(ornamentsKey);
//        };
    addAndMakeVisible(&clearOrnamentsButton);
    
    addAndMakeVisible(&rollProbability);
    rollProbability.setSliderStyle (juce::Slider::LinearBarVertical);
    rollProbability.setRange (0.0, 1.0, 0.01);
    rollProbability.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollProbability.setPopupDisplayEnabled (true, false, this);
    rollProbability.setTextValueSuffix (" p(roll)");
    rollProbability.setValue(1.0);

    addAndMakeVisible(&rollAssociation);
    rollAssociation.setSliderStyle (juce::Slider::LinearBarVertical);
    rollAssociation.setRange (0.0, 1.0, 0.01);
    rollAssociation.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollAssociation.setPopupDisplayEnabled (true, false, this);
    rollAssociation.setTextValueSuffix (" roll association / swing");
    rollAssociation.setValue(0.5);

    addAndMakeVisible(&rollLength);
    rollLength.setSliderStyle (juce::Slider::LinearBarVertical);
    rollLength.setRange (0.0, 1.0, 0.01);
    rollLength.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    rollLength.setPopupDisplayEnabled (true, false, this);
    rollLength.setTextValueSuffix (" roll length");
    rollLength.setValue(0.5);

    flamButton.setClickingTogglesState(true);
    flamButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&flamButton);
    dragButton.setClickingTogglesState(true);
    dragButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&dragButton);
    ruffButton.setClickingTogglesState(true);
    ruffButton.setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(&ruffButton);
    addOrnamentsButton.onClick = [this]() {
        Probability prob = ornamentProbability.getValue();
        double breadth = ornamentBreadth.getValue();
        bool flams = flamButton.getToggleState();
        bool drags = dragButton.getToggleState();
        bool ruffs = ruffButton.getToggleState();

//            generator.ornament(voiceManager.selectedPhraseKeyState, prob, breadth, flams, drags, ruffs);
//
//            string id = generator.ornamentsKey(voiceManager.selectedPhraseKeyState);
//            function<void()> task = [=]() { generator.ornament(voiceManager.selectedPhraseKeyState, prob, breadth, flams, drags, ruffs); };
//            audioProcessor.loopTasks.queue(id, task, regenerateOrnaments.getToggleState());
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
    
    rollProbability.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    rollAssociation.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    rollLength.setBounds (xCursor, yCursor, ui::sliderWidth, sliderHeight);
    xCursor += ui::sliderWidth + ui::spaceBetweenControls;
    
    addRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
    yCursor += buttonHeight + ui::spaceBetweenControls;
    clearRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
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
    clearOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, ui::getButtonHeight(2, workspaceHeight));
    xCursor += buttonWidth + ui::spaceBetweenControls;
    yCursor = yCursorReset;
    
    return 2 * margin + workspaceHeight + ui::spaceBetweenControls;
}

void OrnamentationMenuComponent::updateMenuState() {
    // rolls / runs
    editorState->ornamentationStates.at(voiceName).muteRolls = clearRollsButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).rollProbability = rollProbability.getValue();
    editorState->ornamentationStates.at(voiceName).rollAssociation = rollAssociation.getValue();
    editorState->ornamentationStates.at(voiceName).rollLength = rollLength.getValue();
//    editorState->ornamentationStates.at(voiceName).improviseRolls = regenerateRolls.getToggleState();
//    editorState->ornamentationStates.at(voiceName).improviseOrnaments = regenerateOrnaments.getToggleState();

    // ornaments
    editorState->ornamentationStates.at(voiceName).muteOrnaments = clearOrnamentsButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowflams = flamButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowDrags = dragButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).allowRuffs = ruffButton.getToggleState();
    editorState->ornamentationStates.at(voiceName).ornamentProbability = ornamentProbability.getValue();
    editorState->ornamentationStates.at(voiceName).ornamentBreadth = ornamentBreadth.getValue();
}
