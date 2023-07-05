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
#include <algorithm>
#include <compare>
#include <math.h>
#include "TimeSignature.h"
#include "HostSettings.h"

class Duration;
typedef Duration Position;
typedef Duration Quarters;
class Beats;
class Bars;

using std::function;

static TimeSignature getHostTimeSignature() { return HostSettings::instance().getTimeSignature(); }
static beats quartersToBeats(quarters q) { return getHostTimeSignature().quartersToBeats(q); };
static quarters beatsToQuarters(beats b) { return getHostTimeSignature().beatsToQuarters(b); };
static bars quartersToBars(quarters q) { return getHostTimeSignature().quartersToBars(q); };
static quarters barsToQuarters(bars b) { return getHostTimeSignature().barsToQuarters(b); };

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
    TimeSignature currentTimeSignature() const { return this->settings.getTimeSignature(); }
    double currentTempo() const { return this->settings.getTempo(); }

    double durationValueInQuarters;
    double durationValueInBeats;
    double durationValueInBars;
public:
    bool dynamicTimeSignature = false; // todo: make durations change NICELY with the host's time signature.
    TimeSignature timeSignatureValue;

    enum DurationBehavior { MaintainQuarters, MaintainBeats, MaintainBars };
    DurationBehavior behavior = DurationBehavior::MaintainQuarters;
    
    Duration(double value, bool dynamicTimeSignature):
        settings(HostSettings::instance()),
        durationValueInQuarters(value),
        durationValueInBeats(quartersToBeats(value)),
        durationValueInBars(quartersToBars(value)),
        dynamicTimeSignature(dynamicTimeSignature),
        timeSignatureValue(currentTimeSignature()) { guard(); }
    Duration(double value, TimeSignature timeSignature, bool dynamicTimeSignature):
        settings(HostSettings::instance()),
        durationValueInQuarters(value),
        durationValueInBeats(quartersToBeats(value)),
        durationValueInBars(quartersToBars(value)),
        dynamicTimeSignature(dynamicTimeSignature),
        timeSignatureValue(timeSignature) { guard(); }
    Duration(double value): Duration(value, false) {}
    Duration(): Duration(0) {}
    virtual ~Duration() = default;
    
    TimeSignature getTimeSignature() const {
        return dynamicTimeSignature ? currentTimeSignature() : timeSignatureValue;
    }
    
    Duration &operator=(Duration const& other) {
        if (this == &other) { return *this; }
        this->assignValues(other);
        this->behavior = other.behavior;
        return *this;
    }
    
    Duration &operator=(Bars const& other);
    Duration &operator=(Beats const& other);
    
    Duration operator=(double other) {
        this->durationValueInQuarters = other;
        this->durationValueInBeats = getTimeSignature().quartersToBeats(other);
        this->durationValueInBars = getTimeSignature().quartersToBars(other);
        return *this;
    }
    
private:    
    static constexpr double tolerance = 0.000001;
