/*
  ==============================================================================

    Cascara.h
    Created: 28 May 2023 3:28:40pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Voice.h"

class Cascara : public Voice {
public:
    Cascara(Ensemble& ensemble): Voice(cascaraKey,  cascaraChannel,  false, ensemble) {}

    Phrase newPhrase() override;
    Phrase phraseFrom() override;
    Phrase variation() override;
};