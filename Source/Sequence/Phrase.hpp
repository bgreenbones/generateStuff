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
#include "Syncopation.h"
#include "Random.h"
#include "Pitch.h"
#include "ChordScale.h"
#include "GenerateStuffEditorState.h"

using namespace std;


static const Duration defaultSubdivision = Beats(0.25);
static const Position defaultStartTime = Position(0, true);
static const Duration defaultDuration = Bars(2, true);

class Phrase: private TimedEvent
{
public:
    Position getStartTime() { return startTime; }
    Position getEndTime() { return endTime(); }
    void setStartTime(Position newStartTime) { startTime = newStartTime; }
    Duration getDuration() { return duration; }
    void setDuration(Duration newDuration) {
        if (newDuration < duration) {      
            for (auto sequence : noteSequences) {
                sequence->chopAfterDuration(newDuration);
            }
            subdivisions.chopAfterDuration(newDuration);
            chordScales.chopAfterDuration(newDuration);
        }
        duration = newDuration;
    }
    
    Phrase(Duration subdivision, Position startTime, Duration duration):
        TimedEvent(startTime, duration),
        notes(*this),
        connectingNotes(*this),
        ornamentationNotes(*this),
        subdivisions(*this),
        chordScales(*this)
    {
        this->subdivisions.add(Subdivision(subdivision, startTime, duration));
    }
    Phrase(Position startTime, Duration duration): Phrase (defaultSubdivision, startTime, duration) {}
    Phrase(Duration duration): Phrase(defaultSubdivision, defaultStartTime, duration) {}
    Phrase(): Phrase(defaultSubdivision, defaultStartTime, defaultDuration) {}
    Phrase(Phrase const& other):
        TimedEvent(other),
        voice(other.voice),
        schedule(other.schedule),
        notes(other.notes, *this),
        connectingNotes(other.connectingNotes, *this),
        ornamentationNotes(other.ornamentationNotes, *this),
        subdivisions(other.subdivisions, *this),
        chordScales(other.chordScales, *this)
        {};
    Phrase& operator=(Phrase const& other) {
        TimedEvent::operator=(other);
        notes = Sequence<Note>(other.notes, *this);
        connectingNotes = Sequence<Note>(other.connectingNotes, *this);
        ornamentationNotes = Sequence<Note>(other.ornamentationNotes, *this);
        subdivisions = Sequence<Subdivision>(other.subdivisions, *this);
        chordScales = Sequence<ChordScale>(other.chordScales, *this);
        voice = other.voice;
        schedule = other.schedule;
        return *this;
    };

    string voice;
    set<TimedEvent> schedule;
    Sequence<Note> notes;
    Sequence<Note> connectingNotes;
    Sequence<Note> ornamentationNotes;
    vector<Sequence<Note>*> noteSequences = {
        &notes, &connectingNotes, &ornamentationNotes
    };
    Sequence<Subdivision> subdivisions;
    Sequence<ChordScale> chordScales;

    Position nextSubdivisionPosition(Position position);
    Position previousSubdivisionPosition(Position position);

    bool isPolyphonic() {
        return notes.isPolyphonic();
    }

    Phrase toMonophonic() const {
        if (notes.monophonic) { return *this; }
        Phrase result(*this);
        result.notes = result.notes.toMonophonic();
        result.connectingNotes = result.connectingNotes.toMonophonic();
        result.ornamentationNotes = result.ornamentationNotes.toMonophonic();
        return result;
    }
    Phrase toPolyphonic() const {
        if (!(notes.monophonic)) { return *this; }
        Phrase result(*this);
        result.notes = result.notes.toPolyphonic();
        result.connectingNotes = result.connectingNotes.toPolyphonic();
        result.ornamentationNotes = result.ornamentationNotes.toPolyphonic();
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
    Phrase insert(Phrase other, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore) const;
    Phrase loop(Duration loopDuration) const {
        if (this->duration == loopDuration) {
            return *this;
        }
        Phrase result(*this);
        while (result.duration < loopDuration) {
            result.notes.concat(result.notes);
            result.connectingNotes.concat(result.connectingNotes);
            result.ornamentationNotes.concat(result.ornamentationNotes);
            result.subdivisions.concat(result.subdivisions);
            result.chordScales.concat(result.chordScales);
            result.setDuration(2*result.duration);
        }
        result.setDuration(loopDuration);
        return result;
    }
    
    //
    Duration halfLength() const {
        return duration / 2.;
    };
    bool isNoteOnLeft(Note note) const { return note.startTime < halfLength(); };
    bool isNoteOnRight(Note note) const { return !isNoteOnLeft(note); };

    Phrase fillWithRolls(Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb) const;
    Phrase accents() const;

    // TODO: move to rhythm namespace
    Phrase randomSubdivisions(vector<Duration> availableSubdivisions, vector<double> weights) const;
    Phrase randomGhostSubdivision(Probability ghostProbability = 0.6,
                                  Probability subdivisionProbability = 1.,
                                  Pitch pitch = defaultPitch,
                                  TimedEvent span = nullTime) const;
    Phrase ghostSubdivision(Pitch pitch = defaultPitch) const;
    Phrase randomGhostBursts(Duration minimumBurstLength = Beats(1./2.),
                             Duration maximumBurstLength = Beats(2),
                             Probability ghostProbabilityWithinBurst = 0.8,
                             Probability burstProbability = 0.4, Pitch pitch = defaultPitch) const;
    
    
    // Ornament stuff - TODO: move to ornament namespace
    Phrase addOrnaments(vector<OrnamentSimple> possibleOrnaments, Probability prob, double breadth, vector<float> probabilities =  { }) const;
    Phrase addOrnaments(OrnamentSimple ornament, Probability prob, double breadth) const;
    Phrase withRoll(Position start, Position target, Association association) const;
    
    // Mininotation stuff
    Phrase parseMininotation(std::string phraseString, Subdivision subdivision);
    

};

// THIS ASSUMES SEQUENCE SORTED BY STARTTIME
template <class T>
typename vector<T>::const_iterator next(Sequence<T> const& seq, typename vector<T>::const_iterator iter) {
//    typename vector<T>::const_iterator next(Sequence<T> const& seq, typename vector<T>::const_iterator const& iter) {
    Position startTime = iter->startTime;
    while(iter->startTime == startTime) {
        iter = iter + 1 == seq.end() ? seq.begin() : iter + 1;
        if (iter == seq.begin()) { break; }
    }
    return iter + 1 == seq.end() ? seq.begin() : iter + 1;
}

template <class T>
Duration timeBetween(T const& first, T const& second, Phrase phrase)

{
    return first.startTime < second.startTime
        ? second.startTime - first.startTime
        : second.startTime + phrase.getDuration() - first.startTime;
}

typedef string VoiceName;
// typedef function<Phrase(Phrase, GenerateStuffEditorState const&)> GenerationFunction;
// typedef function<Phrase(Phrase, shared_ptr<Ensemble>, GenerateStuffEditorState const&)> GenerationFunction;
// typedef function<Phrase(Ensemble const&, GenerateStuffEditorState const&)> GenerationFunction;

class Ensemble;
class Voice;
typedef function<Phrase(Phrase, Ensemble&, GenerateStuffEditorState const&)> GenerationFunction;
typedef function<Phrase(Voice const&)> GenerationFunction2;

#endif /* Phrase_hpp */
