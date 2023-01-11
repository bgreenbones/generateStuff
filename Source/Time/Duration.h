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
    Duration(double value): Duration(value, currentTimeSignature()) {}
    Duration(): Duration(0) {} //settings(HostSettings::instance()), timeSignature(currentTimeSignature()), durationValueInQuarters(0) {}
    
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
    bool operator>(const Duration other) { return this->asQuarters() > other.asQuarters(); }
    bool operator>(double other) { return *this > Duration(other, this->getTimeSignature()); }
    bool operator>=(const Duration other) { return this->asQuarters() >= other.asQuarters(); }
    bool operator>=(double other) { return *this >= Duration(other, this->getTimeSignature()); }
    bool operator==(const Duration other)  { return this->asQuarters() == other.asQuarters(); }
    bool operator==(double other) { return *this == Duration(other, this->getTimeSignature()); }
    bool operator!=(const Duration other)  { return this->asQuarters() != other.asQuarters(); }
    bool operator!=(double other) { return *this != Duration(other, this->getTimeSignature()); }
    bool operator<=(const Duration other) { return this->asQuarters() <= other.asQuarters(); }
    bool operator<=(double other) { return *this <= Duration(other, this->getTimeSignature()); }
    bool operator<(const Duration other) { return this->asQuarters() < other.asQuarters(); }
    bool operator<(double other) { return *this < Duration(other, this->getTimeSignature()); }
    Duration operator+(const Duration other) { return Duration(this->asQuarters() + other.asQuarters(), this->getTimeSignature()); }
    Duration operator+(const double other) { return *this + Duration(other, this->getTimeSignature()); }
    Duration operator-(const Duration other) { return Duration(this->asQuarters() - other.asQuarters(), this->getTimeSignature()); }
    Duration operator-(const double other) { return *this - Duration(other, this->getTimeSignature()); }
    Duration operator*(const double other) { return Duration(this->durationValueInQuarters * other, this->getTimeSignature()); }
    Duration operator/(const Duration other) { return Duration(this->asQuarters() / other.asQuarters(), this->getTimeSignature()); }
    Duration operator/(const double other) { return Duration(this->durationValueInQuarters / other, this->getTimeSignature()); }
    Duration operator%(const Duration other) { return Duration(fmodf(this->asQuarters(), other.asQuarters()), this->getTimeSignature()); }
    Duration operator%(const double other) { return Duration(fmodf(this->asQuarters(), other), this->getTimeSignature()); }
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
    beats asBeats() const { return getTimeSignature().quartersToBeats(asQuarters()); }//.beatsPerQuarter() * this->asQuarters(); }
    bars asBars() const { return getTimeSignature().quartersToBars(asQuarters()); }//this->asQuarters() / getTimeSignature().barLengthInQuarters(); }
    quarters asQuarters() const { return this->durationValueInQuarters; }

    operator Beats() const;
    operator Bars() const;
    operator double() const { return this->asQuarters(); };
};

class Beats: public Duration {
protected:
    double durationValueInBeats; // in beats...always?
public:
    Beats(beats value, TimeSignature timeSignature):
        Duration(timeSignature.beatsToQuarters(value), timeSignature) {}
    Beats(double value): Beats(value, currentTimeSignature()) {}
    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature) {}
    Beats(): Beats(1.0, currentTimeSignature()) {}
    Beats operator=(const double other) {
        this->timeSignatureValue = currentTimeSignature();
        this->durationValueInQuarters = getTimeSignature().beatsToQuarters(other);
        return *this;
    }
    
    beats asBeats() const { return durationValueInBeats; }
    bars asBars() const { return getTimeSignature().beatsToBars(asBeats()); }
    quarters asQuarters() const { return getTimeSignature().beatsToQuarters(asBeats()); }
};

class Bars: public Duration {
protected:
    double durationValueInBars;
public:
    Bars(bars value, TimeSignature timeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature) {}
    Bars(double value): Bars(value, currentTimeSignature()) {}
    Bars(TimeSignature timeSignature): Bars(1.0, timeSignature) {}
    Bars(): Bars(1.0, currentTimeSignature()) {}
    Bars operator=(const double other) {
        this->timeSignatureValue = currentTimeSignature();
        this->durationValueInQuarters = getTimeSignature().barsToQuarters(other);
        return *this;
    }
    
    beats asBeats() const { return getTimeSignature().barsToBeats(asBars()); }
    bars asBars() const { return durationValueInBars; }
    quarters asQuarters() const { return getTimeSignature().barsToQuarters(asBars()); }
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
