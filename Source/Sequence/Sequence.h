/*
  ==============================================================================

    Sequence.h
    Created: 2 Oct 2022 6:38:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include "TimedEvent.h"
#include "Subdivision.h"

// maybe this should be an inner class of phrase?
// right now just will have a reference to its parent phrase.
// this can encapsulate functionality related to the various vectors inside phrase
// ...notes...subdivisions...dynamics, and other expressions to be added down the line
template <class T>
class Sequence
{
public:
    Sequence(TimedEvent &parent): parent(parent) {}
    Sequence& operator=(Sequence other) {
        swap(parent, other.parent);
        swap(events, other.events);
        return *this;
    };
    
    bool monophonic;
    TimedEvent &parent;
    vector<T> events;
    T primary() const { return longest<T>(events); }
    void updateTimeSignature();
    
    bool add(T toAdd);
    void tie();
    bool concat(Sequence other, bool useLast = false);
    bool chopAfterDuration(Duration duration);

    
    
    // Mininotation stuff
    static Sequence parseMininotation(std::string phraseString, Subdivision subdivision);
};

