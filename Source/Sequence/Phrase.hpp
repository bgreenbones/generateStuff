//
//  generateStuff - All
//  Phrase.hpp
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#pragma once

#ifndef Phrase_hpp
#define Phrase_hpp

#include <vector>
#include <type_traits>
#include <random>

#include "Note.hpp"
#include "Subdivision.h"
#include "Sequence.h"
#include "Syncopation.h"
#include "Random.h"

using namespace std;

class Phrase: public TimedEvent
{
public:
    Phrase(Duration subdivision, Position startTime, Duration duration):
    TimedEvent(startTime, duration), notes(*this), subdivisions(*this) {//}, subdivisions({ Subdivision(subdivision, startTime, duration) }) {
        this->subdivisions.add(Subdivision(subdivision, startTime, duration));
//        vector<Subdivision> newVec;
//        newVec.push_back(Subdivision(subdivision, startTime, duration));
//        this->subdivisions = newVec;
    }
    Phrase(Position startTime, Duration duration): Phrase (Beats(0.25), startTime, duration) {}
    Phrase(Duration duration): Phrase(Beats(0.25), 0, duration) {}
    Phrase(): Phrase(Beats(0.25), 0, Bars(2)) {}
//    Phrase(Rhythm rhythm, Phrase phrasing);
    Phrase(Phrase const& other): TimedEvent(other), notes(other.notes), subdivisions(other.subdivisions) {
//        notes.parent = *this;
//        subdivisions.parent = *this;
    };

    Phrase& operator=(Phrase other) {
        TimedEvent::operator=(other);
        swap(notes, other.notes);
        notes.parent = *this;
        swap(subdivisions, other.subdivisions);
        subdivisions.parent = *this;
        return *this;
    };
    
    Sequence<Note> notes;
    Sequence<Subdivision> subdivisions;
    
    Subdivision primarySubdivision() const { return subdivisions.primary(); }
    void updateTimeSignature();
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
    
//    vector<Note>::iterator begin() { return notes.begin(); }
//    vector<Note>::iterator end() { return notes.end(); }
//    Rhythm rhythm;
//    Phrase phrasing;
    template <class T>
    static void addTimedEvent(T toAdd, vector<T>& eventList);
    bool addNote(Note toAdd);
    bool addSubdivision(Subdivision toAdd);
    void tieSubdivisions();
    template <class T>
    vector<T> concatEvents(vector<T> eventList, vector<T> otherList) const;
    Phrase concat(Phrase other, bool useLastNote = false, bool keepDuration = false) const;
    
    // Rhythmic thing.
    Phrase pulseAndDisplace(Duration pulse = 0.5,
                              Duration displacement = 0.25,
                              Probability pDisplace = 0.5,
                              Probability pDouble = 0.75) const; // (for forward displacement, set pDouble = 1 and displacement = 1 - amount to displace forward)
    Phrase fillWithRolls(Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb) const;
    Phrase chooseAccents() const;
    
    // Latin.cpp
    Phrase randomCascara(Probability pDisplace = 0.5,
                           Probability pDouble = 0.75) const;
    Phrase randomClave() const;
    Phrase cascaraFromClave() const;
    Phrase claveFromCascara() const;
    
    
    // Ornament stuff - TODO: i don't think these really belong in Phrase class
    Phrase addOrnaments(vector<OrnamentSimple> possibleOrnaments, Probability prob, double breadth, vector<float> probabilities =  { }) const;
    Phrase addOrnaments(OrnamentSimple ornament, Probability prob, double breadth) const;
    Phrase withRoll(Position start, Position target, Association association) const;
    
    // Mininotation stuff
    Phrase parseMininotation(std::string phraseString, Subdivision subdivision);
    

};



#endif /* Phrase_hpp */
