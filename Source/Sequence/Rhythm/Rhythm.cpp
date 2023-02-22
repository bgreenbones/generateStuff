/*
  ==============================================================================

    Rhythm.cpp
    Created: 15 Sep 2022 9:24:27pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"
#include "Random.h"



Phrase Phrase::flip() const {
    Phrase resultPhrase(*this);
    resultPhrase.notes.flip();
    resultPhrase.subdivisions.flip();
    return resultPhrase;
}

Phrase Phrase::pulseAndDisplace(Duration pulse,
                                  Duration displacement, // 1/3 to displace by a triplet, 1/4 for a sixteenth, etc.
                                  Probability pDisplace,
                                  Probability pDouble) const
{
    const Duration length(duration);
    Phrase resultPhrase(*this);
    resultPhrase.notes = resultPhrase.notes.pulseAndDisplace(pulse, displacement, pDisplace, pDouble, length);
    
    return resultPhrase;
}

