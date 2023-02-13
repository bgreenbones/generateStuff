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
