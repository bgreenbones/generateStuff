/*
  ==============================================================================

    Clave.h
    Created: 28 May 2023 3:28:21pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once
#include "Voice.h"

class Clave : public Voice {
public:
    Clave(Ensemble& ensemble): Voice(claveKey,  claveChannel,  false, ensemble) {}

    Phrase newPhrase() override;
    Phrase phraseFrom() override;
    Phrase variation() override;
};