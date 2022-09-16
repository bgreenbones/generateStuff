//
//  Sequence.hpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#pragma once

#ifndef Sequence_hpp
#define Sequence_hpp

#include <vector>
#include <type_traits>
#include <random>

#include "Note.hpp"
#include "Subdivision.h"
#include "Misc.h"

//class Rhythm;
using namespace std;



template<class T> // must be TimedEvent
vector<T> byPosition(vector<T> events, Position position) {
    static_assert(is_base_of<TimedEvent, T>::value, "T not derived from TimedEvent");
    vector<T> result;
    for (auto it = events.begin(); it < events.end(); it++) {
        if (it->contains(position)) {
            result.push_back(*it);
        }
    }
    return result;
};

template<class T> // must be TimedEvent
T longest(vector<T> events) {
    static_assert(is_base_of<TimedEvent, T>::value, "T not derived from TimedEvent");
    T result;
    Duration maximumDuration = 0;
    for (auto it = events.begin(); it < events.end(); it++) {
        if (it->duration > maximumDuration) {
            maximumDuration = it->duration;
            result = *it;
        }
    }
    return result;
};

class Sequence: public TimedEvent
{
private:
    void initialize_random() {
        gen = mt19937(rd());
    }
public:
    Sequence(Duration subdivision, Position startTime, Duration duration):
    TimedEvent(startTime, duration) {//}, subdivisions({ Subdivision(subdivision, startTime, duration) }) {
        initialize_random();
        vector<Subdivision> newVec;
        newVec.push_back(Subdivision(subdivision, startTime, duration));
        this->subdivisions = newVec;
    }
    Sequence(Position startTime, Duration duration): Sequence (Beats(0.25), startTime, duration) {}
    Sequence(Duration duration): Sequence(Beats(0.25), 0, duration) {}
    Sequence(): Sequence(Beats(0.25), 0, Bars(2)) {}
//    Sequence(Rhythm rhythm, Phrase phrasing);
    Sequence(Sequence const& other): TimedEvent(other), notes(other.notes), subdivisions(other.subdivisions) {
        initialize_random();
    };

    Sequence& operator=(Sequence other) {
        TimedEvent::operator=(other);
        initialize_random();
        swap(notes, other.notes);
        swap(subdivisions, other.subdivisions);
        return *this;
    };

    random_device rd; // todo: pull this stuff out into a singleton class that everyone uses?
    mt19937 gen;
    
    vector<Note> notes;
    vector<Subdivision> subdivisions;
    Subdivision primarySubdivision() const { return longest<Subdivision>(subdivisions); }
    
//    vector<Note>::iterator begin() { return notes.begin(); }
//    vector<Note>::iterator end() { return notes.end(); }
//    Rhythm rhythm;
//    Phrase phrasing;
    bool addNote(Note toAdd);
    
    
    // Rhythmic thing.
    Sequence pulseAndDisplace(Duration pulse = 0.5,
                              Duration displacement = 0.25,
                              Probability pDisplace = 0.5,
                              Probability pDouble = 0.75) const; // (for forward displacement, set pDouble = 1 and displacement = 1 - amount to displace forward)
    Sequence fillInGaps() const;
    Sequence chooseAccents() const;
    
    // Latin.cpp
    Sequence randomCascara(Probability pDisplace = 0.5,
                           Probability pDouble = 0.75) const;
    Sequence randomClave() const;
    Sequence cascaraFromClave() const;
    Sequence claveFromCascara() const;
    
    
    // Ornament stuff
    Sequence addOrnaments(vector<OrnamentSimple> possibleOrnaments, float tempo, vector<float> probabilities =  { }) const;
    Sequence addOrnaments(OrnamentSimple ornament, float tempo) const;
};




//vector<Note> one = {};
//vector<Note> two = byPosition<Note>(one, 1);

//
//class Sequence: public TimedEvent
//{
//public:
//    Sequence();
//    Sequence& operator=(Sequence other);
//
//    vector<TimedEvent> sequenced;
//    bool addItem(TimedEvent toAdd);
//};

#endif /* Sequence_hpp */
