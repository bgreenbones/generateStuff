/*
  ==============================================================================

    GenerateStuffEditorState.h
    Created: 12 Feb 2023 10:25:30pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"

class GenerateStuffEditorState {
public:
    // general
    beats subdivision = 1./2.;
    bars phraseLengthBars = 2.;
    beats phraseLengthBeats = 0.;
    beats displace = 0.;
    bars startBar = 1.;
    bars stopBar = 3.;

    double probabilityOfDouble; // not yet using for 'cascara' abstraction

    // rolls / runs
    bool muteRolls = false;
    bool muteOrnaments = false;
    double rollProbability = 0.5;
    double rollAssociation = 0.5;
    double rollLength = 0.5;
    bool improviseRolls = false;
    bool improviseOrnaments = false;
    
    // ornaments
    bool allowflams = true;
    bool allowDrags = false;
    bool allowRuffs = false;
    double ornamentProbability = 0.5;
    double ornamentBreadth = 0.5;
    
    Bars getStartTime() { return Bars(startBar - 1., true); }
    Bars getStopTime() { return Bars(startBar - 1., true); }
    
    Duration getPhraseLength() {
        Duration result = Bars(phraseLengthBars, true) + Beats(phraseLengthBeats, true);
        return result;
    }
    
    Subdivision getSubdivision() {
        return Subdivision(Beats(subdivision), getStartTime(), getPhraseLength());
    }
    
    Beats getDisplacement() { return Beats(displace, true); }
    
};