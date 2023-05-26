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
    const GenerationFunction melodyFromFunction = [](Phrase fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) {
        Phrase phrase(fromPhrase);
        phrase.notes = fromPhrase.notes.toMonophonic();
        phrase = phrase.chordScales.empty() ? harmony::generateChordScales(phrase, playQueue, editorState) : phrase;
        phrase.notes.clear();
        
        Position cursor = 0;
        Pitch lastPitch(uniformInt(55, 75));
        while (cursor < phrase.getDuration()) {
            vector<ChordScale> chordScales = phrase.chordScales.byPosition(cursor);
            if (chordScales.empty()) {
                cursor += 1;
                continue;
            }
            
            ChordScale chordScale = chordScales.at(0);
            Tonality tonality = chordScale.scale;
            
            vector<Subdivision> subdivs = phrase.subdivisions.byPosition(cursor);
            Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
            
            // vector<Note> burstOfNotes = Sequence<Note>::burst(subdiv, min(1 + rollDie(4), (int) ((chordScale.endTime() - cursor) / subdiv)));
            vector<Note> burstOfNotes = Sequence<Note>::burst(subdiv, 1 + rollDie(flipCoin() ? 3 : 7));

            // fun with durations
            for (auto noteIter = burstOfNotes.begin(); noteIter < burstOfNotes.end(); noteIter++) {
                Note& note = *noteIter;
                
                if (flipWeightedCoin(0.75)) {
                    continue;
                }
                
                if (flipCoin()) {
                    note.duration = 2 * note.duration;
                    if (noteIter + 1 == burstOfNotes.end()) { continue; }
                    else if ((noteIter + 1)->startTime < note.endTime()) {
                        burstOfNotes.erase(noteIter + 1);
                    }
                } else {
                    note.duration = 0.5 * note.duration;
                    Note doubleNote(note);
                    doubleNote.startTime = note.endTime();
                    burstOfNotes.insert(noteIter + 1, doubleNote);
                }
            }

            for (Note &note : burstOfNotes) {
                Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
                note.pitch = phrase.chordScales.drawByPosition(note.startTime).scale
                    .step(lastPitch, direction);
                lastPitch = note.pitch;
            }
            
            phrase.notes.insertVector(burstOfNotes, cursor, PushBehavior::truncate);
            
            cursor += (7 + rollDie(4)) * subdiv;
        }
        
        // This was a check for monophonicity... don't think we need it.
        // vector<Position> startTimes;
        // for (Note note : phrase.notes) {
        //     if (find(startTimes.begin(), startTimes.end(), note.startTime) != startTimes.end()) {
        //         DBG("bad");
        //     }
        //     startTimes.push_back(note.startTime);
        // }

        dynamics::randomAccents(phrase.notes, mp, ff);

        return phrase;
    };

    const GenerationFunction melodyFunction = [](Phrase fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) {
        return melodyFromFunction(harmony::generateChordScales(fromPhrase, playQueue, editorState), playQueue, editorState);
    };

    // const GenerationFunction bassFromFunction = [](Phrase const& fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) {
    Phrase bassFromFunction(Phrase fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState);
    const GenerationFunction bassFunction = [](Phrase fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) {
        return bassFromFunction(harmony::generateChordScales(fromPhrase, playQueue, editorState), playQueue, editorState);
    };

}


