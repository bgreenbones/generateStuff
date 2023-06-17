/*
  ==============================================================================

    Schedule.h
    Created: 5 May 2023 3:03:50pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once


#include <map>
#include "Phrase.hpp"
#include "FormMusical.h"


class VoiceSchedule {
public:
    Form form;
    Sequence<Time> scheduleTimes;
    map<Time, Phrase> schedule;
    VoiceSchedule() : scheduleTimes(form) {
      scheduleTimes = scheduleTimes.toMonophonic();
    }

    void clear() {
      schedule.clear();
      scheduleTimes.clear();
    }

    bool isScheduledAt(Quarters ppqPosition) const {
      return !scheduleTimes.byPosition(ppqPosition).empty();
    }
    Phrase const* at(Quarters ppqPosition) const {
      if (!isScheduledAt(ppqPosition)) {
        return nullptr;
      }
      Time time = scheduleTimes.drawByPosition(ppqPosition);
      Phrase const& result = schedule.at(time);
      return &result;
    }

    bool schedulePhrase(Time time, Phrase phrase) {
      phrase.schedule.emplace(time);
      if (find(scheduleTimes.begin(), scheduleTimes.end(), time) == scheduleTimes.end()) {
        if (!scheduleTimes.add(time)) {
          return false;
        }
        //  else {
        //     schedule[time] = phrase;
        //     return true;
        // }
      }
      schedule[time] = phrase;
//      schedule.at(time) = phrase;
      return true;
    }
    
    void unschedulePhrase(Time time) {
      if (schedule.find(time) == schedule.end()) {
        return;
      }
      
      scheduleTimes.erase(std::remove_if(scheduleTimes.begin(), scheduleTimes.end(),
                                [time](Time t) { return time == t; }),
                                scheduleTimes.end());

      schedule.erase(time);
    }
};
