/*
  ==============================================================================

    Mininotation.cpp
    Created: 10 Oct 2022 10:53:39pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/


#include "Mininotation.h"
#include "Note.hpp"
#include "Subdivision.h"

template <class T>
vector<T> Mininotation::parse(std::string phraseString, Duration stepLength) {
    vector<T> result;
    
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

        if (isValue(symbol)) {  // TODO: implement class-specific interpretations of mininotation symbols
            T toAdd(symbol, startTime, stepLength);
            result.push_back(toAdd);
        }
    }

    return result;
}



template vector<Note> Mininotation::parse<Note>(std::string phraseString, Duration stepLength);
template vector<Subdivision> Mininotation::parse<Subdivision>(std::string phraseString, Duration stepLength);
template vector<Tonality> Mininotation::parse<Tonality>(std::string phraseString, Duration stepLength);
