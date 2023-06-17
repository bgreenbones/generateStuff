/*
  ==============================================================================

    Sequence.cpp
    Created: 2 Oct 2022 6:38:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include <algorithm>
#include "Sequence.h"
#include <JuceHeader.h>
#include "Mininotation.h"
#include "Note.hpp"
#include "Random.h"
#include "ChordScale.h"
#include "Rhythm.h"
#include "Utility.h"


template <class T>
vector<T> Sequence<T>::fromTimed(vector<Timed> const& timed, T const& t, Position cursor) {
    return mapp<Timed, T>(timed, [&](Timed time) {
        T newT = T(t);
        newT.startTime = time.startTime + cursor;
        newT.duration = time.duration;
        return newT;
    });
}
template <class T>
vector<T> Sequence<T>::fromTimed(vector<Timed> const& timed, Position cursor) {
    return Sequence<T>::fromTimed(timed, T(), cursor);
}

template <class T>
vector<T> Sequence<T>::burst(Duration eventLength, int numberOfEvents) {
    return fromTimed(rhythm::nOfLengthM(numberOfEvents, eventLength));
}

template <class T>
Position Sequence<T>::nextStartTime(Position previousStartTime) const {
    Position nextStartTime = parent.endTime();
    if (previousStartTime >= nextStartTime) {
        return this->begin()->startTime;
    }
    for (auto event = this->begin(); event < this->end(); event++) {
        if (event->startTime > previousStartTime && event->startTime < nextStartTime) {
            nextStartTime = event->startTime;
        }
    }
    return nextStartTime;
}


template <class T>
bool Sequence<T>::flip() {
    auto flipTime = [this](std::__wrap_iter<T*> t) { return (t->startTime + (parent.duration / 2.)) % parent.duration; };
    for (auto event = this->begin(); event < this->end(); event++) {
        event->startTime = flipTime(event);
    }
    return true;
}

template <class T>
Sequence<T> Sequence<T>::toMonophonic() const {
    if (monophonic) { return *this; }
    
    Sequence<T> result(*this);
    result.clear();
    result.monophonic = true;
    for (T toAdd : *this) { result.add(toAdd, PushBehavior::ignore, OverwriteBehavior::cutoff); }
//    result.tie();
    
    return result;
}

template <class T>
Sequence<T> Sequence<T>::toPolyphonic() const {
    if (!monophonic) { return *this; }
    Sequence<T> result(*this);
    result.monophonic = false;
    return result;
}

template <class T>
bool Sequence<T>::add(T toAdd, PushBehavior pushBehavior, OverwriteBehavior overwriteBehavior) {
    if (!(this->parent.containsPartially(toAdd))) {
        double phraseLength = this->parent.duration.asQuarters();
        double eventStartTime = toAdd.startTime.asQuarters();
        switch (pushBehavior) {
            case PushBehavior::truncate:
                if (toAdd.startTime >= phraseLength) { return false; }
                if (toAdd.endTime() >= phraseLength) { toAdd.duration = Quarters(phraseLength - eventStartTime); }
                break;
            case PushBehavior::wrap:
                toAdd.startTime = std::fmod(eventStartTime, phraseLength);
                break;
            case PushBehavior::ignore:
                break;
            default:
                break;
        }
    } 

    auto needToAvoidTimeOverlap = [this](T const a, T const b) { return monophonic || a.equalsExcludingTime(b); };
    auto problematicOverlap = [this, needToAvoidTimeOverlap](T const toAdd) {
                            return [this, needToAvoidTimeOverlap, toAdd](T t) { 
                                    return needToAvoidTimeOverlap(toAdd, t) && toAdd.containsPartially(t);
                                };
                            };

    vector<T> bad_examples;
    switch (overwriteBehavior) {
        case OverwriteBehavior::ignore:
            copy_if(this->begin(), this->end(),
                    back_inserter(bad_examples),
                    problematicOverlap(toAdd));
            if (bad_examples.size() > 0) {
                DBG ("trying to add timed event where other events are in its way");
                return false;
            }
            break;
        case OverwriteBehavior::erase:
            this->erase(std::remove_if(this->begin(), this->end(), problematicOverlap(toAdd)),
                        this->end());
            break;
        case OverwriteBehavior::cutoff:
            this->erase(std::remove_if(this->begin(), this->end(),
                                        [this, needToAvoidTimeOverlap, toAdd](T t) { return toAdd.startTime == t.startTime
                                                    && needToAvoidTimeOverlap(toAdd, t); }),
                        this->end());
            for (auto iter = this->begin(); iter != this->end(); iter++) {
                if (!needToAvoidTimeOverlap(toAdd, *iter)) {
                    continue;
                }
                if (toAdd.contains(iter->startTime)) {
                    toAdd.setEndTime(iter->startTime);
                } else if (iter->contains(toAdd.startTime)) {
                    iter->setEndTime(toAdd.startTime);
                }
            }
            break;
        case OverwriteBehavior::insert:
            for (auto iter = this->begin(); iter != this->end(); iter++) {
                if (!needToAvoidTimeOverlap(toAdd, *iter)) {
                    continue;
                }
                if (toAdd.contains(iter->startTime)) {
                    T alsoAdd(toAdd);
                    toAdd.setEndTime(iter->startTime);
                    if (alsoAdd.endTime() > iter->endTime()) {
                    //  add: -------- 
                    // iter:   ----
                        alsoAdd.setStartTime(iter->endTime());
                        if (alsoAdd.duration > Duration(0)) {
                            iter = this->insert(iter, alsoAdd);
                        }
                    }
                } else if (iter->contains(toAdd.startTime)) {
                    T alsoAdd(*iter);
                    iter->setEndTime(toAdd.startTime);
                    if (iter->duration <= Duration(0)) {
                        iter = this->erase(iter);
                    }
                    if (alsoAdd.endTime() > toAdd.endTime()) {
                    //  add:   ----
                    // iter: --------
                        alsoAdd.setStartTime(toAdd.endTime());
                        if (alsoAdd.duration > Duration(0)) {
                            iter = this->insert(iter, alsoAdd);
                        }
                    }
                }
            }
          break;
//            case default:
//                break;
    }
    
    
    if (toAdd.duration > Duration(0)) {
        this->push_back(toAdd);
    }
    sort(this->begin(),
         this->end(),
         [](T const &a, T const &b) { return a.startTime < b.startTime; });
    
    return true;
}

template <class T>
bool Sequence<T>::concat(Sequence<T> other, bool useLast, PushBehavior pushBehavior) {
    Duration endTime = useLast ? this->endTime() : parent.endTime();
    for (auto iter = other.begin(); iter < other.end(); iter++) {
            iter->startTime += endTime;
        if (!(this->add(*iter, pushBehavior))) {
            DBG("problem concatenating sequences");
            return false;
        }
    }
    return true;
}

template <class T>
bool Sequence<T>::insertVector(vector<T> other, Position startTime, PushBehavior pushBehavior, OverwriteBehavior overwriteBehavior) {
    for (auto iter = other.begin(); iter < other.end(); iter++) {
            iter->startTime += startTime;
        if (!(this->add(*iter, pushBehavior, overwriteBehavior))) {
            DBG("problem inserting sequence");
            return false;
        }
    }
    return true;
}


template <class T>
bool Sequence<T>::insertSequence(Sequence<T> other, Position startTime, PushBehavior pushBehavior, OverwriteBehavior overwriteBehavior) {
    return insertVector(other, startTime, pushBehavior, overwriteBehavior);
}

template <class T>
Sequence<T> Sequence<T>::tie(bool fillBeginning) { // TODO: this isn't working right??
    if (this->size() <= 1) {
        return *this;
    }
    bool tryAgain = false;
    vector<T> tiedEvents;
    for (auto event = this->begin(); event < this->end(); event++) {
        auto otherEvent = event + 1;
        if (otherEvent == this->end()) {
            event->duration = parent.endTime() - event->startTime;
            tiedEvents.push_back(*event);
            break;
        }
        
        if (event->startTime >= otherEvent->startTime) {
            DBG("aghh");
        }
        
        if (event == this->begin() && fillBeginning) { event->startTime = parent.startTime; }
        event->duration = otherEvent->startTime - event->startTime;
        
        if (event->equalsExcludingTime(*otherEvent)) { // tie events together.
            T tiedEvent(*event);
            tiedEvent.startTime = event->startTime;
            tiedEvent.duration = otherEvent->endTime() - event->startTime; // event->duration + otherEvent->duration;
            tiedEvents.push_back(tiedEvent);
            tryAgain = true;
            event++;
        } else {
            tiedEvents.push_back(*event);
        }
    }

    this->assignEvents(tiedEvents);
    
    if (tryAgain) {
        this->tie();
    }
    
    return *this;
}

template <class T>
Sequence<T> Sequence<T>::legato() {
    if (this->size() <= 1) {
        return *this;
    }

    for (auto event = this->begin(); event < this->end(); event++) {
        auto nextEvent = event + 1;
        if (nextEvent == this->end()) {
            nextEvent = this->begin();
            if (parent.containsPartially(*event)) {
                event->duration = nextEvent->startTime + parent.duration - event->startTime;
            }
        } else {
            event->duration = nextEvent->startTime - event->startTime;
        }
    }
    
    return *this;
}


template <class T>
bool Sequence<T>::chopAfterDuration(Duration duration) {
    // here's another implementation...is it better? more efficient...
//    while (lastNote.startTime >= length) {
//        DBG ("added too many notes...???");
//        sequence.pop_back();
//    }
//
//    if (lastNote.endTime() > length) {
//        Duration lastNoteLength = length - lastNote.startTime;
//        lastNote.duration = lastNoteLength;
//    }
    
    
    if (this->empty()) { return true; }

    vector<T> filtered;
    copy_if (this->begin(),
             this->end(),
             back_inserter(filtered),
             [duration](T &t) { return t.startTime <= duration; });

    if (filtered.empty()) { return true; }

    vector<T> chopped;
    transform(filtered.begin(),
              filtered.end(),
              back_inserter(chopped),
              [duration](T &t) {
                bool tooLong = t.endTime() > duration;
                if (tooLong) {
                    t.duration =  duration - t.startTime;
                }
                return t; });

    this->assignEvents(chopped);
    return true;
}

template <class T>
Sequence<T> Sequence<T>::parseMininotation(std::string phraseString, Duration stepLength) { 
    Sequence<T> result(*this);
    result.clear();
    vector<T> parsed = Mininotation::parse<T>(phraseString, stepLength);
    for (T t : parsed) {
        result.add(t); // might be a better way to get similar result but this will handle duration overflow and stuff.
    }
//
//    auto symbolIter = phraseString.begin();
//    Duration length = stepLength * ((double) Mininotation::getLength(phraseString));
//    for(Position startTime = 0; startTime < length; startTime += stepLength) {
//        char symbol = *symbolIter++;
//
//        if (!Mininotation::isInNotation(symbol)) {
//            DBG ("misuse of mininotation");
//            continue;
//        }
//        if (symbol == Mininotation::rest) { continue; }
//
//
//        if (symbol == Mininotation::sustain) {
//            if (!result.events.empty()) {
//                result.events.back().duration += stepLength;
//            }
//            continue;
//        }
//
//        if (Mininotation::isNote(symbol)) {  // TODO: implement class-specific interpretations of mininotation symbols
//            T toAdd(symbol, startTime, stepLength);
//            result.add(toAdd);
//        }
//    }

    return result;
}

template <class T>
bool Sequence<T>::append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior) {
    return this->concat(Sequence<T>::parseMininotation(phraseString, stepLength), true, pushBehavior);
}

template <class T>
bool Sequence<T>::insertMininotation(std::string phraseString,
                                     Position startTime,
                                     Duration stepLength,
                                     PushBehavior pushBehavior,
                                     OverwriteBehavior overwriteBehavior)
{
    return this->insertSequence(Sequence<T>::parseMininotation(phraseString, stepLength), startTime, pushBehavior, overwriteBehavior);
}


template<class T>
vector<T> Sequence<T>::byPosition(Position position) const {
    vector<T> result;
    for (auto it = this->begin(); it < this->end(); it++) {
        if (it->contains(position)) {
            result.push_back(*it);
        }
    }
    return result;
};
template<class T>
vector<T*> Sequence<T>::pointersByPosition(Position position) {
    vector<T*> result;
    for (auto it = this->begin(); it < this->end(); it++) {
        if (it->contains(position)) {
            result.push_back(&(*it));
        }
    }
    return result;
};
// template<class T>
// vector<T&> Sequence<T>::refsByPosition(Position position) const {
//     vector<T&> result;
//     for (auto it = this->begin(); it < this->end(); it++) {
//         if (it->contains(position)) {
//             result.push_back(*it);
//         }
//     }
//     return result;
// };

template<class T>
vector<T> Sequence<T>::bySpan(Timed span) const {
    vector<T> result;
    for (auto it = this->begin(); it < this->end(); it++) {
        if (span.contains(it->startTime)) {
            result.push_back(*it);
        }
    }
    return result;
}
template<class T>
vector<reference_wrapper<T>> Sequence<T>::refsBySpan(Timed span) {
    vector<reference_wrapper<T>> result;
    for (auto it = this->begin(); it < this->end(); it++) {
        if (span.contains(it->startTime)) {
            result.push_back(reference_wrapper<T>(*it));
        }
    }
    return result;
}

template<class T>
vector<T> Sequence<T>::byStartPosition(Position position) const {
    vector<T> result;
    for (auto it = this->begin(); it < this->end(); it++) {
        if (it->startTime == position) {
            result.push_back(*it);
        }
    }
    return result;
}

template<class T>
T Sequence<T>::drawByPosition(Position position) const {
    auto available = this->byPosition(position);
    if (available.empty()) {
        DBG ("Nothing to draw from :(");
        return T();
    }
    return draw<T>(available);
}


std::string doubleToString(double d) {
    char s[20];
    sprintf(s,"%.2f",d);
    return std::string(s);
}

template<class T>
std::string Sequence<T>::getStartTimesString() {
    std::string startTimesString;
    std::string delimiter(", ");
    
    for (auto eventIt = this->begin(); eventIt < this->end(); eventIt++) {
        startTimesString.append(doubleToString(eventIt->startTime.asBeats()));
        if (eventIt + 1 != this->end()) {
          startTimesString.append(delimiter);
        }
    }
    
    return startTimesString;
}


template <class T>
Sequence<T> Sequence<T>::pulseAndDisplace(Duration pulse,
                             Duration displacement,
                             Probability pDisplace,
                             Probability pDouble,
                             Duration length) const
{
    auto decideToDisplace = [&]() { return flipWeightedCoin(pDisplace); };
    auto decideToDouble = [&]() { return flipWeightedCoin(pDouble); };
    
    Sequence<T> sequence(*this);
    sequence.clear();

    // generate
    do {
        if (decideToDisplace()) {
            if (decideToDouble()) {
                sequence.append("x", displacement);
            } else if (sequence.size() > 0) {
                sequence.back().duration += displacement;
            }
        }
        sequence.append("x", pulse);
    } while (sequence.endTime() < length);

    // clean up.
    sequence.chopAfterDuration(length);
    
    // delete this 'test' code eventually
    if (sequence.endTime() != length) {
        DBG ("phrase doesn't end at right time...");
    }
    
    return sequence;
}


template class Sequence<Note>;
template class Sequence<Subdivision>;
template class Sequence<ChordScale>;
template class Sequence<Timed>;
