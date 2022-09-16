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
    
    TimedEvent() { }
    TimedEvent(Position startTime, Duration duration): startTime(startTime), duration(duration) { }
    TimedEvent(TimedEvent const& other): startTime(other.startTime), duration(other.duration) { }
    TimedEvent& operator=(TimedEvent other) {
        swap(startTime, other.startTime);
        swap(duration, other.duration);
        return *this;
    };
    
    Position endTime() {
        return this->startTime + this->duration;
    }
    
    bool contains(Position position) {
        return this->startTime <= position && this->endTime() > position;
    }
    
    bool containsPartially(TimedEvent other) {
        return this->startTime <= other.startTime && this->endTime() > other.startTime;
    }
    
    bool containsCompletely(TimedEvent other) {
        return this->startTime <= other.startTime && this->endTime() >= other.endTime();
    }
};
