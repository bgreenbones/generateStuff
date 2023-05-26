/*
  ==============================================================================

    Dynamics.h
    Created: 29 Nov 2022 9:41:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"
#include <vector>
using namespace std;
class Note; // just a forward declaration


typedef enum DynamicShape {
    cresc, steady, decresc
} DynamicShape;

typedef enum DynamicLevel {
    minPiano = 1,
    ppppp = 10,
    pppp = 20,
    ppp = 30,
    pp = 40,
    p = 50,
    mp = 60,
    mf = 70,
    f = 80,
    ff = 90,
    fff = 100,
    ffff = 110,
    fffff = 120,
    maxForte = 127
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


namespace dynamics {
    // todo: want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
    vector<Note>& shape(vector<Note>& source, int originVelocity, int targetVelocity);
    vector<Note>& shape(vector<Note>& source, Dynamics dynamics);
    vector<Note>& stretch(vector<Note> &source, DynamicRange targetRange);
    vector<Note>& stretch(vector<Note> &source, DynamicLevel targetMinimum, DynamicLevel targetMaximum);
    vector<Note>& randomFlux(vector<Note> &source, double minScale = 0.7, double maxScale = 1.3);
    vector<Note>& randomAccents(vector<Note> &source, DynamicLevel low, DynamicLevel high);
    vector<Note>& followAccents(vector<Note> &source, vector<Position> accents, DynamicLevel low, DynamicLevel high);
}



