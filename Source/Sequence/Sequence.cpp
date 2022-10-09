/*
  ==============================================================================

    Sequence.cpp
    Created: 2 Oct 2022 6:38:09pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Sequence.h"
#include <JuceHeader.h>
#include "Mininotation.h"
#include "Note.hpp"

template <class T>
void Sequence<T>::updateTimeSignature() {
//    TimedEvent::updateTimeSignature();
    for (auto event = events.begin(); event < events.end(); event++) {
        event->updateTimeSignature();
    }
}


template <class T>
bool Sequence<T>::add(T toAdd, PushBehavior pushBehavior) {
    if (!(this->parent.containsPartially(toAdd))) {
        double phraseLength = this->parent.duration.asQuarters();
        double eventStartTime = toAdd.startTime.asQuarters();
        switch (pushBehavior) {
            case PushBehavior::truncate:
                return false;
            case PushBehavior::wrap:
                toAdd.startTime = std::fmod(eventStartTime, phraseLength);
                break;
            case PushBehavior::ignore:
                break;
            default:
                break;
        }
    }
    
    if (this->monophonic) {
        vector<T> bad_examples;
        copy_if(events.begin(),
                events.end(),
                back_inserter(bad_examples),
                [toAdd](T t) { return toAdd.containsPartially(t) || t.containsPartially(toAdd); });
        if (bad_examples.size() > 0) {
            DBG ("trying to add timed event where other events are in its way");
            return false;
        }
    }
    
    
    events.push_back(toAdd);
    sort(events.begin(),
         events.end(),
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
bool Sequence<T>::insert(Sequence<T> other, Position startTime, PushBehavior pushBehavior) {
    for (auto iter = other.begin(); iter < other.end(); iter++) {
            iter->startTime += startTime;
        if (!(this->add(*iter, pushBehavior))) {
            DBG("problem inserting sequence");
            return false;
        }
    }
    return true;
}

template <class T>
void Sequence<T>::tie() {
    if (events.size() <= 1) {
        return;
    }
    bool tryAgain = false;
    vector<T> tiedEvents;
    for (auto event = events.begin(); event < events.end() - 1; event++) {
        auto otherEvent = event + 1;
        if (event->equalsExcludingTime(*otherEvent)) {
            T tiedEvent(*event);
            tiedEvent.startTime = event->startTime;
            tiedEvent.duration = event->duration + otherEvent->duration;
            tiedEvents.push_back(tiedEvent);
            tryAgain = true;
        }
    }
    if (tryAgain) {
        this->events = tiedEvents;
        this->tie();
    }
}

template <class T>
void Sequence<T>::legato() {
    if (events.size() <= 1) {
        return;
    }
    for (auto event = events.begin(); event < events.end(); event++) {
        auto nextEvent = event + 1;
        if (nextEvent == events.end()) {
            nextEvent = events.begin();
            if (parent.containsPartially(*event)) {
                event->duration = nextEvent->startTime + parent.duration - event->startTime;
            }
        } else {
            event->duration = nextEvent->startTime - event->startTime;
        }
    }
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
    
    
    if (events.empty()) {
        return true;
    }

    vector<T> filtered;
    copy_if (events.begin(),
             events.end(),
             back_inserter(filtered),
             [duration](T &t) { return t.startTime <= duration; });

    if (filtered.empty()) {
        return true;
    }

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

    this->events = chopped;
    return true;
}

template <class T>
Sequence<T> Sequence<T>::parseMininotation(std::string phraseString, Duration stepLength) { 
    Sequence<T> result(*this);
    result.clear();
    
    auto symbolIter = phraseString.begin();
    Duration length = stepLength * ((double) Mininotation::getLength(phraseString));
    for(Position startTime = 0; startTime < length; startTime += stepLength) {
        char symbol = *symbolIter++;

        if (!Mininotation::isInNotation(symbol)) {
            DBG ("misuse of mininotation");
            continue;
        }
        if (symbol == Mininotation::rest) { continue; }
        
        
        if (symbol == Mininotation::sustain) {
            if (!result.events.empty()) {
                result.events.back().duration += stepLength;
            }
            continue;
        }

        if (Mininotation::isNote(symbol)) {  // TODO: implement class-specific interpretations of mininotation symbols
            T toAdd(symbol, startTime, stepLength);
            result.add(toAdd);
        }
    }

    return result;
}

template <class T>
bool Sequence<T>::append(std::string phraseString, Duration stepLength, PushBehavior pushBehavior) {
    return this->concat(Sequence<T>::parseMininotation(phraseString, stepLength), true, pushBehavior);
}


template<class T>
vector<T> Sequence<T>::byPosition(Position position) const {
    vector<T> result;
    for (auto it = events.begin(); it < events.end(); it++) {
        if (it->contains(position)) {
            result.push_back(*it);
        }
    }
    return result;
};


template class Sequence<Note>;
template class Sequence<Subdivision>;
