/*
  ==============================================================================

    Sequence.h
    Created: 2 Oct 2022 6:38:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <string>
#include "TimedEvent.h"
#include "Subdivision.h"

typedef enum PushBehavior {
    ignore, truncate, wrap
} PushBehavior;


// this can encapsulate functionality related to the various vectors inside phrase
// ...notes...subdivisions...dynamics, and other expressions to be added down the line
template <typename T> // T as to be TimedEvent subclass
class Sequence
{
public:
    Sequence(vector<T> events, bool monophonic, TimedEvent& parent): monophonic(monophonic), parent(parent), events(events) {}
    Sequence(vector<T> events, TimedEvent& parent): Sequence(events, true, parent) {}
    Sequence(TimedEvent &parent): Sequence({}, parent) {}
    Sequence(Sequence other, TimedEvent& newParent): Sequence(other.events, other.monophonic, newParent) {}
    Sequence& operator=(Sequence const& other) {
        monophonic = other.monophonic;
        events = other.events;
//        swap(monophonic, other.monophonic);
//        swap(events, other.events);
        return *this;
    };
    
    bool monophonic = true;
    TimedEvent &parent;
    vector<T> events;
    T primary() const { return longest<T>(events); }
    vector<T> byPosition(Position position) const;
    T drawByPosition(Position position) const;
        
    Position endTime() const {
        return events.size() > 0 ? events.back().endTime() : Position(0);
    }
    
    bool add(T toAdd, PushBehavior pushBehavior = PushBehavior::ignore, bool overwrite = false);
    void tie();
    void legato();
    bool concat(Sequence<T> other, bool useLast = false, PushBehavior pushBehavior = PushBehavior::ignore);
    bool insert(vector<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore);
    bool insert(Sequence<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore);
    bool chopAfterDuration(Duration duration);
    bool flip();


    // Mininotation stuff
    Sequence parseMininotation(std::string phraseString, Duration stepLength);
    bool append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore);
    
    // 'visualization' / debugging
    std::string getStartTimesString();
    
    
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





// Haha declaring Note now.


#include "Probability.h"
#include "Mininotation.h"
#include "Ornamentation.h"

class Note: public TimedEvent
{
public:
    Note(int pitch,
         int velocity,
         Position startTime,
         Duration duration):
    TimedEvent(Position(startTime), Duration(duration)), pitch(pitch), velocity(velocity), accented(false), ornamented(false), isOrnament(false) { }
    
    Note(int pitch = 60,
         int velocity = 70,
         double startTime = 0, // in quarter notes
         double duration = 1): // in quarter notes
    Note(pitch, velocity, Position(startTime), Duration(duration)) { }
    Note(Position startTime, Duration duration): Note(60, 70, startTime, duration) { }
    Note(char mininotation, Position startTime, Duration duration): Note(60, 70, startTime, duration) {
        if (Mininotation::isNote(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (mininotation == Mininotation::modifiedDefault) {
            accent();
            if (accented != 1.0) {
                DBG ("i guess accented() called the const version");
            }
        }
    }
    
    static int accentVelocity;
    
    Note operator+(const Duration duration);
    Note operator+(const Note pitch);
    
    Note& operator=(Note const other) {
        this->startTime = other.startTime;
        this->duration = other.duration;
        this->pitch = other.pitch;
        this->velocity = other.velocity;
        this->accented = other.accented;
        this->ornamented = other.ornamented;
        this->isOrnament = other.isOrnament;
        return *this;
    };
    
    Note displace(Duration toDisplaceBy, bool forwards = true);
    Note accent() const {
        Note modified = Note(*this);
        modified.accented = 1.0;
        modified.velocity = accentVelocity;
        return modified;
    };
//    Note accent() {
//        this->accented = 1.0;
//        this->velocity = accentVelocity;
//        return *this;
//    };
    Note ornament() const {
        Note modified = Note(*this);
        modified.ornamented = 1.0;
        return modified;
    }
    
    Sequence<Note> placeOrnament(OrnamentSimple ornamentSimple, double breadth) const;
    
    int pitch; // todo: use my C pitch definitions
    int velocity; // todo: use juce types for unsigned shorts and others?
    Probability accented;
    Probability ornamented;
    bool isOrnament;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) const { return startTime < other.startTime; }
    bool operator> (const Note &other) const { return startTime > other.startTime; }
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
};







//#include <vector>
//
//template <typename T>
//class S
//{
//public:
//    vector<T> stuff;
//};
//
//class N
//{
//public:
//    S<N> f();
//};
