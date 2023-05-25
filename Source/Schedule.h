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

// class Schedule {
//     Form form;
// public:
//     Sequence<TimedEvent> scheduleTimes;
//     map<TimedEvent, vector<Phrase>> schedule;
//     Schedule() : scheduleTimes(form) {
//       scheduleTimes = scheduleTimes.toPolyphonic();
//       scheduleTimes.add(TimedEvent(form.startTime, form.duration));
//       schedule[scheduleTimes.back()] = {};
//     }
// 
//     void clear() {
//       schedule.clear();
//       scheduleTimes.clear();
//     }
// 
//     vector<Phrase> at(Quarters ppqPosition) {
//       vector<Phrase> result;
//     
//       vector<TimedEvent> times = scheduleTimes.byPosition(ppqPosition);
//       for (auto scheduleTime : times)
//       {
//         vector<Phrase> phrases = schedule.at(scheduleTime);
//         for (Phrase phrase : phrases)
//         {
//           result.push_back(phrase);
//         }
//       }
//       return result;
//     }
// 
//     void schedulePhrase(TimedEvent time, Phrase phrase) {
//       if (schedule.find(time) == schedule.end()) {
//         scheduleTimes.add(time);
//         schedule[time] = {};
//       }
//       phrase.schedule.emplace(time);
//       schedule.at(time).push_back(phrase);
//     }
//     
//     void unschedulePhrase(TimedEvent time, VoiceName voiceName) {
//       if (schedule.find(time) == schedule.end()) {
//         return;
//       }
//       
//       scheduleTimes.erase(std::remove_if(scheduleTimes.begin(), scheduleTimes.end(),
//                                 [time](TimedEvent t) { return time == t; }),
//                                 scheduleTimes.end());
//                                 
//       vector<Phrase>& scheduled = schedule[time];                          
//       scheduled.erase(std::remove_if(scheduled.begin(), scheduled.end(),
//                                 [voiceName](Phrase p) { return p.voice == voiceName; }),
//                                 scheduled.end());
//                                 
//       if (scheduled.empty()) {
//         schedule.erase(time);
//       }
//     }
// };




class VoiceSchedule {
    Form form;
public:
    Sequence<TimedEvent> scheduleTimes;
    map<TimedEvent, Phrase> schedule;
    VoiceSchedule() : scheduleTimes(form) {
      scheduleTimes = scheduleTimes.toMonophonic();
    }

    void clear() {
      schedule.clear();
      scheduleTimes.clear();
    }

    Phrase at(Quarters ppqPosition) {
      if (scheduleTimes.byPosition(ppqPosition).empty()) {
        return Phrase(); // todo: i don't know but this works for now
      }
      TimedEvent time = scheduleTimes.drawByPosition(ppqPosition);
      Phrase result = schedule.at(time);
      return result;
    }

    bool schedulePhrase(TimedEvent time, Phrase phrase) {
      if (schedule.find(time) == schedule.end()) {
        if (!scheduleTimes.add(time)) {
          return false;
        }
      }
      phrase.schedule.emplace(time);
      schedule[time] = phrase;
      return true;
    }
    
    void unschedulePhrase(TimedEvent time) {
      if (schedule.find(time) == schedule.end()) {
        return;
      }
      
      scheduleTimes.erase(std::remove_if(scheduleTimes.begin(), scheduleTimes.end(),
                                [time](TimedEvent t) { return time == t; }),
                                scheduleTimes.end());

      schedule.erase(time);
    }
};
