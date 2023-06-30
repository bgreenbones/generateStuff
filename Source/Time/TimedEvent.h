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
    bool operator!=(Time const& other) const { return !((*this) == other); };
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

static vector<Time> findLoop(vector<Time> timed) {
    Position previous(0);
    Duration loopLength = Bars(1);
    sort(timed);
    while (loopLength < timed.back().startTime) {
        bool loopFound = true;

        vector<Time> loopToMatch = filter<Time>(timed, [loopLength](Time t) { return t.startTime < loopLength; });
        sort(loopToMatch);
        double i = 2.;
        while (i * loopLength < timed.back().startTime) {
            vector<Time> potentialMatch = filter<Time>(timed, [i, loopLength](Time t) {
                return t.startTime >= (i - 1) * loopLength &&
                        t.startTime < i * loopLength;
            });
            sort(potentialMatch);

            for (Time timeToMatch : loopToMatch) {
                for (Time potentialTimeMatch : potentialMatch) {
                    potentialTimeMatch.startTime -= (i - 1) * loopLength;
                    if (potentialTimeMatch != timeToMatch) {
                        loopFound = false;
                        break;
                    }
                }
                if(!loopFound) { break; }
            }
            if(!loopFound) { break; }
            i++;
        }
        if (loopFound) {
            return loopToMatch;
        }
        loopLength += Bars(1);
    }
    return timed;
}
// to be called after findLoop. this...makes a lot of assumptions and will probably break or crash if called wrong
// assumptions:
// timed and loop not empty
// timed has elements starting after loop ends
static Duration findLoopLength(vector<Time> timed, vector<Time> loop) {
    sort(timed);
    if (timed.size() == loop.size()) {
        return timed.back().endTime();
    }
    sort(loop);
    Time firstAfterLoop = filter<Time>(timed, [loop](Time t) { return t.startTime >= loop.back().endTime(); }).front();
    return firstAfterLoop.startTime - loop.front().startTime;
}

static vector<Time> concat(vector<Time> first, vector<Time> second, Duration firstDuration) {
    for(Time t : second) {
        t.startTime += firstDuration;
        first.push_back(t);
    }
    return first;
}

static vector<Time> loop(vector<Time> toLoop, Duration loopDuration, Duration resultDuration) {
    vector<Time> result = toLoop;
    Duration currentResultDuration = loopDuration;
    while(currentResultDuration < resultDuration) {
        result = concat(result, toLoop, currentResultDuration);
        currentResultDuration += loopDuration;
    }
    return result;
}

