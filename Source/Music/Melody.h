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
    const GenerationFunction melodyFromFunction = [](Phrase fromPhrase) {
        Phrase phrase(fromPhrase);
        phrase.notes = fromPhrase.notes.toMonophonic();
        phrase = phrase.tonalities.empty() ? harmony::generateTonalities(phrase, 0.6) : phrase;
        phrase.notes.clear();
        
        Position cursor = 0;
        while (cursor < phrase.duration) {
            vector<Tonality> tonalities = phrase.tonalities.byPosition(cursor);
            Tonality tonality = tonalities.empty() ? harmony::randomTonality() : tonalities.at(0);
            
            vector<Subdivision> subdivs = phrase.subdivisions.byPosition(cursor);
            Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
            
            Sequence<Note> burstOfNotes = Sequence<Note>::burst(phrase, subdiv, 1 + rollDie(4));
            Pitch lastPitch = draw<Pitch>(tonality.getPitches());

            for (Note &note : burstOfNotes) {
                Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
                note.pitch = tonality.step(lastPitch, direction);
                lastPitch = note.pitch;
            }
            
            phrase.notes.insertSequence(burstOfNotes, cursor);
            
            cursor += (7 + rollDie(4)) * subdiv;
        }

        return phrase;
    };

    const GenerationFunction melodyFunction = [](Phrase fromPhrase) {
        return melodyFromFunction(harmony::generateTonalities(fromPhrase));
    };

    const GenerationFunction bassFromFunction = [](Phrase const& fromPhrase) {
        Phrase phrase(fromPhrase);
        phrase.notes = fromPhrase.notes.toMonophonic();
        phrase = phrase.tonalities.empty() ? harmony::generateTonalities(phrase, 0.6) : phrase;
        phrase.notes.clear();
        
        set<Position> keyPoints;
        
        auto notes = fromPhrase.toMonophonic();
        auto accents = notes.accents();
        
        for (Note note : notes.notes) { keyPoints.emplace(note.startTime); }
        for (Tonality tonality : phrase.tonalities) { keyPoints.emplace(tonality.startTime); }
        
        
        // TODO: use burst to come up with some rando beat repeat style rhythms.

        for (Position keyPoint : keyPoints) {
            vector<Tonality> tonalities = phrase.tonalities.byPosition(keyPoint);
            if (tonalities.empty()) { continue; }
            Tonality tonality = tonalities.at(0);
            
            Note noteToAdd(Pitch(tonality.root, 3), 70, tonality.startTime, tonality.duration);
            phrase = rhythm::burst(phrase, noteToAdd, 1, 4);
        }

        return phrase;
    };

    const GenerationFunction bassFunction = [](Phrase fromPhrase) {
        return bassFromFunction(harmony::generateTonalities(fromPhrase));
    };

}


