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


static vector<Pitch> randomChord() {
    PitchClass root = draw<PitchClass>(pitches);
    Interval third = draw<Interval>({ m3, M3 });
    Interval fifth = P5; // draw<Interval>({ b5, P5 });
    Interval seventh = draw<Interval>({ m7, M7 });
    Tonality harm;
    harm.root = root;
    harm.intervalsUpFromRoot = { unison, third, fifth, seventh };
    return harm.randomVoicing();
}

static const GenerationFunction chordsFunction = [](Phrase phrase) {
    int numberOfChords = phrase.duration.asBars();
    vector<vector<Pitch>> chords;
    while(numberOfChords-- > 0) { chords.push_back(randomChord()); }
    
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
//
static const GenerationFunction chordsFromFunction = [](Phrase phrase) {
//Phrase chordsFrom(string phraseKey) {
//    Position startTime = editorState->getStartTime();
//    Duration phraseLength = editorState->getPhraseLength();
//    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) { this->generate(phraseKey); }
//    Voice voice = playQueue->getVoice(phraseKey);
//
//    Phrase phrase = Phrase(editorState->getSubdivision(),
//                           startTime,
//                           phraseLength).toPolyphonic();
//
//
//    Sequence<Note> notes = voice.base.notes.toMonophonic();
//    Sequence<Note> accents(notes);
//    accents.clear();
//
//    copy_if(notes.begin(), notes.end(), back_inserter(accents), [](Note note) { return note.accented; });
//    accents.tie();
//
//    for (Note accent : accents) {
//        if (Probability(0.6)) { // TODO: parameterize this
//            vector<Pitch> chord = randomChord();
//            for (Pitch pitchToAdd : chord) {
//                Note noteToAdd(pitchToAdd.pitchValue, 70, accent.startTime, accent.duration);
//                phrase.addNote(noteToAdd);
//            }
//        }
//    }
//
//    playQueue->queuePhrase(harmonyKey, phrase);
    return phrase;
};
