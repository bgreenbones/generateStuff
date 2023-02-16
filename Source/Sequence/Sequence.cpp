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

template <class T>
bool Sequence<T>::flip() {
    auto flipTime = [this](std::__wrap_iter<T*> t) { return (t->startTime + (parent.duration / 2.)) % parent.duration; };
    for (auto event = events.begin(); event < events.end(); event++) {
        event->startTime = flipTime(event);
    }
    return true;
}

template <class T>
bool Sequence<T>::add(T toAdd, PushBehavior pushBehavior, bool overwrite) {
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
        if (overwrite) {
            events.erase(std::remove_if(events.begin(), events.end(),
                                        [toAdd](T t) { return toAdd.containsPartially(t) || t.containsPartially(toAdd); }),
                                        events.end());
//            std::erase_if(events, [toAdd](T t) { return toAdd.containsPartially(t) || t.containsPartially(toAdd); });
        } else {
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
bool Sequence<T>::insert(vector<T> other, Position startTime, PushBehavior pushBehavior, bool overwrite) {
    for (auto iter = other.begin(); iter < other.end(); iter++) {
            iter->startTime += startTime;
        if (!(this->add(*iter, pushBehavior, overwrite))) {
            DBG("problem inserting sequence");
            return false;
        }
    }
    return true;
}


template <class T>
bool Sequence<T>::insert(Sequence<T> other, Position startTime, PushBehavior pushBehavior, bool overwrite) {
    return insert(other.events, startTime, pushBehavior, overwrite);
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

template<class T>
T Sequence<T>::drawByPosition(Position position) const {
    auto available = this->byPosition(position);
    Subdivision subdivision;
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
    
    for (auto eventIt = this->events.begin(); eventIt < this->events.end(); eventIt++) {
        startTimesString.append(doubleToString(eventIt->startTime.asBeats()));
        if (eventIt + 1 != this->events.end()) {
          startTimesString.append(delimiter);
        }
    }
    
    return startTimesString;
}

template class Sequence<Note>;
template class Sequence<Subdivision>;
