/*
  ==============================================================================

    TimedEvent.h
    Created: 12 Sep 2022 12:36:44am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Duration.h"
#include "Utility.h"

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
    
    void trim(Time const& container) {
        if (container.containsCompletely(*this)) { return; }
        if(startTime < container.startTime) {
            setStartTime(container.startTime);
        }
        if (endTime() > container.endTime()) {
            setEndTime(container.endTime());
        }
    }
    
};

static const Time nullTime(0,0);

template <typename T>
class Timed : public Time {
public:
    T item;
    Timed(Time time, T t): Time(time), item(t) {}
    Timed(const Time& time): Time(time) {}
    Timed(): Time(nullTime) {}

    static vector<Timed<T>> fromTimes(vector<Time> times) {
        return mapp<Time, Timed<T>>(times, [](Time time) { return Timed<T>(time); });
    }
    
    Timed<T> withDuration(Duration const& newDuration) const {
        Timed<T> modified = Timed<T>(*this);
        modified.duration = newDuration;
        return modified;
    }

    bool equalsExcludingTime(Timed<T> const& other) const { 
         return item == other.item;
    };
    
    Timed<T>& operator=(Timed<T> const& other) {
        Time::operator=(other);
        this->item = other.item;
        return *this;
    };
    
    Timed<T>& operator=(Time const& other) {
        Time::operator=(other); 
        return *this;
    };

    operator T() const { return item; }
};
