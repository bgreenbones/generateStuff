/*
  ==============================================================================

    Subdivision.cpp
    Created: 28 Aug 2022 11:31:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Subdivision.h"
#include <algorithm>


using namespace std;

Subdivision::Subdivision() {
    frequency = 1.0;
    span = duration { .bars = 0, .beats = 1};
}

double Subdivision::guard(double frequency) {
    return frequency <= 0.0 ? 1.0 : frequency;
    
}

Subdivision::Subdivision(double frequency) { this->frequency = guard(frequency); }
Subdivision::Subdivision(double frequency, duration span) {
    this->frequency = guard(frequency);
    this->span = span;
}
Subdivision::Subdivision(Subdivision const& other) {
    this->frequency = other.frequency;
    this->span = other.span;
}
Subdivision Subdivision::operator=(Subdivision other) {
    swap(this->frequency, other.frequency);
    swap(this->span, other.span);
    return *this;
}
Subdivision Subdivision::operator=(double other) {
    this->frequency = guard(other);
    this->span = duration { .bars = 0, .beats = 1};
    return *this;
}
bool Subdivision::operator>(Subdivision other) {
//    return this->frequency > other.frequency;
    return true; // todo: decide how
}
bool Subdivision::operator>(double other) { return this->frequency > other; }
bool Subdivision::operator>=(Subdivision other) { return this->frequency >= other.frequency; }
bool Subdivision::operator>=(double other) { return this->frequency >= other; }
bool Subdivision::operator==(Subdivision other) { return this->frequency == other.frequency; }
bool Subdivision::operator==(double other) { return this->frequency == other; }
bool Subdivision::operator<=(Subdivision other) { return this->frequency <= other.frequency; }
bool Subdivision::operator<=(double other) { return this->frequency <= other; }
bool Subdivision::operator<(Subdivision other) { return this->frequency < other.frequency; }
bool Subdivision::operator<(double other) { return this->frequency < other; }



Subdivision Subdivision::operator+(Subdivision other) {
    // todo: decide what you want to do. probably return another type, actually...Duration?
    return Subdivision(this->frequency + other.frequency);
}
Subdivision Subdivision::operator+(double other) { return Subdivision(this->frequency + other, this->span); }
//Subdivision Subdivision::operator-(Subdivision other) { return Subdivision(this->frequency - other.frequency); }
Subdivision Subdivision::operator-(double other) { return Subdivision(this->frequency - other, this->span); }
//Subdivision Subdivision::operator*(Subdivision other) { return Subdivision(this->frequency * other.frequency); }
Subdivision Subdivision::operator*(double other) { return Subdivision(this->frequency * other, this->span); }
//Subdivision Subdivision::operator/(Subdivision other) { return Subdivision(this->frequency / other.frequency); }
Subdivision Subdivision::operator/(double other) { return Subdivision(this->frequency / other, this->span); }
Subdivision Subdivision::operator++() { return Subdivision(this->frequency + 1.0, this->span); }
Subdivision Subdivision::operator--() { return Subdivision(this->frequency - 1.0, this->span); }
