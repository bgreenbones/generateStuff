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
#include "Probability.h"
#include "Pitch.h"

typedef enum PushBehavior {
    ignore, truncate, wrap
} PushBehavior;

enum OverwriteBehavior {
    ignore, erase, cutoff
};

// this can encapsulate functionality related to the various vectors inside phrase
// ...notes...subdivisions...dynamics, and other expressions to be added down the line
template <typename T> // T as to be TimedEvent subclass
class Sequence: public vector<T>
{
public:
    static Sequence<T> burst(TimedEvent &parent, Duration eventLength, int numberOfEvents) {
        Sequence<T> result(parent);
        
        for (double i = 0; i < numberOfEvents; i++) {
            result.push_back(T(i * eventLength, eventLength));
        }
        
        return result;
    }
    
    
    Sequence(vector<T> events, bool monophonic, TimedEvent& parent): vector<T>(events), monophonic(monophonic), parent(parent) {}
    Sequence(vector<T> events, TimedEvent& parent): Sequence(events, true, parent) {}
    Sequence(TimedEvent &parent): Sequence({}, parent) {}
    Sequence(Sequence other, TimedEvent& newParent): Sequence(other, other.monophonic, newParent) {}
    Sequence& operator=(Sequence const& other) {
        this->assignEvents(other);
        this->parent = other.parent;
        monophonic = other.monophonic;
        return *this;
    };
    
    bool monophonic = true;
    bool isPolyphonic() { return !monophonic; }
    TimedEvent &parent;

    Sequence<T> toMonophonic() const;
    Sequence<T> toPolyphonic() const;

    T primary() const { return longest<T>(*this); }
    vector<T> byPosition(Position position) const;
    vector<T> byStartPosition(Position position) const;
    vector<T> bySpan(TimedEvent span) const;
    T drawByPosition(Position position) const;
    Position nextStartTime(Position previousStartTime) const;
        
    Position endTime() const {
        return this->size() > 0 ? this->back().endTime() : Position(0);
    }
    
    bool add(T toAdd, PushBehavior pushBehavior = PushBehavior::ignore, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore);
    Sequence<T> tie(bool fillBeginning = false);
    Sequence<T> legato();
    bool concat(Sequence<T> other, bool useLast = false, PushBehavior pushBehavior = PushBehavior::ignore);
    bool insertVector(vector<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore);
    bool insertSequence(Sequence<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignore, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore);
    bool chopAfterDuration(Duration duration);
    bool flip();
    
    // Rhythm stuff
    Sequence<T> pulseAndDisplace(Duration pulse,
                                 Duration displacement,
                                 Probability pDisplace,
                                 Probability pDouble,
                                 Duration length) const;

    // Mininotation stuff
    Sequence<T> parseMininotation(std::string phraseString, Duration stepLength);
    bool append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore);
//    bool insertMininotation(std::string phraseString, Position startTime, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore, bool overwrite = false);
    bool insertMininotation(std::string phraseString, Position startTime, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignore);
    
    // 'visualization' / debugging
    std::string getStartTimesString();
    
    
    void assignEvents(vector<T> events) {
        this->clear();
        this->assign(events.begin(), events.end());
    };
    
    bool equals(vector<T> other) {
        if (other.size() != this->size()) {
            return false;
        }
        
        for (int i = 0; i < other.size(); i++) { // TODO: probably need to check other criteria...
            if (other[i].startTime != this->at(i).startTime) {
                return false;
            }
            
            if (other[i].duration != this->at(i).duration) {
                return false;
            }
        }
        
        return true;
    }
};





// Haha declaring Note now because of crazy explicit template instantiation logistics.
#include "Probability.h"
#include "Mininotation.h"
#include "Ornamentation.h"
#include "Dynamics.h"

static const DynamicLevel accentVelocity = DynamicLevel::fff;
static const DynamicLevel unaccentedVelocity = DynamicLevel::mp;
static const DynamicLevel defaultVelocity = DynamicLevel::mf;

static const Pitch defaultPitch(C, 5);

class Note: public TimedEvent
{
public:
    Note(Pitch pitch = defaultPitch,
         int velocity = defaultVelocity,
         Position startTime = 0,
         Duration duration = 1): TimedEvent(startTime, duration), pitch(pitch), velocity(velocity), accented(false), ornamented(false), isOrnament(false) { }
    Note(Position startTime, Duration duration): Note(defaultPitch, defaultVelocity, startTime, duration) { }
    Note(char mininotation, Position startTime, Duration duration): Note(defaultPitch, defaultVelocity, startTime, duration) {
        if (Mininotation::isValue(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (Mininotation::isAlternate(mininotation)) {
            accent();
            if (accented != 1.0) {
                DBG ("i guess accented() called the const version");
            }
        }
    }
    
    static DynamicLevel accentVelocity;
    operator int() const { return pitch.pitchValue; };
    
    Note operator+(const Duration duration);
    Note operator+(const Note pitch);
    
    Note& operator=(Note const& other) {
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
    Note accent() {
        this->accented = 1.0;
        this->velocity = accentVelocity;
        return *this;
    };

    Note withAccent() const {
        Note modified = Note(*this);
        return modified.accent();
    };
    
    Note withDuration(Duration newDuration) const {
        Note modified = Note(*this);
        modified.duration = newDuration;
        return modified;
    }

    Note ornament() const {
        Note modified = Note(*this);
        modified.ornamented = 1.0;
        return modified;
    }
    
    Sequence<Note> placeOrnament(OrnamentSimple ornamentSimple, double breadth) const;
    
    Pitch pitch;
    int velocity;
    Probability accented;
    Probability ornamented;
    bool isOrnament;
    // todo: other expressions for ccs
    
    bool operator< (const Note &other) { return startTime < other.startTime; }
    bool operator> (const Note &other) { return startTime > other.startTime; }
    
    bool equalsExcludingTime(Note &other) {
        return this->pitch == other.pitch &&
            this->velocity == other.velocity &&
            this->accented == other.accented &&
            this->ornamented == other.ornamented &&
            this->isOrnament == other.isOrnament;
    }
};




