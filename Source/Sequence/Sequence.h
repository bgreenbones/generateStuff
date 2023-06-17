/*
  ==============================================================================

    Sequence.h
    Created: 2 Oct 2022 6:38:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <string>
#include <vector>
#include "TimedEvent.h"
#include "Probability.h"


using std::vector, std::reference_wrapper;

typedef enum PushBehavior {
    ignorePush, truncate, wrap
} PushBehavior;

enum OverwriteBehavior {
    ignoreOverwrite, erase, cutoff, insert
};

// this can encapsulate functionality related to the various vectors inside phrase
// ...notes...subdivisions...dynamics, and other expressions to be added down the line
template <typename T> // T as to be TimedEvent subclass
class Sequence: public vector<Timed<T>>
{
public:
    static vector<Timed<T>> fromTimes(vector<Time> const& timed, T const& t, Position cursor = 0);
    static vector<Timed<T>> fromTimes(vector<Time> const& timed, Position cursor = 0);
    static vector<Timed<T>> burst(Duration eventLength, int numberOfEvents);
    
    Sequence(vector<Timed<T>> events, bool monophonic, Time& parent): vector<Timed<T>>(events), monophonic(monophonic), parent(parent) {}
    Sequence(vector<Timed<T>> events, Time& parent): Sequence(events, true, parent) {}
    Sequence(Time &parent): Sequence({}, parent) {}
    Sequence(Sequence other, Time& newParent): Sequence(other, other.monophonic, newParent) {}
    Sequence& operator=(Sequence const& other) {
        this->assignEvents(other);
        this->parent = other.parent;
        monophonic = other.monophonic;
        return *this;
    };
    
    bool monophonic = true;
    bool isPolyphonic() { return !monophonic; }
    Time &parent;

    Sequence<T> toMonophonic() const;
    Sequence<T> toPolyphonic() const;

    Timed<T> primary() const { 
        Timed<T> result;
        Duration maximumDuration = 0;
        for (auto it = this->begin(); it < this->end(); it++) {
            if (it->duration > maximumDuration) {
                maximumDuration = it->duration;
                result = *it;
            }
        }
        return result;
        // return longest<T>(*this);
    }
    Timed<T>* last() { return this->empty() ? nullptr : &(*this)[this->size() - 1];};
    vector<Timed<T>> byPosition(Position position) const;
    vector<Timed<T>*> pointersByPosition(Position position);
    // vector<T&> refsByPosition(Position position) const;
    vector<Timed<T>> byStartPosition(Position position) const;
    vector<Timed<T>> bySpan(Time span) const;
    vector<reference_wrapper<Timed<T>>> refsBySpan(Time span);
    Timed<T> drawByPosition(Position position) const;
    Position nextStartTime(Position previousStartTime) const;
        
    Position endTime() const {
        return this->size() > 0 ? this->back().endTime() : Position(0);
    }
    
    bool add(Timed<T> toAdd, PushBehavior pushBehavior = PushBehavior::ignorePush, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignoreOverwrite);
    Sequence<T> tie(bool fillBeginning = false);
    Sequence<T> legato();
    bool concat(Sequence<T> other, bool useLast = false, PushBehavior pushBehavior = PushBehavior::ignorePush);
    bool insertVector(vector<Timed<T>> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignorePush, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignoreOverwrite);
    bool insertSequence(Sequence<T> other, Position startTime, PushBehavior pushBehavior = PushBehavior::ignorePush, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignoreOverwrite);
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
    bool append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignorePush);
//    bool insertMininotation(std::string phraseString, Position startTime, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignore, bool overwrite = false);
    bool insertMininotation(std::string phraseString, Position startTime, Duration stepLength, PushBehavior pushBehavior = PushBehavior::ignorePush, OverwriteBehavior overwriteBehavior = OverwriteBehavior::ignoreOverwrite);
    
    // 'visualization' / debugging
    std::string getStartTimesString();
    
    
    void assignEvents(vector<Timed<T>> events) {
        this->clear();
        this->assign(events.begin(), events.end());
    };
    
    bool equals(vector<Timed<T>> other) {
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
    
    
// TODO: sparse representation/.
// then we won't have to iterate to get timed events from their positions
    // vector<int> sparse;
};


