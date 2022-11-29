/*
  ==============================================================================

    Ornamentation.h
    Created: 28 Nov 2022 11:12:16pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "TimeSignature.h"

typedef enum Placement {
    ahead = -1, onTop = 0, behind = 1
} Placement;

typedef enum DynamicShape {
    cresc, steady, decresc
} DynamicShape;

typedef enum DynamicLevel {
    pppp, ppp, pp, p, mp, mf, f, ff, fff, ffff
} DynamicLevel;

typedef struct DynamicRange {
    DynamicLevel high;
    DynamicLevel low;
    DynamicLevel median;
} DynamicRange;

typedef struct Dynamics {
    DynamicShape shape;
    DynamicRange range;
} Dynamics;

//typedef enum OrnamentPlacement {
//    leading, trailing
//} OrnamentPlacement;

typedef enum Griddedness {
    gridFree, loose, gridded
} Griddedness;

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
