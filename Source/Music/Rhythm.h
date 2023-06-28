/*
  ==============================================================================

    Rhythm.h
    Created: 7 Mar 2023 8:33:49pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Phrase.hpp"
#include "VoiceParameters.h"


typedef string VoiceName;

namespace rhythm {

  // todo: maybe this go in a orchestration namespace or something
    Phrase leaveSpace(Phrase tooBusy, vector<Phrase> competingVoices);
    Phrase fillLegatoLongTones(Phrase unfilled, vector<Phrase> competingVoices = {});

    vector<Time> busySpots(vector<Phrase> competingVoices, Duration threshold = 2*sixteenths);
    vector<Time> gaps(Phrase gapFiller, vector<Phrase> competingVoices);
    
    vector<Time> doublesAndDiddles(vector<Time> t, double modifyProportion = 0.25, double doubleProportion = 0.3, double halfProportion = 0.1);
    vector<Time> onePerShortForLong(Duration longDuration, Duration shortDuration);
    vector<Time> nOfLengthM(int n, Duration m);
    vector<Time> stabilityBased(Time time, 
                                  Sequence<Duration> subdivisions, 
                                  double stabilityThreshold = 0., 
                                  Probability filter = 0.6);
    
    template <typename T>
    vector<T*> selectAtRandom(vector<T>& t, Probability prob);
    template <typename T>
    vector<vector<T*>> distinctSubsets(vector<T>& t, int n, Probability prob);
    template <typename T>
    vector<vector<T*>> distinctSubsets(vector<T>& t, Probability prob, vector<double> distribution);
    
    void multiplyTimeLength(vector<Time>& timed, vector<Time*> toMultiply, double multiplyBy);
    void repeat(vector<Time>& timed, vector<Time*> toMultiply, double multiplyBy);

    Phrase rhythmicVariation(Phrase source);

    // TODO: IDEAS:
    Phrase additive(Duration subdivision, vector<int> groupings);


    Phrase pulseAndDisplace(Phrase fromPhrase,
                            Duration pulse = 0.5, // TODO: create a rhythm type that gives access to these params RAW instead of the hardcoded cascara idea...
                              Duration displacement = 0.25,
                              Probability pDisplace = 0.5,
                              Probability pDouble = 0.75); // (for forward displacement, set pDouble = 1 and displacement = 1 - amount to displace forward)
    Phrase fillWithRolls(Phrase fromPhrase,
                         Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb);
    Phrase accents(Phrase fromPhrase);
    Phrase flip(Phrase fromPhrase);

        
    Phrase burst(Phrase fromPhrase, Timed<Note> note, int minimumRepeats, int maximumRepeats, float noteLengthInSubdivisions = 1.);


    // 0. is super not stable, 1. is super stable.
    double beatWiseStability(Position position);
    Phrase stabilityBased(Phrase fromPhrase, Probability filter = 0.8);
    Phrase stabilityFilter(Phrase fromPhrase, Direction direction = Direction::up);


    int getPotentialClaveNoteCount(Phrase fromPhrase, Duration minNoteLength, Duration maxNoteLength);
    int chooseNumberOfNotesOnLeft(double numNotes);
    
    Phrase fillCascara(Phrase fromPhrase);
    Phrase fillClave(Phrase fromPhrase,
                     int notesNeededOnLeft,
                     int notesNeededOnRight,
                     Duration minNoteLength,
                     Duration maxNoteLength);
    

    // TODO: instead of generating from a single phrase...
    // we should generate from the whole totality of phrases that currently exist. make them all available.
    Phrase randomCascara(Phrase fromPhrase,
                         Probability pDisplace = 0.5,
                         Probability pDouble = 0.75);
    Phrase randomClave(Phrase fromPhrase,
                       int minNoteLengthInSubdivisions = 2,
                       int maxNoteLengthInSubdivisions = 4);
    Phrase cascaraFrom(Phrase fromPhrase);
    Phrase claveFrom(Phrase fromPhrase,
                     int minNoteLengthInSubdivisions = 2,
                     int maxNoteLengthInSubdivisions = 4);


}




