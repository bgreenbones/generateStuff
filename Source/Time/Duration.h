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

using namespace std;

class Beats;
class Bars;
class Quarters;

class Duration {
private:
    bool guard() {
        if (durationValueInQuarters < 0) {
            DBG ("negative duration value...");
        }
        return true;
    }
public:
    Duration(double value, bool dynamicTimeSignature):
        settings(HostSettings::instance()),
        dynamicTimeSignature(dynamicTimeSignature),
        timeSignatureValue(HostSettings::instance().getTimeSignature()),
        durationValueInQuarters(value) { guard(); }
    Duration(double value, TimeSignature timeSignature):
        settings(HostSettings::instance()),
        dynamicTimeSignature(false),
        timeSignatureValue(timeSignature),
        durationValueInQuarters(value){ guard(); }
    Duration(double value): Duration(value, HostSettings::instance().getTimeSignature()) {}
    Duration(): Duration(0) {} //settings(HostSettings::instance()), timeSignature(HostSettings::instance().getTimeSignature()), durationValueInQuarters(0) {}
    
    HostSettings &settings;
    bool dynamicTimeSignature = false; // todo: make durations that change NICELY with the host's time signature.
    TimeSignature timeSignatureValue;
    TimeSignature getTimeSignature() const {
        if (dynamicTimeSignature) {
            return settings.getTimeSignature();
        }
        return timeSignatureValue;
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
    beats asBeats() const { return getTimeSignature().beatsPerQuarter() * this->asQuarters(); }
    bars asBars() const { return this->asQuarters() / getTimeSignature().barLengthInQuarters(); }
    quarters asQuarters() const { return this->durationValueInQuarters; }

    operator Beats() const;
    operator Bars() const;
    operator Quarters() const;
    
    operator double() const { return this->asQuarters(); };
protected:
    double durationValueInQuarters; // in quarters, always
};

typedef Duration Position; // same thing, for now

class Beats: public Duration {
public:
    Beats(beats value, TimeSignature timeSignature):
        Duration(timeSignature.beatsToQuarters(value), timeSignature) {}
    Beats(double value): Beats(value, HostSettings::instance().getTimeSignature()) {}
    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature) {}
    Beats(): Beats(1.0, HostSettings::instance().getTimeSignature()) {}
    Beats operator=(const double other) {
        this->timeSignatureValue = HostSettings::instance().getTimeSignature();
        this->durationValueInQuarters = getTimeSignature().beatsToQuarters(other);
        return *this;
    }
};

class Bars: public Duration {
public:
    Bars(bars value, TimeSignature timeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature) {}
    Bars(double value): Bars(value, HostSettings::instance().getTimeSignature()) {}
    Bars(TimeSignature timeSignature): Bars(1.0, timeSignature) {}
    Bars(): Bars(1.0, HostSettings::instance().getTimeSignature()) {}
    Bars operator=(const double other) {
        this->timeSignatureValue = HostSettings::instance().getTimeSignature();
        this->durationValueInQuarters = getTimeSignature().barsToQuarters(other);
        return *this;
    }
};

class Quarters: public Duration {
public:
    Quarters(quarters value, TimeSignature timeSignature):
        Duration(value, timeSignature) {}
    Quarters(double value): Quarters(value, HostSettings::instance().getTimeSignature()) {}
    Quarters(TimeSignature timeSignature): Quarters(1.0, timeSignature) {}
    Quarters(): Quarters(1.0, HostSettings::instance().getTimeSignature()) {}
    Quarters operator=(const double other) {
        this->timeSignatureValue = HostSettings::instance().getTimeSignature();
        this->durationValueInQuarters = other;
        return *this;
    }
};
