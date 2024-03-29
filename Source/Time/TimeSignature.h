/*
  ==============================================================================

    TimeSignature.h
    Created: 29 Aug 2022 10:46:19pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

typedef float tempo;
typedef double seconds; // depends on tempo
typedef double beats; // depends on time signature
typedef double bars; // depends on time signature
typedef double quarters; // always measured in quarters

class TimeSignature {
public:
    TimeSignature();
    TimeSignature(int numerator);
    TimeSignature(int numerator, int denominator);
    TimeSignature operator=(TimeSignature other);
    bool operator!=(TimeSignature other) { return numerator != other.numerator || denominator != other.denominator; }
    
    int numerator;
    int denominator;
    
    beats beatsPerQuarter() const;
    quarters quartersPerBar() const;
    quarters beatLengthInQuarters() const;
    quarters barLengthInQuarters() const;
    
    beats barsToBeats(bars bars) const;
    beats quartersToBeats(quarters quarters) const;
    bars beatsToBars(beats beats) const;
    bars quartersToBars(quarters quarters) const;
    quarters beatsToQuarters(beats beats) const;
    quarters barsToQuarters(bars bars) const;
private:
    static bool guard(int numerator, int denominator);
};
