/*
  ==============================================================================

    Duration.cpp
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Duration.h"
#include <algorithm>

Duration::operator Beats() const { return Beats(this->asBeats(), this->getTimeSignature()); }
Duration::operator Bars() const { return Bars(this->asBars(), this->getTimeSignature()); }
Duration::operator Quarters() const { return Quarters(this->asQuarters(), this->getTimeSignature()); }
