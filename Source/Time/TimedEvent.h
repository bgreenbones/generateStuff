/*
  ==============================================================================

    TimedEvent.h
    Created: 12 Sep 2022 12:36:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"

class Timed {
public:
    Position startTime;
    Duration duration;
    HostSettings &settings;

    void setStartTime(Position newStartTime) {
        duration = (newStartTime < endTime()) ? endTime() - newStartTime : Duration(0);
        startTime = newStartTime;
    }
    void setEndTime(Position newEndTime) {
        duration = newEndTime > startTime ? newEndTime - startTime : Duration(0);
    }
    void shiftByDuration(Duration toShiftBy) {
        startTime += toShiftBy;        
    }
    
    Timed(Position startTime, Duration duration):
        startTime(startTime),
        duration(duration),
        settings(HostSettings::instance()) { }
    Timed(): Timed(0, 1) { }
    Timed(Timed const& other): Timed(other.startTime, other.duration) {}
//        startTime(other.startTime),
//        duration(other.duration),
//        settings(HostSettings::instance()) { }

    Timed(char mininotation, Position startTime, Duration duration) : Timed(startTime, duration) {}
    Timed& operator=(Timed const& other) {
        startTime = other.startTime;
        duration = other.duration;
        return *this;
    };
    bool operator==(Timed const& other) const { return startTime == other.startTime && duration == other.duration; };
    bool operator<(Timed const& other) const { return startTime < other.startTime || duration > other.duration; };
    bool operator>(Timed const& other) const { return startTime > other.startTime || duration > other.duration; };
    
    virtual ~Timed() {};
    
    Position endTime() const { return this->startTime + this->duration; }
    bool contains(const Position position) const { return this->startTime <= position && this->endTime() > position; }
    bool containsPartially(const Timed &other) const {
        return (this->startTime <= other.startTime && this->endTime() > other.startTime)
            || (other.startTime <= this->startTime && other.endTime() > this->startTime); }
    bool containsCompletely(const Timed &other) const {
        return (this->startTime <= other.startTime && this->endTime() >= other.endTime()); }
    
    // TODO: siighhh ideally this is like, pure virtual, but then inheriting classes implementing it actually take their SPECIFIC class as argument, not a TimedEvent
    // also we need instances of TimedEvent so it doesn't really work as purevirtual
//    virtual bool equalsExcludingTime(TimedEvent &other) {
//        DBG("...should this be pure virtual? but then timed event is an abstract class..which makes some things not work..");
//        return false;
//    };//= 0;
    virtual bool equalsExcludingTime(Timed &other) const { return true; };

};

static const Timed nullTime(0,0);



template<class T> // must be TimedEvent
std::vector<T> byPosition(std::vector<T> events, Position position) { // todo: remove this in favor of Sequence implementation.
    static_assert(std::is_base_of<Timed, T>::value, "T not derived from TimedEvent");
    std::vector<T> result;
    for (auto it = events.begin(); it < events.end(); it++) {
        if (it->contains(position)) {
            result.push_back(*it);
        }
    }
    return result;
};

template<class T> // must be TimedEvent
T longest(std::vector<T> events) {
    static_assert(std::is_base_of<Timed, T>::value, "T not derived from TimedEvent");
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



