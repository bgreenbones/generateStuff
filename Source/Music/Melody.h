/*
  ==============================================================================

    Melody.h
    Created: 7 Mar 2023 8:34:19pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Phrase.hpp"
#include "GenerateStuffEditorState.h"
#include "Harmony.h"
#include "Rhythm.h"

namespace melody {
    const GenerationFunction melodyFromFunction = [](Phrase fromPhrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        Phrase phrase(fromPhrase);
        phrase.notes = fromPhrase.notes.toMonophonic();
        phrase = phrase.chordScales.empty() ? harmony::generateChordScales(phrase, editorState) : phrase;
        phrase.notes.clear();
        
        Position cursor = 0;
        Pitch lastPitch(uniformInt(55, 75));
        while (cursor < phrase.duration) {
            vector<ChordScale> chordScales = phrase.chordScales.byPosition(cursor);
            if (chordScales.empty()) {
                cursor += 1;
                continue;
            }
            
            ChordScale chordScale = chordScales.at(0);
            Tonality tonality = chordScale.scale;
            
            vector<Subdivision> subdivs = phrase.subdivisions.byPosition(cursor);
            Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
            
            Sequence<Note> burstOfNotes = Sequence<Note>::burst(phrase, subdiv, min(1 + rollDie(4), (int) ((chordScale.endTime() - cursor) / subdiv)));

            for (Note &note : burstOfNotes) {
                Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
                note.pitch = tonality.step(lastPitch, direction);
                lastPitch = note.pitch;
            }
            
            phrase.notes.insertSequence(burstOfNotes, cursor, PushBehavior::truncate);
            
            cursor += (7 + rollDie(4)) * subdiv;
        }
        
        vector<Position> startTimes;
        for (Note note : phrase.notes) {
            if (find(startTimes.begin(), startTimes.end(), note.startTime) != startTimes.end()) {
                DBG("bad");
            }
            startTimes.push_back(note.startTime);
        }

        return phrase;
    };

    const GenerationFunction melodyFunction = [](Phrase fromPhrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        return melodyFromFunction(harmony::generateChordScales(fromPhrase, editorState), editorState, voiceName);
    };

    const GenerationFunction bassFromFunction = [](Phrase const& fromPhrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        Phrase phrase(fromPhrase);
        phrase.notes = fromPhrase.notes.toMonophonic();
        phrase = phrase.chordScales.empty() ? harmony::generateChordScales(phrase, editorState) : phrase;
        phrase.notes.clear();
        
        set<Position> keyPoints { phrase.startTime };
        
        auto notes = fromPhrase.toMonophonic();
        auto accents = notes.accents();
        
        for (Note note : notes.notes) { keyPoints.emplace(note.startTime); }
        for (ChordScale tonality : phrase.chordScales) { keyPoints.emplace(tonality.startTime); }

        for (Position keyPoint : keyPoints) {
            vector<ChordScale> chordScales = phrase.chordScales.byPosition(keyPoint);
            if (chordScales.empty()) { continue; }
            ChordScale chordScale = chordScales.at(0);
            Tonality tonality = chordScale.harmony;
            
            Note noteToAdd(Pitch(tonality.root, 3), 70, chordScale.startTime, chordScale.duration);
            phrase = rhythm::burst(phrase, noteToAdd, 1, 4);
        }

        return phrase;
    };

    const GenerationFunction bassFunction = [](Phrase fromPhrase, GenerateStuffEditorState const& editorState, VoiceName voiceName) {
        return bassFromFunction(harmony::generateChordScales(fromPhrase, editorState), editorState, voiceName);
    };

}


