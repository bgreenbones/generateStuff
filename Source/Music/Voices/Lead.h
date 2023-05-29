/*
  ==============================================================================

    Lead.h
    Created: 28 May 2023 5:59:04pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Voice.h"

class Lead : public Voice {
public:
    Lead(Ensemble& ensemble): Voice(melodyKey,  melodyChannel,  false, ensemble) {}

    Phrase newPhrase() override;
    Phrase phraseFrom() override;
    Phrase variation() override;
};