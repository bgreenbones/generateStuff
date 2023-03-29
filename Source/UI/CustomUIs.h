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


static const int editorWidth = 900;
static const int editorHeight = 500;
static const int sliderWidth = 20;
static const int spaceBetweenControls = 10;

static const int getButtonHeight(int rows, int height) {
    return (height - spaceBetweenControls * (rows - 1)) / rows;
};

class PopUpComponent : public juce::Component
{
protected:
    int margin = 20;
    int finalizeButtonWidth = 150;
    int finalizeButtonHeight = 30;
    int workspaceHeight = 0;
public:
    PopUpComponent()
    {
        this->setBounds(0, 0, editorWidth, editorHeight);
        setSize(editorWidth, editorHeight);
        addAndMakeVisible (okButton);
        addAndMakeVisible (cancelButton);
        
        okButton.onClick = [this]() {
            // TODO: save settings;
            delete this;
        };
        cancelButton.onClick = [this]() { delete this; };
    }
    
    ~PopUpComponent() override { }
    void paint (juce::Graphics& g) override
    {
        g.setColour (juce::Colours::darkolivegreen); // TODO: choose a good color lol
        g.setOpacity(0.25);
        g.fillAll();

        int cornerSize = 10;
        g.setColour (juce::Colours::black);
        g.setOpacity(1.);
        g.drawRoundedRectangle(margin, margin, editorWidth - 2 * margin, editorHeight - 2 * margin, cornerSize, 1.0);
        
        g.setColour (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        g.setOpacity(0.95);
        g.fillRoundedRectangle(margin, margin, editorWidth - 2 * margin, editorHeight - 2 * margin, cornerSize);
        
        g.setOpacity(1.);
    }
    
    virtual int placeWorkspace() { return margin * 2; }
    void resized() override
    {
//        int cursorX = margin * 2;
//        int cursorY = margin * 2;
        int cursorY = placeWorkspace();
        drawFinalizeButtons(cursorY);
    }
    
    void drawFinalizeButtons(int cursorY) {
        int okButtonX = editorWidth / 2 - finalizeButtonWidth  - spaceBetweenControls / 2;
        int cancelButtonX = editorWidth / 2 + spaceBetweenControls / 2;
        okButton.setBounds (okButtonX, cursorY, finalizeButtonWidth, finalizeButtonHeight);
        cancelButton.setBounds (cancelButtonX, cursorY, finalizeButtonWidth, finalizeButtonHeight);
    }
    
    
    
private:
    juce::TextButton okButton { "ok" };
    juce::TextButton cancelButton { "cancel" };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopUpComponent)
};


class TransformPhraseMenuComponent : public PopUpComponent {
    juce::TextButton addRollsButton { "rolls" };
    juce::TextButton clearRollsButton { "clearRolls" };
    juce::Slider rollProbability;
    juce::Slider rollAssociation;
    juce::Slider rollLength;
    juce::TextButton addOrnamentsButton { "orns" };
    juce::TextButton clearOrnamentsButton { "clear orns" };
    juce::TextButton flamButton { "flam" };
    juce::TextButton dragButton { "drag" };
    juce::TextButton ruffButton { "ruff" };
    juce::Slider ornamentProbability;
    juce::Slider ornamentBreadth;
    juce::TextButton flipButton { "flip" };
public:
    
    TransformPhraseMenuComponent() {
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
    
        flipButton.onClick = [this]() {
//            generator.flipClave(voiceManager.selectedPhraseKeyState);
        };
        addAndMakeVisible(&flipButton);

    }
    
//    void resized() override {
    int placeWorkspace() override {
        int xCursor = margin * 2;
        int yCursor = margin * 2;
        int yCursorReset = yCursor;
        
        int menuWidth = editorWidth - margin * 2;
        int menuHeight = editorHeight - margin * 2;
        workspaceHeight = menuHeight - margin * 2 - finalizeButtonHeight - spaceBetweenControls * 2;
        int buttonWidth = menuWidth / 8;
        int buttonHeight = menuHeight / 6;
        int sliderHeight = workspaceHeight;
        
        flipButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(1, workspaceHeight));
        xCursor += buttonWidth + spaceBetweenControls;
        
        rollProbability.setBounds (xCursor, yCursor, sliderWidth, sliderHeight);
        xCursor += sliderWidth + spaceBetweenControls;
        rollAssociation.setBounds (xCursor, yCursor, sliderWidth, sliderHeight);
        xCursor += sliderWidth + spaceBetweenControls;
        rollLength.setBounds (xCursor, yCursor, sliderWidth, sliderHeight);
        xCursor += sliderWidth + spaceBetweenControls;
        
        addRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        yCursor += buttonHeight + spaceBetweenControls;
        clearRollsButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        xCursor += buttonWidth + spaceBetweenControls;
        yCursor = yCursorReset;
        
        buttonHeight = getButtonHeight(3, workspaceHeight);
        flamButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        yCursor += buttonHeight + spaceBetweenControls;
        dragButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        yCursor += buttonHeight + spaceBetweenControls;
        ruffButton.setBounds (xCursor, yCursor, buttonWidth, buttonHeight);
        xCursor += buttonWidth + spaceBetweenControls;
        yCursor = yCursorReset;
        
        ornamentProbability.setBounds (xCursor, yCursor, sliderWidth, sliderHeight);
        xCursor += sliderWidth + spaceBetweenControls;
        ornamentBreadth.setBounds (xCursor, yCursor, sliderWidth, sliderHeight);
        xCursor += sliderWidth + spaceBetweenControls;
        
        addOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(2, workspaceHeight));
        yCursor += getButtonHeight(2, workspaceHeight) + spaceBetweenControls;
        clearOrnamentsButton.setBounds (xCursor, yCursor, buttonWidth, getButtonHeight(2, workspaceHeight));
        xCursor += buttonWidth + spaceBetweenControls;
        yCursor = yCursorReset;
        
        return 2 * margin + workspaceHeight + spaceBetweenControls;
    }
    
    void updateMenuState() {
//        // rolls / runs
//        editorState->muteRolls = clearRollsButton.getToggleState();
//        editorState->rollProbability = rollProbability.getValue();
//        editorState->rollAssociation = rollAssociation.getValue();
//        editorState->rollLength = rollLength.getValue();
//        editorState->improviseRolls = regenerateRolls.getToggleState();
//        editorState->improviseOrnaments = regenerateOrnaments.getToggleState();
//
//        // ornaments
//        editorState->muteOrnaments = clearOrnamentsButton.getToggleState();
//        editorState->allowflams = flamButton.getToggleState();
//        editorState->allowDrags = dragButton.getToggleState();
//        editorState->allowRuffs = ruffButton.getToggleState();
//        editorState->ornamentProbability = ornamentProbability.getValue();
//        editorState->ornamentBreadth = ornamentBreadth.getValue();
    }
};
class VoiceSettingsMenuComponent : public PopUpComponent {
public:
    VoiceSettingsMenuComponent() {}
    int placeWorkspace() override
    {
        int cursorY = margin * 2;
        return cursorY;
    }
};


/*
 ### Place for trying UI generalization stuff before moving out into their own files.
 
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
