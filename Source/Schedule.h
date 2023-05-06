/*
  ==============================================================================

    Schedule.h
    Created: 5 May 2023 3:03:50pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once


#include <map>
#include "TimedEvent.h"
#include "Voice.h"


class Schedule {
    Form form;
public:
    Sequence<TimedEvent> scheduleTimes;
    map<TimedEvent, vector<Phrase>> schedule;
    Schedule() : scheduleTimes(form) {
      scheduleTimes = scheduleTimes.toPolyphonic();
      scheduleTimes.add(TimedEvent(form.startTime, form.duration));
      schedule[scheduleTimes.back()] = {};
    }

    void schedulePhrase(TimedEvent time, Phrase phrase) {
      if (schedule.find(time) == schedule.end()) {
        scheduleTimes.add(time);
        schedule[time] = {};
      }
      schedule.at(time).push_back(phrase);
    }
    
    void unschedulePhrase(TimedEvent time, VoiceName voiceName) {
      if (schedule.find(time) == schedule.end()) {
        return;
      }
      
      scheduleTimes.erase(std::remove_if(scheduleTimes.begin(), scheduleTimes.end(),
                                [other](TimedEvent t) { return time == t; }),
                                scheduleTimes.end());
                                
      vector<Phrase>& scheduled = schedule[time];                          
      scheduled.erase(std::remove_if(scheduled.begin(), scheduled.end(),
                                [other](Phrase p) { return p.voice == voiceName; }),
                                scheduled.end());
                                
      if (scheduled.empty()) {
        schedule.erase(time);
      }
    }
};