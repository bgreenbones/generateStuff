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

    Phrase newPhrase() override;
    Phrase phraseFrom() override;
    Phrase variation() override;
};