/*
  ==============================================================================

    Melody.h
    Created: 7 Mar 2023 8:34:19pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Phrase.hpp"
#include "GenerateStuffEditorState.h"
#include "Harmony.h"
#include "Rhythm.h"

namespace melody {
    void arpeggiator(vector<Note>& notes,
                    Tonality tonality);
    Pitch stepwiseMotion(vector<Note>& notes,
                        Sequence<ChordScale>& harmonies,
                        Pitch lastPitch,
                        Position cursor);
    Phrase bass(Phrase harmony, Phrase rhythm, int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices = {});
    Phrase melody(Phrase harmony);
}


