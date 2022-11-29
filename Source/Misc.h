/*
  ==============================================================================

    Misc.h
    Created: 26 Aug 2022 1:10:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>
#include "Ornamentation.h"
#include "Note.hpp"



using namespace std;

// just a place to define static functions and play around with things
// before working them into more final designs


//
//typedef enum Placement {
//    ahead = -1, onTop = 0, behind = 1
//} Placement;
//
//typedef enum DynamicShape {
//    cresc, steady, decresc
//} DynamicShape;
//
//typedef enum DynamicLevel {
//    pppp, ppp, pp, p, mp, mf, f, ff, fff, ffff
//} DynamicLevel;
//
//typedef struct DynamicRange {
//    DynamicLevel high;
//    DynamicLevel low;
//    DynamicLevel median;
//} DynamicRange;
//
//typedef struct Dynamics {
//    DynamicShape shape;
//    DynamicRange range;
//} Dynamics;
//
////typedef enum OrnamentPlacement {
////    leading, trailing
////} OrnamentPlacement;
//
//typedef enum Griddedness {
//    gridFree, loose, gridded
//} Griddedness;
//
//typedef enum OrnamentContent {
//    even, buzz, ornament,
//} OrnamentContent;
//
//
//typedef struct Ornament { // todo: hell yea
//    Placement placement;
//    Griddedness griddedness;
//    OrnamentContent content;
//    Dynamics dynamics;
//    beats length;
//    unsigned short numNotes;
//    
//} Ornament;
//
//typedef enum OrnamentSimple {
//  flam = 1, drag = 2, ruff = 3,
//  flamA, cheese,
//} OrnamentSimple;
//
//Ornament getOrnament(OrnamentSimple ornamentSimple, double tempo, double breadth);
//vector<Note> placeOrnament(Note ornamented, Ornament ornament);
//vector<Note> placeOrnamentSimple(Note ornamented, OrnamentSimple ornament, double breadth);
//Phrase ornamentPhrase(Phrase phrase, vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities =  { });
//Phrase ornamentPhrase(Phrase phrase, OrnamentSimple ornament, float tempo);

Position quantize(Position toQuantize, Duration quantizeGrid, Position relativeTo = 0);

typedef enum Association {
    pickup, rebound
} Association;




// shaping - want to be generic over pitch, velocity, pressure, generic CC, uhhhh note length, uh,, other stuff, idk
vector<Note> applyDynamics(vector<Note> source,
                           int originVelocity,
                           int targetVelocity);


