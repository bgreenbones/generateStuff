/*
  ==============================================================================

    Syncopation.h
    Created: 10 Oct 2022 9:58:30pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"
#include "Subdivision.h"

// TODO: this should be its own roll-to-target-note function? we can make different kinds of fills and have fill-in-gaps select them randomly
// options:
// XxxxxxxxX
// X...xxxxX = syncoptation == even
// X.....xxX = syncopation == swing (higher value means starts later)
// X.xxxxxxX = syncopation == wonk (higher value means starts earlier)
// syncopation amount: 0-1 - 0 results in even, 1 results in no filler notes.
typedef enum SyncopationType {
    wonk = -1, straight = 0, swing = 1,
} SyncopationType;

class Syncopation {
public:
    SyncopationType type;
    double amount; // values 0 - 1, please
    
    Syncopation(SyncopationType type, double amount): type(type), amount(amount) {}
    Position getPlacement(Position start, Position target);
    Position getPlacement(Position start, Position target, Subdivision subdivision);
};
