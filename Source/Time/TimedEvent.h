/*
  ==============================================================================

    TimedEvent.h
    Created: 12 Sep 2022 12:36:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"
#include "Mininotation.h"

class Time {
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
    
    Time(Position startTime, Duration duration):
        startTime(startTime),
        duration(duration),
        settings(HostSettings::instance()) { }
    Time(): Time(0, 1) { }
    Time(Time const& other): Time(other.startTime, other.duration) {}
//        startTime(other.startTime),
//        duration(other.duration),
//        settings(HostSettings::instance()) { }

    Time(char mininotation, Position startTime, Duration duration) : Time(startTime, duration) {}
    Time& operator=(Time const& other) {
        startTime = other.startTime;
        duration = other.duration;
        return *this;
    };
    bool operator==(Time const& other) const { return startTime == other.startTime && duration == other.duration; };
    bool operator<(Time const& other) const { return startTime < other.startTime || (startTime == other.startTime && duration < other.duration); };
    bool operator>(Time const& other) const { return startTime > other.startTime || (startTime == other.startTime && duration > other.duration); };
    
    virtual ~Time() {};
    
    Position endTime() const { return this->startTime + this->duration; }
    bool contains(const Position position) const { return this->startTime <= position && this->endTime() > position; }
    bool containsPartially(const Time &other) const {
        return (this->startTime <= other.startTime && this->endTime() > other.startTime)
            || (other.startTime <= this->startTime && other.endTime() > this->startTime); }
    bool containsCompletely(const Time &other) const {
        return (this->startTime <= other.startTime && this->endTime() >= other.endTime()); }
    

    virtual bool equalsExcludingTime(Time const& other) const { return true; }; // TODO: remove when we can
};

static const Time nullTime(0,0);

template <typename T>
class Timed : public Time, public T {
// class Timed : public Time {
public:
    // T item;
    Timed(Time time, T t): Time(time), T(t) {}
    // Timed(Time time, T t): Time(time), item(t) {}
    Timed(const Time& time): Time(time) {}
    Timed(): Time(nullTime) {}
    Timed(char mininotation, Position startTime, Duration duration): Time(startTime, duration) {
        if (Mininotation::isValue(mininotation)) {
            DBG ("ok, good");
        } else {
            DBG ("i think we have to handle this");
        }
        
        if (Mininotation::isAlternate(mininotation)) {
            // accent();
            // if (accented != 1.0) {
            //     DBG ("i guess accented() called the const version");
            // }
        }
    }
    
    Timed<T> withDuration(Duration const& newDuration) const {
        Timed<T> modified = Timed<T>(*this);
        modified.duration = newDuration;
        return modified;
    }

    bool equalsExcludingTime(Timed<T> const& other) const { 
        // return item == other.item;
        return T::operator==(other.item);
    };
    
    Timed<T>& operator=(Timed<T> const& other) {
        Time::operator=(other);
        T::operator=(other);
        // this->item = other.item;
        return *this;
    };
    
    Timed<T>& operator=(Time const& other) {
        Time::operator=(other); 
        return *this;
    };
};



