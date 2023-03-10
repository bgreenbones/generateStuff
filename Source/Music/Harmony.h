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
    Tonality randomTonality(Position startTime = 0, Duration duration = Bars(1));
    vector<Pitch> randomChord();
        
    const GenerationFunction chordsFunction = [](Phrase phrase) {
        phrase.notes.monophonic = false;
        phrase.tonalities.monophonic = true;
        phrase.notes.clear();
        phrase.tonalities.clear();
        
        int numberOfChords = phrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            Tonality tonality = randomTonality(startTime, chordLength);
            phrase.tonalities.add(tonality);
            for (Pitch pitchToAdd : tonality.randomVoicing()) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, startTime, chordLength);
                phrase.addNote(noteToAdd);
            }
        }
        return phrase;
    };


    Phrase generateTonalities(Phrase fromPhrase, Probability chordProbabilityPerAccent = 0.6);


    const GenerationFunction chordsFromFunction = [](Phrase fromPhrase) {
        Phrase phrase = fromPhrase.toPolyphonic();
        phrase = phrase.tonalities.empty() ? generateTonalities(phrase, 0.6) : phrase;
        phrase.notes.clear();
        
        for (Tonality tonality : phrase.tonalities) {
            for (Pitch pitchToAdd : tonality.randomVoicing()) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, tonality.startTime, tonality.duration);
                phrase.addNote(noteToAdd);
            }
        }
        
        return phrase;
    };
}

