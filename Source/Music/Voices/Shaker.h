/*
  ==============================================================================

    Shaker.h
    Created: 28 May 2023 3:30:10pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Voice.h"

class Shaker : public Voice {
public:
    Shaker(Ensemble& ensemble): Voice(subdivisionsKey,  subdivisionsChannel,  false, ensemble) {}

    Phrase newPhrase() const override;
    Phrase phraseFrom() const override;
    Phrase variation() const override;
};