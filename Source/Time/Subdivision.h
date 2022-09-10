/*
  ==============================================================================

    Subdivision.h
    Created: 28 Aug 2022 11:31:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once
#include "TimeTypedefs.h"

class Subdivision {
public:
    Subdivision();
    Subdivision(double frequency);
    Subdivision(double frequency, duration span);
    Subdivision(Subdivision const&);
    Subdivision operator=(Subdivision other);
    Subdivision operator=(double other);
    bool operator>(Subdivision other);
    bool operator>(double other);
    bool operator>=(Subdivision other);
    bool operator>=(double other);
    bool operator==(Subdivision other);
    bool operator==(double other);
    bool operator<=(Subdivision other);
    bool operator<=(double other);
    bool operator<(Subdivision other);
    bool operator<(double other);
    Subdivision operator+(Subdivision other);
    Subdivision operator+(double other);
    Subdivision operator-(Subdivision other);
    Subdivision operator-(double other);
    Subdivision operator*(Subdivision other);
    Subdivision operator*(double other);
    Subdivision operator/(Subdivision other);
    Subdivision operator/(double other);
    Subdivision operator++();
    Subdivision operator--();
private:
    double guard(double frequency);
    double frequency;
    duration span;
};
