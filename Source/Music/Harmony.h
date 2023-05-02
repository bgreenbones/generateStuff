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
#include "VoiceParameters.h"


namespace harmony {
    ChordScale randomChordScale(Position startTime = 0, Duration duration = Bars(1));
    ChordScale newChordSameScale(ChordScale previousChordScale, Position startTime, Duration duration);
    ChordScale subtleModulations(ChordScale previousChordScale, Position startTime, Duration duration);
    
    vector<Pitch> randomChord();
        
    ChordScale selectApproachAndGenerate(juce::String approach, Sequence<ChordScale> chordScales, Position startTime, Duration chordLength);

    const GenerationFunction chordsFunction = [](Phrase phrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        phrase.notes.monophonic = false;
        phrase.chordScales.monophonic = true;
        phrase.notes.clear();
        phrase.chordScales.clear();
        
        juce::String harmonyApproach = editorState.getChoiceValue(harmonyApproachKey);
        
        int numberOfChords = phrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords, 1));
            ChordScale chordScale = selectApproachAndGenerate(harmonyApproach, phrase.chordScales, startTime, chordLength);
            phrase.chordScales.add(chordScale);
            for (Pitch pitchToAdd : chordScale.harmony.randomVoicing()) {
                Note noteToAdd(pitchToAdd.pitchValue, 70, startTime, chordLength);
                phrase.addNote(noteToAdd);
            }
        }
        // phrase.chordScales.tie();
        return phrase;
    };


    Phrase generateChordScales(Phrase fromPhrase, GenerateStuffEditorState const& editorState);


    const GenerationFunction chordsFromFunction = [](Phrase fromPhrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        Phrase phrase = fromPhrase.toPolyphonic();
        phrase = phrase.chordScales.empty() ? generateChordScales(phrase, editorState) : phrase;
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

