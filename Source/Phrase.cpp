//
//  Phrase.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#include "Phrase.hpp"

Phrase Phrase::defaultPhrase = Phrase();

Phrase::Phrase(float bars, float beats, juce::AudioPlayHead::TimeSignature timeSignature,
               float offset, short bar) {
    this->bars = bars;
    if (beats >= timeSignature.numerator) { throw exception(); }
    this->beats = beats;
    this->timeSignature = timeSignature;
    this->offset = offset;
    this->bar = bar;
}

Phrase& Phrase::operator=(Phrase other) {
    swap(bars, other.bars);
    swap(beats, other.beats);
    swap(timeSignature, other.timeSignature);
    swap(offset, other.offset);
    swap(bar, other.bar);
    return *this;
};

float Phrase::length() {
    float quartersPerBeat = (4.0 / (float) this->timeSignature.denominator);
    float beatsPerBar = this->timeSignature.numerator;
    float beats = this->bars * beatsPerBar + this->beats;
    return beats * quartersPerBeat;
}

double Phrase::ppqUntilBarStart(double ppqPosition) {
    return (double) bar - ppqPosition;
}
