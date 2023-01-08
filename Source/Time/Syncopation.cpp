/*
  ==============================================================================

    Syncopation.cpp
    Created: 10 Oct 2022 9:58:30pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Syncopation.h"
#include "Timing.h"


Position Syncopation::getPlacement(Position start, Position target) {
    Position centerBetweenNotes = (start + target) / 2.;
    double syncopationOffset = this->type * this->amount * (target - centerBetweenNotes);
    return centerBetweenNotes + syncopationOffset;
}

Position Syncopation::getPlacement(Position start, Position target, Subdivision subdivision) {
    return quantize(getPlacement(start, target), subdivision, target);
}


