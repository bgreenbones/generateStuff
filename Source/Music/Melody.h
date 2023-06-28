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
    void applyPitchSelector(vector<Timed<Note>*> notes,
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
    static PitchRange pitchRange = { lowPitch, highPitch };
    
    // static Pitch bassHighPitch = Pitch(D, 5);
    // static Pitch bassLowPitch = Pitch(A, 2);
    static Pitch bassHighPitch = Pitch(F, 5);
    static Pitch bassLowPitch = Pitch(F, 2);
    static PitchRange bassRange = { bassLowPitch, bassHighPitch };

    double slope(vector<Timed<Note>> shape);
    void stepwiseMotion(vector<Timed<Note>*> notes,
                    Sequence<ChordScale>& scales,
                    PitchRange range = pitchRange);
    void stepwiseMotion(vector<Timed<Note>*> notes, PitchRange range = pitchRange);
    void decreaseSpread(vector<Timed<Note>> & melody, Interval maxJump);
    // void stepwiseMotion(vector<Note>::iterator noteBegin, vector<Note>::iterator noteEnd,
    //                 Sequence<ChordScale>& scales,
    //                 Pitch rangeMinimum = Pitch(45), Pitch rangeMaximum = Pitch(85)
    //                 );
    Phrase bass(Phrase harmony, Phrase rhythm,
                // int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices = {},
                PitchRange range = bassRange, float density = 0.5);
    Phrase streamOfConsciousness(Phrase harmony);
    Phrase streamWithThemes(Phrase harmony, PitchRange = pitchRange, float density = 0.5);
    vector<Timed<Note>> shape(Duration shapeLength, Duration subdivision, PitchRange range = pitchRange, float density = 0.5);
    Phrase repeatingShape(Phrase harmony, Duration shapeLength);
}


