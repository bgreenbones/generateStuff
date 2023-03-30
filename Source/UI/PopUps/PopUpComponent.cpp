/*
  ==============================================================================

    PopUpComponent.cpp
    Created: 29 Mar 2023 12:13:06am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "PopUpComponent.h"


PopUpComponent::PopUpComponent()
{
    this->setBounds(0, 0, editorWidth, editorHeight);
    setSize(editorWidth, editorHeight);
    addAndMakeVisible (okButton);
    addAndMakeVisible (cancelButton);
    
    okButton.onClick = [this]() {
        updateMenuState();
        delete this;
    };
    cancelButton.onClick = [this]() { delete this; };
}

PopUpComponent::~PopUpComponent()
{
    
}

void PopUpComponent::paint (juce::Graphics& g)
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

int PopUpComponent::placeWorkspace() { return margin * 2; }

void PopUpComponent::resized()
{
//        int cursorX = margin * 2;
//        int cursorY = margin * 2;
    int cursorY = placeWorkspace();
    drawFinalizeButtons(cursorY);
}

void PopUpComponent::drawFinalizeButtons(int cursorY) {
    int okButtonX = editorWidth / 2 - finalizeButtonWidth  - spaceBetweenControls / 2;
    int cancelButtonX = editorWidth / 2 + spaceBetweenControls / 2;
    okButton.setBounds (okButtonX, cursorY, finalizeButtonWidth, finalizeButtonHeight);
    cancelButton.setBounds (cancelButtonX, cursorY, finalizeButtonWidth, finalizeButtonHeight);
}
