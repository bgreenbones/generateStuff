/*
  ==============================================================================

    Dynamics.h
    Created: 29 Nov 2022 9:41:41pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once

#include "TimedEvent.h"
#include "Duration.h"
#include "Random.h"
#include <vector>

using std::vector;

class Note;

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

struct Dynamics {
    DynamicShape shape;
    DynamicRange range;
};


namespace dynamics {
    // todo: want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
    vector<Timed<Note>>& shape(vector<Timed<Note>>& source, int originVelocity, int targetVelocity);
    vector<Timed<Note>>& shape(vector<Timed<Note>>& source, Dynamics dynamics = {
        .range = {
          .high = draw<DynamicLevel>({ff, fff, ffff}),
          .median = draw<DynamicLevel>({p, mp, mf, f}),
          .low = draw<DynamicLevel>({pp, ppp, pppp}),
        },
        .shape = draw<DynamicShape>({cresc, decresc, steady})
      });
    vector<Timed<Note>>& stretch(vector<Timed<Note>> &source, DynamicRange targetRange);
    vector<Timed<Note>>& stretch(vector<Timed<Note>> &source, DynamicLevel targetMinimum, DynamicLevel targetMaximum);
    vector<Timed<Note>>& randomFlux(vector<Timed<Note>> &source, double minScale = 0.7, double maxScale = 1.3);
    vector<Timed<Note>>& randomAccents(vector<Timed<Note>> &source, DynamicLevel low, DynamicLevel high);
    vector<Timed<Note>>& randomAccents(vector<Timed<Note>> &source
                                , DynamicLevel high);
    vector<Timed<Note>>& followAccents(vector<Timed<Note>> &source, vector<Position> accents, DynamicLevel low, DynamicLevel high);
}



