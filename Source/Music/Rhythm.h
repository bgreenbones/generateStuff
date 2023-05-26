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
    Phrase rhythmicVariation(Voice const& voice);

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

        
    Phrase burst(Phrase fromPhrase, Note note, int minimumRepeats, int maximumRepeats, float noteLengthInSubdivisions = 1.);


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

    const GenerationFunction fillSubdivisionsFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
        Phrase newPhrase(phrase);
        newPhrase.notes.clear();
        newPhrase = newPhrase.randomGhostSubdivision(0.9,1.);
        dynamics::randomFlux(newPhrase.notes);
        return newPhrase;
    };

    const GenerationFunction cascaraFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
        float pDisplace = editorState.getKnobValue(cascaraDisplaceProbabilityKey);
        float pDouble = editorState.getKnobValue(cascaraDoubleProbabilityKey);
        return randomCascara(phrase, pDisplace, pDouble);
    };
    const GenerationFunction claveFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
        int minNoteLengthInSubdivisions = editorState.getKnobValue(claveMinNoteLengthKey);
        int maxNoteLengthInSubdivisions = editorState.getKnobValue(claveMaxNoteLengthKey);
        return randomClave(phrase, minNoteLengthInSubdivisions, maxNoteLengthInSubdivisions);
    };
    const GenerationFunction cascaraFromFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
        return cascaraFrom(phrase);
    };
    const GenerationFunction claveFromFunction = [](Phrase phrase, PlayQueue& playQueue, GenerateStuffEditorState const& editorState) {
        int minNoteLengthInSubdivisions = editorState.getKnobValue(claveMinNoteLengthKey);
        int maxNoteLengthInSubdivisions = editorState.getKnobValue(claveMaxNoteLengthKey);
        return claveFrom(phrase, minNoteLengthInSubdivisions, maxNoteLengthInSubdivisions);
    };
}




