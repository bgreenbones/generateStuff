/*
  ==============================================================================

    Dynamics.h
    Created: 29 Nov 2022 9:41:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>
using namespace std;
class Note; // just a forward declaration

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

// shaping - want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
vector<Note> applyDynamics(vector<Note> source,
                           int originVelocity,
                           int targetVelocity);


