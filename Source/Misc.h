/*
  ==============================================================================

    Misc.h
    Created: 26 Aug 2022 1:10:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>
#include "Note.hpp"
#include "Sequence.hpp"
#include "Time/TimeTypedefs.h"

using namespace std;

// just a place to define static functions that we can eventually organize into classes
// because why just make everything a class

typedef struct Time {
    TimeSignature timeSignature;
    duration duration;
    short startTime;
    float subdivision;
    float tempo;
} Time;



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

Ornament getOrnament(OrnamentSimple ornamentSimple, float tempo);
vector<Note> placeOrnament(Note ornamented, Ornament ornament);
vector<Note> placeOrnamentSimple(Note ornamented, OrnamentSimple ornament, Time time);
Sequence ornamentSequence(Sequence sequence, vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities =  { });
Sequence ornamentSequence(Sequence sequence, OrnamentSimple ornament, float tempo);
