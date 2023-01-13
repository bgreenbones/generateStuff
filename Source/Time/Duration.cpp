/*
  ==============================================================================

    Duration.cpp
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Duration.h"
#include <algorithm>

Duration::operator Beats() const { return Beats(asBeats(), getTimeSignature()); }
Duration::operator Bars() const { return Bars(asBars(), getTimeSignature()); }
//Duration::operator Quarters() const { return Quarters(this->asQuarters(), this->getTimeSignature()); }


Duration& Duration::operator=(Beats const& other) {
    if (this == (Duration*)&other) { return *this; }
    this->dynamicTimeSignature = other.dynamicTimeSignature;
    this->durationValueInQuarters = other.durationValueInQuarters;
    this->timeSignatureValue = other.timeSignatureValue;

    return *this;
}

Duration& Duration::operator=(Bars const& other) {
    if (this == (Duration*)&other) { return *this; }
    this->dynamicTimeSignature = other.dynamicTimeSignature;
    this->durationValueInQuarters = other.durationValueInQuarters;
    this->timeSignatureValue = other.timeSignatureValue;
    return *this;
}


Beats& Beats::operator=(Duration const& other) {
    if (this == (Beats*)&other) { return *this; }
    this->durationValueInBeats = other.asBeats();
    return *this;
}

Bars& Bars::operator=(Duration const& other) {
    if (this == (Bars*)&other) { return *this; }
    this->durationValueInBars = other.asBars();
    return *this;
}

Beats& Beats::operator=(Bars const& other) {
    if (this == (Beats*)&other) { return *this; }
    this->durationValueInBeats = other.asBeats();
    return *this;
}

Bars& Bars::operator=(Beats const& other) {
    if (this == (Bars*)&other) { return *this; }
    this->durationValueInBars = other.asBars();
    return *this;
}