public:
    bool operator==(const Duration other) const  { return abs(asQuarters() - other.asQuarters()) < tolerance; }
    bool operator==(double other) const { return *this == Duration(other, getTimeSignature(), dynamicTimeSignature); }
    bool operator!=(const Duration other) const  { return !(*this == other); }
    bool operator!=(double other) const { return *this != Duration(other, getTimeSignature(), dynamicTimeSignature); }
    bool operator>(const Duration other) const { return asQuarters() - other.asQuarters() > tolerance; }
    bool operator>(double other) const { return *this > Duration(other, getTimeSignature(), dynamicTimeSignature); }
    bool operator>=(const Duration other) const { return *this > other || *this == other; }
    bool operator>=(double other) const { return *this >= Duration(other, getTimeSignature(), dynamicTimeSignature); }
    bool operator<(const Duration other) const { return other > *this; }
    bool operator<(double other) const { return *this < Duration(other, getTimeSignature(), dynamicTimeSignature); }
    bool operator<=(const Duration other) const { return *this < other || *this == other; }
    bool operator<=(double other) const { return *this <= Duration(other, getTimeSignature(), dynamicTimeSignature); }
    Duration operator+(const Duration other) const { return Duration(asQuarters() + other.asQuarters(), getTimeSignature(), dynamicTimeSignature); }
    Duration operator+(const double other) const { return *this + Duration(other, getTimeSignature(), dynamicTimeSignature); }
    Duration operator-(const Duration other) const { return Duration(asQuarters() - other.asQuarters(), getTimeSignature(), dynamicTimeSignature); }
    Duration operator-(const double other) const { return *this - Duration(other, getTimeSignature(), dynamicTimeSignature); }
    Duration operator*(const double other) { return Duration(asQuarters() * other, getTimeSignature(), dynamicTimeSignature); }
    double operator/(const Duration other) { return asQuarters() / other.asQuarters(); }
    Duration operator/(const double other) { return Duration(asQuarters() / other, getTimeSignature(), dynamicTimeSignature); }
    Duration operator%(const Duration other) const { return Duration(fmodf(asQuarters(), other.asQuarters()), getTimeSignature(), dynamicTimeSignature); }
    Duration operator%(const double other) const { return Duration(fmodf(asQuarters(), other), getTimeSignature(), dynamicTimeSignature); }
    Duration operator++(int) { return (*this) + 1.0; }
    Duration operator--(int) { return (*this) - 1.0; }
    Duration operator+=(Duration other) {
        this->durationValueInQuarters += other.asQuarters();
        this->durationValueInBeats += other.asBeats();
        this->durationValueInBars += other.asBars();
        return *this;
    }
    Duration operator-=(Duration other) {
        this->durationValueInQuarters -= other.asQuarters();
        this->durationValueInBeats -= other.asBeats();
        this->durationValueInBars -= other.asBars();
        return *this;
    }

    void assignValues(Duration const& other) {
        this->dynamicTimeSignature = other.dynamicTimeSignature;
        this->timeSignatureValue = other.timeSignatureValue;
        this->durationValueInQuarters = other.asQuarters();
        this->durationValueInBeats = other.asBeats();
        this->durationValueInBars = other.asBars();
        this->updateValues();
    }
    
    void updateValues() {
        if (dynamicTimeSignature) {
            if (timeSignatureValue != getTimeSignature()) { timeSignatureValue = getTimeSignature(); }
            this->durationValueInQuarters = this->asQuarters();
            this->durationValueInBeats = this->asBeats();
            this->durationValueInBars = this->asBars();
        }
    }
    
    beats beatsInLastBar() const { return asBeats() - wholeBars() * getTimeSignature().numerator; }
    bars wholeBars() const { return floor(asBars()); }
    virtual seconds asSeconds() const { return 60. * asBeats() / currentTempo(); }
    beats asBeats() const {
        switch(behavior) {
            case DurationBehavior::MaintainQuarters:
                return getTimeSignature().quartersToBeats(durationValueInQuarters);
            case DurationBehavior::MaintainBeats:
                return durationValueInBeats;
            case DurationBehavior::MaintainBars:
                return getTimeSignature().barsToBeats(durationValueInBars);
        }
    }
    bars asBars() const {
        switch(behavior) {
            case DurationBehavior::MaintainQuarters:
                return getTimeSignature().quartersToBars(durationValueInQuarters);
            case DurationBehavior::MaintainBeats:
                return getTimeSignature().beatsToBars(durationValueInBeats);;
            case DurationBehavior::MaintainBars:
                return durationValueInBars;
        }
    }
    quarters asQuarters() const {
        switch(behavior) {
            case DurationBehavior::MaintainQuarters:
                return durationValueInQuarters;
            case DurationBehavior::MaintainBeats:
                return getTimeSignature().beatsToQuarters(durationValueInBeats);
            case DurationBehavior::MaintainBars:
                return getTimeSignature().barsToQuarters(durationValueInBars);
        }
    }

    operator Beats() const;
    operator Bars() const;
    operator double() const { return this->asQuarters(); };

};


// TODO: all duration subtypes won't actually update the durationValueInQuarters with time sig changes and tempo changes. not sure what to do about.

class Beats: public Duration {
public:
    Beats(beats value, bool dynamicTimeSignature):
        Duration(beatsToQuarters(value), dynamicTimeSignature) {
            behavior = DurationBehavior::MaintainBeats;
        }
    Beats(beats value, TimeSignature timeSignature, bool dynamicTimeSignature):
        Duration(timeSignature.beatsToQuarters(value), timeSignature, dynamicTimeSignature) {
            behavior = DurationBehavior::MaintainBeats;
        }
    Beats(double value): Beats(value, false) {}
//    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature, false) {}
    Beats(): Beats(1.0, false) {}
    Beats operator=(const double other) {
        this->durationValueInBeats = other;
        this->durationValueInQuarters = getTimeSignature().beatsToQuarters(other);
        this->durationValueInBars = getTimeSignature().beatsToBars(other);
        return *this;
    }

    Beats &operator=(Duration const& other);
    Beats &operator=(Bars const& other);
    
//    operator double() const { return this->asBeats(); };
};

class Bars: public Duration {
public:
    Bars(bars value, bool dynamicTimeSignature):
        Duration(barsToQuarters(value), dynamicTimeSignature) {
            behavior = DurationBehavior::MaintainBars;
        }
    Bars(bars value, TimeSignature timeSignature, bool dynamicTimeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature, dynamicTimeSignature) {
            behavior = DurationBehavior::MaintainBars;
        }
    Bars(double value): Bars(value, false) {}
//    Bars(TimeSignature timeSignature): Bars(1.0, timeSignature, false) {}
    Bars(): Bars(1.0, false) {}
    Bars operator=(const double other) {
        this->durationValueInBars = other;
        this->durationValueInQuarters = getTimeSignature().barsToQuarters(other);
        this->durationValueInBeats = getTimeSignature().barsToBeats(other);
        return *this;
    }
    
    Bars &operator=(Duration const& other);
    Bars &operator=(Beats const& other);

//    operator double() const { return this->asBars(); };
};

// TODO: integrate this with the other duration types?
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
    Seconds(bars value, TimeSignature timeSignature, bool dynamicTimeSignature):
        Duration(timeSignature.barsToQuarters(value), timeSignature, dynamicTimeSignature),
        durationValueInSeconds(value) {}
    Seconds(double value): Seconds(value, true) {}
//    Seconds(TimeSignature timeSignature): Seconds(1.0, timeSignature, false) {}
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


static const Quarters triplets(1./3.);
static const Quarters sixteenths(1./4.);
static const Quarters quintuplets(1./5.);
static const Quarters thirtySeconds(1./8.);
