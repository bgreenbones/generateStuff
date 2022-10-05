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
bool Sequence<T>::add(T toAdd) {
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
bool Sequence<T>::concat(Sequence<T> other, bool useLast) {
    Duration endTime = useLast ? this->endTime() : parent->endTime();
    for (auto iter = other.events.begin(); iter < other.events.end(); iter++) {
        iter->startTime += endTime;
        if (!(this->add(*iter))) {
            DBG("problem concatenating sequences");
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
    Sequence<T> result;
    
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
        
        if (Mininotation::isNote(symbol)) {  // todo: implement class-specific interpretations of mininotation symbols
            T toAdd(startTime, stepLength);
            
            if (symbol == Mininotation::accentNote) {
                toAdd = toAdd.accent();
            }
            
            result.add(toAdd);
        }
    }

    return result;
}

template <class T>
bool Sequence<T>::append(std::string phraseString, Duration stepLength) {
    return this->concat(Sequence<T>::parseMininotation(phraseString, stepLength), true);
}


template class Sequence<Note>;
