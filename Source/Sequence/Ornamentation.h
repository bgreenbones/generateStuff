/*
  ==============================================================================

    Ornamentation.h
    Created: 28 Nov 2022 11:12:16pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "TimeSignature.h"
#include "Dynamics.h"
#include "Timing.h"

//typedef enum OrnamentPlacement {
//    leading, trailing
//} OrnamentPlacement;

typedef enum Association {
    pickup, rebound
} Association;

typedef enum OrnamentContent {
    even, buzz, ornament,
} OrnamentContent;


typedef struct Ornament { // todo: hell yea
    Placement placement;
    Griddedness griddedness;
    OrnamentContent content;
    Dynamics dynamics;
    beats length;
    unsigned short numNotes;
    
} Ornament;

typedef enum OrnamentSimple {
  flam = 1, drag = 2, ruff = 3,
  flamA, cheese,
} OrnamentSimple;

Ornament getOrnament(OrnamentSimple ornamentSimple, double tempo, double breadth);
