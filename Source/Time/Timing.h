/*
  ==============================================================================

    Timing.h
    Created: 29 Nov 2022 10:05:57pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"

typedef enum Placement {
    ahead = -1, onTop = 0, behind = 1
} Placement;

typedef enum Griddedness {
    gridFree, loose, gridded
} Griddedness;

// relativeTo is an offset. for example:
// to quantize notes to the second 16th of each quarter note, call:
//              quantize(toQuantize, Quarters(1), Quarters(0.25));
Position quantize(Position toQuantize, Duration quantizeGrid, Position relativeTo = 0);
