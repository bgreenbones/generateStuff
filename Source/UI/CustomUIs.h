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


static const int editorWidth = 1200;
static const int editorHeight = 500;
static const int spaceBetweenControls = 10;

class PopUpComponent : public juce::Component
{
    int margin = 20;
public:
    PopUpComponent()
    {
        setSize(editorWidth, editorHeight);
        addAndMakeVisible (okButton);
        addAndMakeVisible (cancelButton);
        
        okButton.onClick = [this]() {
            // TODO: save settings;
            delete this;
        };
        cancelButton.onClick = [this]() { delete this; };
    }
    
    ~PopUpComponent() { }
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
    void resized() override
    {
        
        int cursorX = margin * 2;
        int cursorY = margin * 2;
        
        int finalizeButtonWidth = 150;
        int finalizeButtonHeight = 30;
        
//        okButton.setOpaque(true);
//        okButton.setAlpha(1.0);
    
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


class TransformPhraseMenuComponent : public PopUpComponent {};
class VoiceSettingsMenuComponent : public PopUpComponent {};


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
