/*
  ==============================================================================

    TimeSignature.cpp
    Created: 29 Aug 2022 10:46:19pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "TimeSignature.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

//TimeSignature TimeSignature::defaultTimeSignature = TimeSignature();
//TimeSignature TimeSignature::currentTimeSignature = TimeSignature();

bool TimeSignature::guard(int numerator, int denominator) {
    if (numerator <= 0) {
        throw exception();
    }
    if (denominator != 2 &&
        denominator != 4 &&
        denominator != 8 &&
        denominator != 16 &&
        denominator != 32) {
        throw exception();
    }
    return true;
}

TimeSignature TimeSignature::operator=(TimeSignature other) {
    swap(this->numerator, other.numerator);
    swap(this->denominator, other.denominator);
    return *this;
}

TimeSignature::TimeSignature() {
    this->numerator = 4;
    this->denominator = 4;
}

TimeSignature::TimeSignature(int numerator) {
    guard(numerator, 4);
    this->numerator = numerator;
    this->denominator = 4;
}

TimeSignature::TimeSignature(int numerator, int denominator) {
    guard(numerator, denominator);
    this->numerator = numerator;
    this->denominator = denominator;
}

beats TimeSignature::beatsPerQuarter() const {
    return (double) denominator / 4.0;
}

quarters TimeSignature::beatLengthInQuarters() const {
    return 4.0 / (double) denominator;
}

quarters TimeSignature::barLengthInQuarters() const {
    double quartersPerBeat = this->beatLengthInQuarters();
    double beatsPerMeasure = (double) (this->numerator);
    double quartersPerMeasure = beatsPerMeasure * quartersPerBeat;
    return (quarters) quartersPerMeasure;
}

quarters TimeSignature::quartersPerBar() const {
    return barLengthInQuarters();
}

beats TimeSignature::barsToBeats(bars bars) const { return  bars * numerator; }
beats TimeSignature::quartersToBeats(quarters quarters) const { return quarters * (denominator / 4.); }
bars TimeSignature::beatsToBars(beats beats) const { return beats / numerator; }
bars TimeSignature::quartersToBars(quarters quarters) const { return quarters / quartersPerBar(); }
quarters TimeSignature::beatsToQuarters(beats beats) const { return beats / beatsPerQuarter(); }
quarters TimeSignature::barsToQuarters(bars bars) const { return barLengthInQuarters() * bars; }
