/*
  ==============================================================================

    Harmony.h
    Created: 7 Mar 2023 8:34:01pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include "Phrase.hpp"
#include "GenerateStuffEditorState.h"
#include "VoiceParameters.h"


namespace harmony {
    
    vector<Timed<ChordScale>> timedChordScales(vector<Time> times, HarmonyApproach approach);
    ChordScale randomChordScale();
    ChordScale newChordSameScale(ChordScale previousChordScale,
                                  vector<vector<Interval>> limitChordQualities = {});
    ChordScale subtleModulations(ChordScale previousChordScale);
    
    vector<Pitch> randomChord();
        
    ChordScale selectApproachAndGenerate(juce::String approach, vector<Timed<ChordScale>> chordScales);

    Phrase generateChordScales(Phrase fromPhrase,
                            HarmonyApproach approach,
                            Probability chordProbabilityPerAccent,
                            double harmonicDensity);


    Phrase chordSteps(Phrase chords);
    Phrase randomVoicings(Phrase phrase);
    Phrase smoothVoicings(Phrase harmony, Phrase rhythm, Probability randomVoicingProb = 0.1, int maximumCrunch = 4);
    // vector<ChordScale> onePerShortForLong(HarmonyApproach approach, Duration longDuration, Duration shortDuration);
}

