/*
  ==============================================================================

    HostSettings.h
    Created: 10 Sep 2022 12:19:45am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once


#include <iostream>
#include "Time/TimeSignature.h"

class HostSettings {
private:
    TimeSignature timeSignature = TimeSignature(4, 4);
    double tempo = 120;

    HostSettings() {
        tempo = 120;
        timeSignature = TimeSignature(4, 4);
    }
public:
    static HostSettings &instance() {
        static HostSettings settings;
        return settings;
    }

    HostSettings(HostSettings const&) = delete;
    void operator=(HostSettings const&) = delete;

    TimeSignature getTimeSignature() const {
        return this->timeSignature;
    }
    void setTimeSignature(TimeSignature const& newTimeSignature) {
        this->timeSignature = newTimeSignature;
    }

    double getTempo() const {
        return this->tempo;
    }
    void setTempo(double newTempo) {
        this->tempo = newTempo;
    }
};

//
//template <class T>
//class Singleton {
//protected:
//    T data;
//
//private:
//    Singleton<T>() { }
//public:
//    static Singleton<T> &getInstance() {
//        static Singleton<T> instance;
//        return instance;
//    }
//
//    Singleton<T>(Singleton<T> const&) = delete;
//    void operator=(Singleton<T> const&) = delete;
//};
//
//class HostSettingsData {
//public:
//    TimeSignature timeSignature;
//    double tempo;
//};
//
//class HostSettings: public Singleton<HostSettingsData> {
//    TimeSignature getTimeSignature() {
//        return data.timeSignature;
//    }
//    void setTimeSignature(TimeSignature newTimeSignature) {
//        data.timeSignature = newTimeSignature;
//    }
//
//    double getTempo() {
//        return data.tempo;
//    }
//    void setTempo(double tempo) {
//        data.tempo = tempo;
//    }
//};
