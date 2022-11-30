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

Position quantize(Position toQuantize, Duration quantizeGrid, Position relativeTo = 0);
