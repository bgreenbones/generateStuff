/*
  ==============================================================================

    Chords.h
    Created: 28 May 2023 5:58:21pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Voice.h"

class Chords : public Voice {
public:
    Chords(Ensemble& ensemble): Voice(harmonyKey,  harmonyChannel,  false, ensemble) {}

    Phrase newPhrase() override;
    Phrase phraseFrom() override;
    Phrase variation() override;
};