/*
  ==============================================================================

    Duration.h
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

//#include <stdio.h>
#include "TimeTypedefs.h"
#include "TimeSignature.h"
#include <algorithm>
#include "../HostSettings.h"

using namespace std;

class Beats;
class Bars;
class Quarters;
//
//template<class T>
//class Duration {
//public:
//    Duration(double value, TimeSignature timeSignature): timeSignature(timeSignature), value(value) {}
//    Duration(double value): Duration(value, TimeSignature::defaultTimeSignature) {}
//
//    template <class U>
//    Duration(Duration<U> const&);
//    template <class U>
//    Duration<T>& operator=(const Duration<U>& other) {
//        swap(this->value, other.value);
//        swap(this->timeSignature, other.timeSignature);
//        return *this;
//    }
//    template <class U>
//    bool operator>(const Duration<U> other) { return this->asQuarters() > other.asQuarters(); }
//    bool operator>(double other) { return this > Duration<T>(other, this->timeSignature); }
//    template <class U>
//    bool operator>=(const Duration<U> other) { return this->asQuarters() >= other.asQuarters(); }
//    bool operator>=(double other) { return this >= Duration<T>(other, this->timeSignature); }
//    template <class U>
//    bool operator==(const Duration<U> other)  { return this->asQuarters() == other.asQuarters(); }
//    bool operator==(double other) { return this == Duration<T>(other, this->timeSignature); }
//    template <class U>
//    bool operator<=(const Duration<U> other) { return this->asQuarters() <= other.asQuarters(); }
//    bool operator<=(double other) { return this <= Duration<T>(other, this->timeSignature); }
//    template <class U>
//    bool operator<(const Duration<U> other) { return this->asQuarters() < other.asQuarters(); }
//    bool operator<(double other) { return this < Duration<T>(other, this->timeSignature); }
//    template <class U>
//    Duration<T> operator+(const Duration<U> other) { return Duration(this->asQuarters() + other.asQuarters(), this->timeSignature); }
//    Duration<T> operator+(const double other) { return this + Duration<T>(other, this->timeSignature); }
//    template <class U>
//    Duration<T> operator-(const Duration<U> other) { return Duration(this->asQuarters() - other.asQuarters(), this->timeSignature); }
//    Duration<T> operator-(const double other) { return this - Duration<T>(other, this->timeSignature); }
//    Duration<T> operator*(const double other) { return Duration(this->value * other, this->timeSignature); }
//    Duration<T> operator/(const double other) { return Duration(this->value / other, this->timeSignature); }
//    Duration<T>& operator++() { return this + 1.0; }
//    Duration<T> operator++(int) { return this + 1.0; }
//    Duration<T>& operator--() { return this - 1.0; }
//    Duration<T> operator--(int) { return this - 1.0; }
//
//    beats asBeats() const { return timeSignature.beatsPerQuarter() * this->asQuarters(); }
//    bars asBars() const { return timeSignature.barLengthInQuarters() / this->asQuarters(); }
//    quarters asQuarters() const { return this->value; }
//
//    operator Beats() const;
//    operator Bars() const;
//    operator Quarters() const;
//
//    TimeSignature timeSignature;
//protected:
//    double value; // in quarters, always
//};
//
//class Beats: public Duration<Beats> {
//public:
//    Beats(beats value, TimeSignature timeSignature):
//        Duration(timeSignature.beatsToQuarters(value), timeSignature) {}
//    Beats(double value): Beats(value, TimeSignature::defaultTimeSignature) {}
//    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature) {}
//    Beats(): Beats(1.0, TimeSignature::defaultTimeSignature) {}
//    Beats& operator=(const double other) {
//        this->timeSignature = TimeSignature::defaultTimeSignature;
//        this->value = timeSignature.beatsToQuarters(other);
//        return *this;
//    }
//};
//
//class Bars: public Duration<Bars> {
//public:
//    Bars(bars value, TimeSignature timeSignature):
//        Duration(timeSignature.barsToQuarters(value), timeSignature) {}
//    Bars(double value): Bars(value, TimeSignature::defaultTimeSignature) {}
//    Bars(TimeSignature timeSignature): Bars(1.0, timeSignature) {}
//    Bars(): Bars(1.0, TimeSignature::defaultTimeSignature) {}
//    Bars& operator=(const double other) {
//        this->timeSignature = TimeSignature::defaultTimeSignature;
//        this->value = timeSignature.barsToQuarters(other);
//        return *this;
//    }
//};
//
//class Quarters: public Duration<Quarters> {
//public:
//    Quarters(quarters value, TimeSignature timeSignature):
//        Duration(value, timeSignature) {}
//    Quarters(double value): Quarters(value, TimeSignature::defaultTimeSignature) {}
//    Quarters(TimeSignature timeSignature): Quarters(1.0, timeSignature) {}
//    Quarters(): Quarters(1.0, TimeSignature::defaultTimeSignature) {}
//    Quarters& operator=(const double other) {
//        this->timeSignature = TimeSignature::defaultTimeSignature;
//        this->value = other;
//        return *this;
//    }
//};
//
//template<class T>
//Duration<T>::operator Beats() const { return Beats(this->asBeats(), this->timeSignature); }
//template<class T>
//Duration<T>::operator Bars() const { return Bars(this->asBars(), this->timeSignature); }
//template<class T>
//Duration<T>::operator Quarters() const { return Quarters(this->asQuarters(), this->timeSignature); }



class Duration {
public:
    Duration(double value, TimeSignature timeSignature): timeSignature(timeSignature), value(value) {}
    Duration(double value): Duration(value, HostSettings::instance().getTimeSignature()) {}
    
    Duration(Duration const& other): Duration(other.value, other.timeSignature) {}
    Duration operator=(Duration other) {
      swap(value, other.value);
      swap(timeSignature, other.timeSignature);
      return *this;
    }
    bool operator>(const Duration other) { return this->asQuarters() > other.asQuarters(); }
    bool operator>(double other) { return *this > Duration(other, this->timeSignature); }
    bool operator>=(const Duration other) { return this->asQuarters() >= other.asQuarters(); }
    bool operator>=(double other) { return *this >= Duration(other, this->timeSignature); }
    bool operator==(const Duration other)  { return this->asQuarters() == other.asQuarters(); }
    bool operator==(double other) { return *this == Duration(other, this->timeSignature); }
    bool operator!=(const Duration other)  { return this->asQuarters() != other.asQuarters(); }
    bool operator!=(double other) { return *this != Duration(other, this->timeSignature); }
    bool operator<=(const Duration other) { return this->asQuarters() <= other.asQuarters(); }
    bool operator<=(double other) { return *this <= Duration(other, this->timeSignature); }
    bool operator<(const Duration other) { return this->asQuarters() < other.asQuarters(); }
    bool operator<(double other) { return *this < Duration(other, this->timeSignature); }
    Duration operator+(const Duration other) { return Duration(this->asQuarters() + other.asQuarters(), this->timeSignature); }
    Duration operator+(const double other) { return *this + Duration(other, this->timeSignature); }
    Duration operator-(const Duration other) { return Duration(this->asQuarters() - other.asQuarters(), this->timeSignature); }
    Duration operator-(const double other) { return *this - Duration(other, this->timeSignature); }
    Duration operator*(const double other) { return Duration(this->value * other, this->timeSignature); }
    Duration operator/(const double other) { return Duration(this->value / other, this->timeSignature); }
//    Duration& operator++() { return (*this) + 1.0; }
    Duration operator++(int) { return (*this) + 1.0; }
//    Duration& operator--() { return &((*this) - 1.0); }
    Duration operator--(int) { return (*this) - 1.0; }

    beats asBeats() const { return timeSignature.beatsPerQuarter() * this->asQuarters(); }
    bars asBars() const { return this->asQuarters() / timeSignature.barLengthInQuarters(); }
    quarters asQuarters() const { return this->value; }

    operator Beats() const;
    operator Bars() const;
    operator Quarters() const;
    operator double() const { return this->asQuarters(); };
    
    TimeSignature timeSignature;
protected:
    double value; // in quarters, always
};

class Beats: public Duration {
public:
    Beats(beats value, TimeSignature timeSignature):
        Duration(timeSignature.beatsToQuarters(value), timeSignature) {}
    Beats(double value): Beats(value, HostSettings::instance().getTimeSignature()) {}
    Beats(TimeSignature timeSignature): Beats(1.0, timeSignature) {}
    Beats(): Beats(1.0, HostSettings::instance().getTimeSignature()) {}
    Beats operator=(const double other) {
        this->timeSignature = HostSettings::instance().getTimeSignature();
        this->value = timeSignature.beatsToQuarters(other);
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
        this->timeSignature = HostSettings::instance().getTimeSignature();
        this->value = timeSignature.barsToQuarters(other);
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
        this->timeSignature = HostSettings::instance().getTimeSignature();
        this->value = other;
        return *this;
    }
};


