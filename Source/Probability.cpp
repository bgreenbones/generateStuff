/*
  ==============================================================================

    Probability.cpp
    Created: 27 Aug 2022 8:12:06pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Probability.h"
#include <algorithm>
#include <random>

using namespace std;

Probability::Probability() {
    value = 0.0;
}

double Probability::guard(double value) {
    return value < 0.0 ? 0.0 : (value > 1.0 ? 1.0 : value);
}

Probability::Probability(double value) { this->value = guard(value); }
Probability::Probability(Probability const& other) { this->value = other.value; }
Probability Probability::operator=(Probability other) { swap(value, other.value); return *this; }
Probability Probability::operator=(double other) { this->value = guard(other); return *this; }
bool Probability::operator>(Probability other) { return this->value > other.value; }
bool Probability::operator>(double other) { return this->value > other; }
bool Probability::operator>=(Probability other) { return this->value >= other.value; }
bool Probability::operator>=(double other) { return this->value >= other; }
bool Probability::operator==(Probability other) { return this->value == other.value; }
bool Probability::operator==(double other) { return this->value == other; }
bool Probability::operator<=(Probability other) { return this->value <= other.value; }
bool Probability::operator<=(double other) { return this->value <= other; }
bool Probability::operator<(Probability other) { return this->value < other.value; }
bool Probability::operator<(double other) { return this->value < other; }
Probability Probability::operator+(Probability other) { return Probability(this->value + other.value); }
Probability Probability::operator+(double other) { return *this + Probability(other); }
Probability Probability::operator-(Probability other) { return Probability(this->value - other.value); }
Probability Probability::operator-(double other) { return *this - Probability(other); }
Probability Probability::operator*(Probability other) { return Probability(this->value * other.value); }
Probability Probability::operator*(double other) { return *this * Probability(other); }
Probability Probability::operator/(Probability other) { return Probability(this->value / other.value); }
Probability Probability::operator/(double other) { return *this / Probability(other); }
Probability::operator bool() const { return (rand() % 100) <= this->value * 100; } // todo: finer resolution
