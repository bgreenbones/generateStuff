/*
  ==============================================================================

    Transport.h
    Created: 28 Aug 2022 8:17:32pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

// basic units
typedef double beats; // depends on time signature
typedef double bars; // depends on time signature
typedef double quarters; // always measured in quarters

// --- positions in time
typedef struct position {
    int bar;
    beats beat;
} position;

typedef float tempo;

typedef struct duration {
    bars bars;
    beats beats;
} duration;


typedef struct section {
    position start; // required
    position end; // only one of these two
    duration duration;
} section;

typedef struct ppqSection {
    quarters ppqStart; // or this required
    quarters ppqEnd; // only one of these two
    quarters ppqDuration;
} ppqSection;
