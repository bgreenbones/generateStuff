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
template <typename T>
class Sequence
{
public:
    Sequence(vector<T> events, TimedEvent *parent): parent(parent), events(events) {}
    Sequence(TimedEvent *parent): Sequence({}, parent) {}
    Sequence(): Sequence({}, nullptr) {}
    Sequence& operator=(Sequence other) {
        swap(parent, other.parent);
        swap(events, other.events);
        return *this;
    };
    
    bool monophonic;
    TimedEvent *parent;
    vector<T> events;
    T primary() const { return longest<T>(events); }
    void updateTimeSignature();
    
    Position endTime() const {
        return events.size() > 0 ? events.back().endTime() : Position(0);
    }
    
    bool add(T toAdd);
    void tie();
    bool concat(Sequence<T> other, bool useLast = false);
    bool chopAfterDuration(Duration duration);

    // Mininotation stuff
    static Sequence parseMininotation(std::string phraseString, Duration stepLength);
    
    
    // call throughs...
    void clear() { return events.clear(); }
    void pop_back() { return events.pop_back(); }
    T& back() { return events.back(); }
    
    
    
    bool equals(vector<T> other) {
        if (other.size() != events.size()) {
            return false;
        }
        
        for (int i = 0; i < other.size(); i++) {
            if (other[i].startTime != events[i].startTime) {
                return false;
            }
            
            if (other[i].duration != events[i].duration) {
                return false;
            }
        }
        
        return true;
    }
};

