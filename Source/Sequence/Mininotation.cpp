/*
  ==============================================================================

    Mininotation.cpp
    Created: 10 Oct 2022 10:53:39pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/


#include "Mininotation.h"
#include "Note.hpp"
#include "ChordScale.h"
#include "Phrase.hpp"

vector<Time> Mininotation::parse(std::string phraseString, Duration stepLength) {
    vector<Time> result;
    
    auto symbolIter = phraseString.begin();
    Duration length = stepLength * ((double) getLength(phraseString));
    for(Position startTime = 0; startTime < length; startTime += stepLength) {
        char symbol = *symbolIter++;

        if (!isInNotation(symbol)) {
            DBG ("misuse of mininotation");
            continue;
        }
        if (symbol == rest) { continue; }
        
        
        if (symbol == sustain) {
            if (!result.empty()) {
                result.back().duration += stepLength;
            }
            continue;
        }

        if (isValue(symbol)) {
            Time toAdd(startTime, stepLength);
            result.push_back(toAdd);
        }
    }

    return result;
}

