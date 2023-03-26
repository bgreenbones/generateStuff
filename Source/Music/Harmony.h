/*
  ==============================================================================

    Harmony.h
    Created: 7 Mar 2023 8:34:01pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Phrase.hpp"
#include "GenerateStuffEditorState.h"



namespace harmony {
    ChordScale randomChordScale(Position startTime = 0, Duration duration = Bars(1));
    ChordScale newChordSameScale(ChordScale previousChordScale, Position startTime, Duration duration);
    vector<Pitch> randomChord();
        
    const GenerationFunction chordsFunction = [](Phrase phrase) {
        phrase.notes.monophonic = false;
        phrase.chordScales.monophonic = true;
        phrase.notes.clear();
        phrase.chordScales.clear();
        
        int numberOfChords = phrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            ChordScale chordScale = phrase.chordScales.empty()
                ? randomChordScale(startTime, chordLength)
                : newChordSameScale(phrase.chordScales.back(), startTime, chordLength);
            phrase.chordScales.add(chordScale);
            for (Pitch pitchToAdd : chordScale.harmony.randomVoicing()) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, startTime, chordLength);
                phrase.addNote(noteToAdd);
            }
        }
        return phrase;
    };


    Phrase generateChordScales(Phrase fromPhrase, Probability chordProbabilityPerAccent = 0.6);


    const GenerationFunction chordsFromFunction = [](Phrase fromPhrase) {
        Phrase phrase = fromPhrase.toPolyphonic();
        phrase = phrase.chordScales.empty() ? generateChordScales(phrase, 0.6) : phrase;
        phrase.notes.clear();
        
        for (ChordScale chordScale : phrase.chordScales) {
            for (Pitch pitchToAdd : chordScale.harmony.randomVoicing()) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, chordScale.startTime, chordScale.duration);
                phrase.addNote(noteToAdd);
            }
        }
        
        return phrase;
    };
}

