/*
  ==============================================================================

    Duration.cpp
    Created: 29 Aug 2022 9:15:47am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Duration.h"
#include <algorithm>

using namespace std;


Duration::operator Beats() const { return Beats(this->asBeats(), this->timeSignature); }
Duration::operator Bars() const { return Bars(this->asBars(), this->timeSignature); }
Duration::operator Quarters() const { return Quarters(this->asQuarters(), this->timeSignature); }
