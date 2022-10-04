/*
  ==============================================================================

    TimedEvent.h
    Created: 12 Sep 2022 12:36:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"


class TimedEvent {
public:
    Position startTime;
    Duration duration;
    
    TimedEvent(Position startTime, Duration duration): startTime(startTime), duration(duration) { }
    TimedEvent(): TimedEvent(0, 1) { }
    TimedEvent(TimedEvent const& other): startTime(other.startTime), duration(other.duration) { }
    TimedEvent& operator=(TimedEvent &other) {
        swap(startTime, other.startTime);
        swap(duration, other.duration);
        return *this;
    };
    virtual ~TimedEvent() {};
    
    void updateTimeSignature() {
        bars numBars = duration.wholeBars();
        beats beatsInLastBar = duration.beatsInLastBar();
        startTime.timeSignature = HostSettings::instance().getTimeSignature();
        duration.timeSignature = HostSettings::instance().getTimeSignature();
        duration = Bars(numBars) + Beats(beatsInLastBar);
    }
    
    Position endTime() const {
        return this->startTime + this->duration;
    }
    
    bool contains(const Position position) const {
        return this->startTime <= position && this->endTime() > position;
    }
    
    bool containsPartially(const TimedEvent &other) const {
        return this->startTime <= other.startTime && this->endTime() > other.startTime;
    }
    
    bool containsCompletely(const TimedEvent &other) const {
        return this->startTime <= other.startTime && this->endTime() >= other.endTime();
    }
    
    virtual bool equalsExcludingTime(TimedEvent &other) = 0;
};



// todo: this shoud be (static) member functions of timedevent??
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
