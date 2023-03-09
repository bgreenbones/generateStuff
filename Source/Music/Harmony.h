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
    Tonality randomTonality();
    vector<Pitch> randomChord();
        
    const GenerationFunction chordsFunction = [](Phrase phrase) {
        int numberOfChords = phrase.duration.asBars();
        vector<vector<Pitch>> chords;
        while(numberOfChords-- > 0) { chords.push_back(harmony::randomChord()); }

        phrase.notes.monophonic = false;
        numberOfChords = phrase.duration.asBars();
        bars startTimeInBars = 0;
        for (vector<Pitch> chordToAdd : chords) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            for (Pitch pitchToAdd : chordToAdd) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, startTime, chordLength);
                phrase.addNote(noteToAdd);
            }
        }
        return phrase;
    };

    const GenerationFunction chordsFromFunction = [](Phrase fromPhrase) {

        Phrase phrase = fromPhrase.toPolyphonic();
        phrase.notes.clear();
        Sequence<Note> notes(fromPhrase.notes.toMonophonic());
        Sequence<Note> accents(notes);
        accents.clear();

        copy_if(notes.begin(), notes.end(), back_inserter(accents), [](Note note) { return note.accented; });
        accents.tie();

        for (Note accent : accents) {
            if (Probability(0.6)) { // TODO: parameterize this
                vector<Pitch> chord = harmony::randomChord();
                for (Pitch pitchToAdd : chord) {
                    Note noteToAdd(pitchToAdd.pitchValue, 70, accent.startTime, accent.duration);
                    phrase.addNote(noteToAdd);
                }
            }
        }

        return phrase;
    };

}

