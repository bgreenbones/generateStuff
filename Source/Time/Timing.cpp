/*
  ==============================================================================

    Timing.cpp
    Created: 29 Nov 2022 10:05:57pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Timing.h"

Position quantize(Position toQuantize, Duration quantizeGrid, Position relativeTo) {
//    Position smaller = toQuantize < relativeTo ? toQuantize : relativeTo;
    double temp = toQuantize.asQuarters() - relativeTo.asQuarters();
//    double mod = std::fmod(temp, quantizeGrid.asQuarters());
    double mod = (toQuantize - relativeTo) % quantizeGrid;
    int roundDirection = (mod <= (quantizeGrid.asQuarters() * 0.5)) ? -1 : 1;
    double quantized = temp + roundDirection * mod;
    return quantized + relativeTo;
}
