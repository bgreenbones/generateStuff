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

namespace melody {
    
    const GenerationFunction melodyFunction = [](Phrase fromPhrase) {
        // TODO: this
        return fromPhrase;
    };

    const GenerationFunction melodyFromFunction = [](Phrase fromPhrase) {
        // TODO: this
        return fromPhrase;
    };

    const GenerationFunction bassFunction = [](Phrase fromPhrase) {
        // TODO: this
        return fromPhrase;
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
        
        //            vector<Tonality> tonalities = phrase.tonalities.byPosition(note.startTime);
        //            if (tonalities.empty()) { continue; }
        //            Tonality tonality = tonalities.at(0);
        //
        //            Note noteToAdd(Pitch(tonality.root, 3), 70, tonality.startTime, tonality.duration);
        //            phrase.addNote(noteToAdd);
        
        return phrase;
    };

}


