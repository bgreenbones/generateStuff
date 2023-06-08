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
    void applyPitchSelector(vector<Note*> notes,
                    Sequence<ChordScale>& harmonies,
                    function<PitchClass(ChordScale)> pitchClassSelector,
                    function<int()> octaveSelector,
                    Position cursor);
    // void arpeggiator(vector<Note>& notes,
    //                 Tonality tonality);
    // void arpeggiator(vector<Note>& notes,
    //                 Sequence<ChordScale>& harmonies,
    //                 Position cursor = 0);
    // void arpeggiator(vector<Note*> notes,
    //                 Sequence<ChordScale>& harmonies,
    //                 Position cursor = 0);
    // Pitch stepwiseMotion(vector<Note>& notes,
    //                     Sequence<ChordScale>& harmonies,
    //                     Pitch lastPitch,
    //                     Position cursor = 0);
    // void stepwiseMotion(vector<Note>& notes,
    //                 Sequence<ChordScale>& scales,
    //                 Pitch rangeMinimum = Pitch(45), Pitch rangeMaximum = Pitch(85)
    //                 );
    void stepwiseMotion(vector<Note*> notes,
                    Sequence<ChordScale>& scales,
                    Pitch rangeMinimum = Pitch(45), Pitch rangeMaximum = Pitch(85)
                    );
    // void stepwiseMotion(vector<Note>::iterator noteBegin, vector<Note>::iterator noteEnd,
    //                 Sequence<ChordScale>& scales,
    //                 Pitch rangeMinimum = Pitch(45), Pitch rangeMaximum = Pitch(85)
    //                 );
    Phrase bass(Phrase harmony, Phrase rhythm, int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices = {});
    Phrase melody(Phrase harmony);
}


