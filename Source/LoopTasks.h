/*
  ==============================================================================

    LoopTasks.h
    Created: 5 Feb 2023 11:12:21am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <string>
#include <functional>
#include <map>
#include <vector>

using std::vector, std::string, std::function, std::map;


class LoopTask {
private:
    bool active;
    function<void()> task;
public:
    LoopTask(function<void()> task, bool active): active(active), task(task) {}
    LoopTask(function<void()> task): LoopTask(task, true) {}
    
    bool execute() {
        if (active) {
            task();
        }
        return active;
    }
    
    void activate() { active = true; };
    void deactivate() { active = false; };
};

class LoopTasks {
private:
    bool tasksComplete = false;
    map<string, LoopTask> tasks;
public:
    void queue(string id, function<void()> task, bool active = true) {
        auto result = tasks.emplace(id, LoopTask(task, active));
        if (result.second) return;
        tasks.at(id) = LoopTask(task, active);
    };
    void complete() { tasksComplete = true; };
    void schedule() { tasksComplete = false; };
    bool isScheduled() { return !tasksComplete; }
    bool isComplete() { return tasksComplete; }
    void deactivate(const vector<const string> ids) {
        for (const string &id : ids) {
            if (tasks.find(id) != tasks.end()) {
                tasks.at(id).deactivate();
            }
        }
    }
    void activate(const vector<const string> ids) {
        for (const string &id : ids) {
            if (tasks.find(id) != tasks.end()) {
                tasks.at(id).activate();
            }
        }
    }
    
    void performTasks() {
        if (!tasksComplete) {
            for (auto taskIt = tasks.begin(); taskIt != tasks.end(); ++taskIt) {
                LoopTask task = taskIt->second;
                task.execute();
            }
            complete();
        }
    }
};
