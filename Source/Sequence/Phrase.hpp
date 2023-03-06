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
#include "Pitch.h"

using namespace std;

static const Duration defaultSubdivision = Beats(0.25);
static const Position defaultStartTime = Position(0, true);
static const Duration defaultDuration = Bars(2, true);

class Phrase: public TimedEvent
{
public:
    
    Phrase(Duration subdivision, Position startTime, Duration duration):
        TimedEvent(startTime, duration),
        notes(*this),
        subdivisions(*this)
    {
        this->subdivisions.add(Subdivision(subdivision, startTime, duration));
    }
    Phrase(Position startTime, Duration duration): Phrase (defaultSubdivision, startTime, duration) {}
    Phrase(Duration duration): Phrase(defaultSubdivision, defaultStartTime, duration) {}
    Phrase(): Phrase(defaultSubdivision, defaultStartTime, defaultDuration) {}
    Phrase(Phrase const& other): TimedEvent(other), notes(other.notes, *this), subdivisions(other.subdivisions, *this) {};

    Phrase& operator=(Phrase const& other) {
        TimedEvent::operator=(other);
        notes = Sequence<Note>(other.notes, *this);
        subdivisions = Sequence<Subdivision>(other.subdivisions, *this);
        return *this;
    };
    
    Sequence<Note> notes;
    Sequence<Subdivision> subdivisions;
    
    Phrase toPolyphonic() const {
        if (!(notes.monophonic)) { return *this; }
        Phrase result(*this);
        result.notes = result.notes.toPolyphonic();
        return result;
    }
    Subdivision primarySubdivision() const { return subdivisions.primary(); }
    
    bool equalsExcludingTime(TimedEvent &other) {
        DBG("Not implemented yet");
        return false;
    }
    
    template <class T>
    static void addTimedEvent(T toAdd, vector<T>& eventList);
    bool addNote(Note toAdd);
    bool addSubdivision(Subdivision toAdd);
    void tieSubdivisions();
    void clear() { notes.clear(); subdivisions.clear(); };
    template <class T>
    vector<T> concatEvents(vector<T> eventList, vector<T> otherList) const;
    Phrase concat(Phrase other, bool useLastNote = false, bool keepDuration = false) const;
//    Phrase insert(Phrase other, bool overwrite = true) const;
    Phrase insert(Phrase other, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore) const;
    
    
    //
    Duration halfLength() const { return duration / 2.; };
    bool isNoteOnLeft(Note note) const { return note.startTime < halfLength(); };
    bool isNoteOnRight(Note note) const { return !isNoteOnLeft(note); };
    int getPotentialClaveNoteCount(Duration minNoteLength, Duration maxNoteLength) const;
    int chooseNumberOfNotesOnLeft(double numNotes) const;

    
    // Rhythmic thing.
    Phrase pulseAndDisplace(Duration pulse = 0.5, // TODO: create a rhythm type that gives access to these params RAW instead of the hardcoded cascara idea...
                              Duration displacement = 0.25,
                              Probability pDisplace = 0.5,
                              Probability pDouble = 0.75) const; // (for forward displacement, set pDouble = 1 and displacement = 1 - amount to displace forward)
    Phrase fillWithRolls(Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb) const;
    Phrase chooseAccents() const;
    Phrase flip() const;
    
    // Latin.cpp
    Phrase fillCascara() const;
    Phrase fillClave(int notesNeededOnLeft,
                     int notesNeededOnRight,
                     Duration minNoteLength,
                     Duration maxNoteLength) const;
    Phrase randomCascara(Probability pDisplace = 0.5,
                         Probability pDouble = 0.75) const;
    Phrase randomClave(int minNoteLengthInSubdivisions = 2, int maxNoteLengthInSubdivisions = 4) const;
    Phrase cascaraFrom() const;
    Phrase claveFrom(int minNoteLengthInSubdivisions = 2, int maxNoteLengthInSubdivisions = 4) const;
    
    //
    Phrase randomSubdivisions(vector<double> availableSubdivisions, vector<Probability> weights) const;
    Phrase ghostSubdivision(Pitch pitch) const;
    
    
    
    
    // Ornament stuff - TODO: i don't think these really belong in Phrase class
    Phrase addOrnaments(vector<OrnamentSimple> possibleOrnaments, Probability prob, double breadth, vector<float> probabilities =  { }) const;
    Phrase addOrnaments(OrnamentSimple ornament, Probability prob, double breadth) const;
    Phrase withRoll(Position start, Position target, Association association) const;
    
    // Mininotation stuff
    Phrase parseMininotation(std::string phraseString, Subdivision subdivision);
    

};

template <class T>
typename vector<T>::const_iterator next(Sequence<T> const& seq, typename vector<T>::const_iterator const& iter) {
    return iter + 1 == seq.end() ? seq.begin() : iter + 1;
}

template <class T>
Duration timeBetween(T const& first, T const& second, Phrase phrase)

{
    return first.startTime < second.startTime
        ? second.startTime - first.startTime
        : second.startTime + phrase.duration - first.startTime;
}




#endif /* Phrase_hpp */
