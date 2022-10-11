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

typedef enum PushBehavior {
    ignore, truncate, wrap
} PushBehavior;

// maybe this should be an inner class of phrase?
// right now just will have a reference to its parent phrase.
// this can encapsulate functionality related to the various vectors inside phrase
// ...notes...subdivisions...dynamics, and other expressions to be added down the line
template <typename T>
class Sequence
{
public:
    Sequence(vector<T> events, TimedEvent& parent): parent(parent), events(events) {}
    Sequence(TimedEvent &parent): Sequence({}, parent) {}
//    Sequence(): Sequence({}, nullptr) {}
    Sequence& operator=(Sequence other) {
        swap(monophonic, other.monophonic);
        this->parent = other.parent;
//        swap(parent, other.parent);
        swap(events, other.events);
        return *this;
    };
    
    bool monophonic = true;
    TimedEvent &parent;
    vector<T> events;
    T primary() const { return longest<T>(events); }
    vector<T> byPosition(Position position) const;
    
    void updateTimeSignature();
    
    Position endTime() const {
        return events.size() > 0 ? events.back().endTime() : Position(0);
    }
    
    bool add(T toAdd, PushBehavior pushBehavior = PushBehavior::ignore);
    void tie();
    void legato();
    bool concat(Sequence<T> other, bool useLast = false, PushBehavior pushBehavior = PushBehavior::ignore);
    bool insert(vector<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore);
    bool insert(Sequence<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore);
    bool chopAfterDuration(Duration duration);


    // Mininotation stuff
    Sequence parseMininotation(std::string phraseString, Duration stepLength);
    bool append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore);
    
    
    
    // call throughs... ... maybe this should just inherit from vector...
    void clear() { return events.clear(); }
    void pop_back() { return events.pop_back(); }
    T& front() { return events.front(); }
    T front() const { return events.front(); }
    T& back() { return events.back(); }
    T back() const { return events.back(); }
    size_t size() const { return events.size(); }
    auto begin() const { return events.begin(); }
    auto begin() { return events.begin(); }
    auto end() const { return events.end(); }
    auto end() { return events.end(); }
    bool empty() const { return events.empty(); }
    
    
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

