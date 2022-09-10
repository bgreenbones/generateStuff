//
//  Phrase.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#include "Phrase.hpp"
#include "../HostSettings.h"

//Phrase Phrase::defaultPhrase = Phrase();

Phrase::Phrase(): duration(Bars(2) + Beats(4)) {
    this->bars = 2;
    this->beats = 4;
    this->duration = Bars(this->bars) + Beats(this->beats);
    this->timeSignature = HostSettings::instance().getTimeSignature();
    this->offset = 0;
    this->bar = 0;
}


Phrase::Phrase(float bars, float beats, TimeSignature timeSignature,
               float offset, short bar): duration(Bars(bars) + Beats(beats)), timeSignature(timeSignature) {
    this->bars = bars;
    if (beats >= timeSignature.numerator) { throw exception(); }
    this->beats = beats;
    this->duration = Bars(bars, timeSignature) + Beats(beats, timeSignature);
//    this->duration = Bars(bars) + Beats(beats);
    this->offset = offset;
    this->bar = bar;
}

Phrase& Phrase::operator=(Phrase other) {
    swap(bars, other.bars);
    swap(beats, other.beats);
    swap(duration, other.duration);
    swap(timeSignature, other.timeSignature);
    swap(offset, other.offset);
    swap(bar, other.bar);
    return *this;
};


float Phrase::length() {
    float quartersPerBeat = this->timeSignature.beatLengthInQuarters();
    float beatsPerBar = this->timeSignature.numerator;
    float beats = this->bars * beatsPerBar + this->beats;
    return beats * quartersPerBeat;
}

double Phrase::ppqUntilBarStart(double ppqPosition) {
    return (double) bar - ppqPosition;
}
