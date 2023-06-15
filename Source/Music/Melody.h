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
    static Pitch highPitch = Pitch(85);
    static Pitch lowPitch = Pitch(45);
    void stepwiseMotion(vector<Note*> notes,
                    Sequence<ChordScale>& scales,
                    Pitch rangeMinimum = lowPitch, Pitch rangeMaximum = highPitch);
    void stepwiseMotion(vector<Note*> notes,
                    Pitch rangeMinimum = lowPitch, Pitch rangeMaximum = highPitch);
    void decreaseSpread(vector<Note> & melody, Interval maxJump);
    // void stepwiseMotion(vector<Note>::iterator noteBegin, vector<Note>::iterator noteEnd,
    //                 Sequence<ChordScale>& scales,
    //                 Pitch rangeMinimum = Pitch(45), Pitch rangeMaximum = Pitch(85)
    //                 );
    Phrase bass(Phrase harmony, Phrase rhythm, int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices = {});
    Phrase streamOfConsciousness(Phrase harmony);
    Phrase streamWithThemes(Phrase harmony);
    vector<Note> shape(Duration shapeLength, Duration subdivision);
    Phrase repeatingShape(Phrase harmony, Duration shapeLength);
}


