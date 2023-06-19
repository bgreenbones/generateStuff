/*
  ==============================================================================

    ChordScale.h
    Created: 19 Mar 2023 5:09:13pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Tonality.h"
#include "TimedEvent.h"

class ChordScale {
public:
    Tonality scale;
    Tonality harmony;

    ChordScale(Tonality scale, Tonality harmony);
    ChordScale(Tonality scale);
    ChordScale();
    
public:
    bool operator==(ChordScale const & other) const;
};
