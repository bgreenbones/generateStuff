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
    vector<ChordScale> timedChordScales(vector<Timed> times, HarmonyApproach approach);
    ChordScale randomChordScale(Position startTime = 0, Duration duration = Bars(1));
    ChordScale newChordSameScale(ChordScale previousChordScale, 
                                  Position startTime, 
                                  Duration duration,
                                  vector<vector<Interval>> limitChordQualities = {});
    ChordScale subtleModulations(ChordScale previousChordScale, Position startTime, Duration duration);
    
    vector<Pitch> randomChord();
        
    ChordScale selectApproachAndGenerate(juce::String approach, vector<ChordScale> chordScales, Position startTime, Duration chordLength);

    Phrase generateChordScales(Phrase fromPhrase,
                            HarmonyApproach approach,
                            Probability chordProbabilityPerAccent,
                            double harmonicDensity);


    Phrase randomVoicings(Phrase phrase);
    Phrase smoothVoicings(Phrase harmony, Phrase rhythm);
    vector<ChordScale> onePerShortForLong(HarmonyApproach approach, Duration longDuration, Duration shortDuration);
}

