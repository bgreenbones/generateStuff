/*
  ==============================================================================

    Duration.cpp
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Duration.h"
#include <algorithm>

Duration::operator Beats() const { return Beats(asBeats(), getTimeSignature(), this->dynamicTimeSignature); }
Duration::operator Bars() const { return Bars(asBars(), getTimeSignature(), this->dynamicTimeSignature); }
//Duration::operator Quarters() const { return Quarters(this->asQuarters(), this->getTimeSignature()); }


Duration& Duration::operator=(Beats const& other) {
    if (this == (Duration*)&other) { return *this; }
    this->assignValues(other);
    // The returned duration functions as a Beats
    this->behavior = DurationBehavior::MaintainBeats;
    return *this;
}

Duration& Duration::operator=(Bars const& other) {
    if (this == (Duration*)&other) { return *this; }
    this->assignValues(other);
    // The returned duration functions as a Bars
    this->behavior = DurationBehavior::MaintainBars;
    return *this;
}


Beats& Beats::operator=(Duration const& other) {
    if (this == (Beats*)&other) { return *this; }
    this->assignValues(other);
    return *this;
}

Bars& Bars::operator=(Duration const& other) {
    if (this == (Bars*)&other) { return *this; }
    this->assignValues(other);
    return *this;
}

Beats& Beats::operator=(Bars const& other) {
    if (this == (Beats*)&other) { return *this; }
    this->assignValues(other);
    return *this;
}

Bars& Bars::operator=(Beats const& other) {
    if (this == (Bars*)&other) { return *this; }
    this->assignValues(other);
    return *this;
}
