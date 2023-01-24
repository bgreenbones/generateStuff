/*
  ==============================================================================

    Duration.h
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

//#include <stdio.h>
#include <JuceHeader.h>
#include "TimeSignature.h"
#include <algorithm>
#include <math.h>
#include "../HostSettings.h"

//static TimeSignature currentTimeSignature() {
//    return HostSettings::instance().getTimeSignature();
//}

class Duration;
typedef Duration Position; // same thing, for now
typedef Duration Quarters;
class Beats;
class Bars;


class Duration {
private:
    HostSettings const& settings;
    
    bool guard() {
        if (durationValueInQuarters < 0) {
            DBG ("negative duration value...");
        }
        return true;
    }
protected:
    TimeSignature currentTimeSignature() const {
        return this->settings.getTimeSignature();
    }
    double currentTempo() const {
        return this->settings.getTempo();
    }
    double durationValueInQuarters; // in quarters, always
    bool dynamicTimeSignature = false; // todo: make durations that change NICELY with the host's time signature.
    TimeSignature timeSignatureValue;
public:
    Duration(double value, bool dynamicTimeSignature):
        settings(HostSettings::instance()),
        durationValueInQuarters(value),
        dynamicTimeSignature(dynamicTimeSignature),
        timeSignatureValue(currentTimeSignature()) { guard(); }
    Duration(double value, TimeSignature timeSignature):
        settings(HostSettings::instance()),
        durationValueInQuarters(value),
        dynamicTimeSignature(false),
        timeSignatureValue(timeSignature) { guard(); }
    Duration(double value): Duration(value, false) {}
    Duration(): Duration(0) {}
    virtual ~Duration() = default;
    
    TimeSignature getTimeSignature() const {
        return dynamicTimeSignature ? currentTimeSignature() : timeSignatureValue;
    }
    
    Duration &operator=(Duration const& other) {
      if (this == &other) { return *this; }
      this->dynamicTimeSignature = other.dynamicTimeSignature;
      this->durationValueInQuarters = other.durationValueInQuarters;
      this->timeSignatureValue = other.timeSignatureValue;
      return *this;
    }
    
    Duration &operator=(Bars const& other);
    Duration &operator=(Beats const& other);
    
    Duration operator=(double other) {
      this->durationValueInQuarters = other;
      return *this;
    }
    
    bool operator>(const Duration other) const { return this->asQuarters() > other.asQuarters(); } // TODO: just use snowball operator or whatever it's called
    bool operator>(double other) const { return *this > Duration(other, this->getTimeSignature()); }
    bool operator>=(const Duration other) const { return this->asQuarters() >= other.asQuarters(); }
    bool operator>=(double other) const { return *this >= Duration(other, this->getTimeSignature()); }
    bool operator==(const Duration other) const  { return this->asQuarters() == other.asQuarters(); }
    bool operator==(double other) const { return *this == Duration(other, this->getTimeSignature()); }
    bool operator!=(const Duration other) const  { return this->asQuarters() != other.asQuarters(); }
    bool operator!=(double other) const { return *this != Duration(other, this->getTimeSignature()); }
    bool operator<=(const Duration other) const { return this->asQuarters() <= other.asQuarters(); }
    bool operator<=(double other) const { return *this <= Duration(other, this->getTimeSignature()); }
    bool operator<(const Duration other) const { return this->asQuarters() < other.asQuarters(); }
    bool operator<(double other) const { return *this < Duration(other, this->getTimeSignature()); }
    Duration operator+(const Duration other) const { return Duration(this->asQuarters() + other.asQuarters(), this->getTimeSignature()); }
    Duration operator+(const double other) const { return *this + Duration(other, this->getTimeSignature()); }
    Duration operator-(const Duration other) const { return Duration(this->asQuarters() - other.asQuarters(), this->getTimeSignature()); }
    Duration operator-(const double other) const { return *this - Duration(other, this->getTimeSignature()); }
//    Duration operator*(const Duration other) const { return Duration(this->asQuarters() * other.asQuarters(), this->getTimeSignature()); }
    Duration operator*(const double other) { return Duration(this->durationValueInQuarters * other, this->getTimeSignature()); }
//    Duration operator/(const Duration other) const { return Duration(this->asQuarters() / other.asQuarters(), this->getTimeSignature()); }
    Duration operator/(const double other) { return Duration(this->durationValueInQuarters / other, this->getTimeSignature()); }
    Duration operator%(const Duration other) const { return Duration(fmodf(this->asQuarters(), other.asQuarters()), this->getTimeSignature()); }
    Duration operator%(const double other) const { return Duration(fmodf(this->asQuarters(), other), this->getTimeSignature()); }
//    Duration& operator++() { return (*this) + 1.0; }
    Duration operator++(int) { return (*this) + 1.0; }
//    Duration& operator--() { return &((*this) - 1.0); }
    Duration operator--(int) { return (*this) - 1.0; }
    Duration operator+=(Duration other) {
      this->durationValueInQuarters += other.asQuarters();
      return *this;
    }
    Duration operator-=(Duration other) {
        this->durationValueInQuarters -= other.asQuarters();
        return *this;
    }

    beats beatsInLastBar() const { return asBeats() - wholeBars() * getTimeSignature().numerator; }
    bars wholeBars() const { return floor(asBars()); }
    virtual beats asBeats() const { return getTimeSignature().quartersToBeats(asQuarters()); }
    virtual bars asBars() const { return getTimeSignature().quartersToBars(asQuarters()); }
    virtual quarters asQuarters() const { return this->durationValueInQuarters; }
    virtual seconds asSeconds() const { return 60. * asBeats() / currentTempo(); }

    operator Beats() const;
    operator Bars() const;
    operator double() const { return this->asQuarters(); };
};

// TODO: all duration subtypes won't actually update the durationValueInQuarters with time sig changes and tempo changes. not sure what to do about.

class Beats: public Duration {
protected:
    double durationValueInBeats; // in beats...always?
public:
    Beats(beats value, bool dynamicTimeSignature):
        Duration(HostSettings::instance().getTimeSignature().beatsToQuarters(value), dynamicTimeSignature),
        durationValueInBeats(value) {}
    Beats(beats value, TimeSignature timeSignature):
        Duration(timeSignature.beatsToQuarters(value), timeSignature),
         durationValueInBeats(value) {}
    Beats(double value): Beats(value, false) {}
    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature) {}
    Beats(): Beats(1.0, false) {}
    Beats operator=(const double other) {
        this->durationValueInBeats = other;
        this->durationValueInQuarters = getTimeSignature().beatsToQuarters(other);
        return *this;
    }

    Beats &operator=(Duration const& other);
    Beats &operator=(Bars const& other);
    
    beats asBeats() const { return durationValueInBeats; }
    bars asBars() const { return getTimeSignature().beatsToBars(asBeats()); }
    quarters asQuarters() const { return getTimeSignature().beatsToQuarters(asBeats()); }
//    operator double() const { return this->asBeats(); };
};

class Bars: public Duration {
protected:
    double durationValueInBars;
public:
    Bars(bars value, bool dynamicTimeSignature):
        Duration(HostSettings::instance().getTimeSignature().barsToQuarters(value), dynamicTimeSignature),
        durationValueInBars(value) {}
    Bars(bars value, TimeSignature timeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature),
        durationValueInBars(value) {}
    Bars(double value): Bars(value, false) {}
    Bars(TimeSignature timeSignature): Bars(1.0, timeSignature) {}
    Bars(): Bars(1.0, false) {}
    Bars operator=(const double other) {
        this->durationValueInBars = other;
        this->durationValueInQuarters = getTimeSignature().barsToQuarters(other);
        return *this;
    }
    
    Bars &operator=(Duration const& other);
    Bars &operator=(Beats const& other);
    beats asBeats() const { return getTimeSignature().barsToBeats(asBars()); }
    bars asBars() const { return durationValueInBars; }
    quarters asQuarters() const { return getTimeSignature().barsToQuarters(asBars()); }
//    operator double() const { return this->asBars(); };
};

//class Quarters: public Duration {
//public:
//    Quarters(quarters value, TimeSignature timeSignature):
//        Duration(value, timeSignature) {}
//    Quarters(double value): Quarters(value, currentTimeSignature()) {}
//    Quarters(TimeSignature timeSignature): Quarters(1.0, timeSignature) {}
//    Quarters(): Quarters(1.0, currentTimeSignature()) {}
//    Quarters operator=(const double other) {
//        this->timeSignatureValue = currentTimeSignature();
//        this->durationValueInQuarters = other;
//        return *this;
//    }
//};



class Seconds: public Duration {
protected:
    double durationValueInSeconds;
public:
    static double secondsToQuarters(seconds value) {
        beats valueAsBeats = HostSettings::instance().getTempo() * (value / 60.);
        return HostSettings::instance().getTimeSignature().beatsToQuarters(valueAsBeats);
    }
    
    Seconds(bars value, bool dynamicTimeSignature):
        Duration(secondsToQuarters(value), dynamicTimeSignature),
        durationValueInSeconds(value) {}
    Seconds(bars value, TimeSignature timeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature),
        durationValueInSeconds(value) {}
    Seconds(double value): Seconds(value, true) {}
    Seconds(TimeSignature timeSignature): Seconds(1.0, timeSignature) {}
    Seconds(): Seconds(1.0, true) {}
    Seconds operator=(const double other) {
        this->durationValueInSeconds = other;
        this->durationValueInQuarters = getTimeSignature().barsToQuarters(other);
        return *this;
    }

    beats asBeats() const { return currentTempo() * (asSeconds() / 60.); }
    bars asBars() const { return getTimeSignature().beatsToBars(asBeats()); }
    quarters asQuarters() const { return getTimeSignature().beatsToQuarters(asBeats()); }
    seconds asSeconds() const { return durationValueInSeconds; }
//    operator double() const { return this->asBars(); };
};
