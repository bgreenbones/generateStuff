/*
  ==============================================================================

    Bass.h
    Created: 28 May 2023 3:29:05pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Voice.h"

class Bass : public Voice {
public:
    Bass(Ensemble& ensemble): Voice(bassKey,  bassChannel,  false, ensemble) {}

    Phrase newPhrase() const override;
    Phrase phraseFrom() const override;
    Phrase variation() const override;
};
