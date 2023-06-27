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
    struct HarmonicArguments {
      HarmonyApproach approach;
      Time upcomingTime;
      Probability modulationProbability;
      vector<Position> modulationPoints;
      ChordScale previousChordScale;
      vector<Timed<ChordScale>> chordScales;
      vector<vector<Interval>> chordQualities;
      Probability chordProbabilityPerAccent;
      double harmonicDensity;
    };

    // vector<Timed<ChordScale>> timedChordScales(vector<Time> times, HarmonicArguments args);
    ChordScale randomChordScale(HarmonicArguments args = {});
    ChordScale newChordSameScale(HarmonicArguments args);
    ChordScale subtleModulations(HarmonicArguments args);
    vector<Pitch> randomChord();
    ChordScale selectApproachAndGenerate(HarmonicArguments &args);

    Phrase generateChordScales(Phrase fromPhrase, HarmonicArguments args);


    static Pitch highPitch = Tonality::chordsHigh;
    static Pitch lowPitch = Tonality::chordsLow;
    static PitchRange pitchRange =  { lowPitch, highPitch };

    Phrase chordSteps(Phrase chords);
    Phrase randomVoicings(Phrase phrase);
    Phrase smoothVoicings(Phrase harmony, Phrase rhythm,
                          Probability randomVoicingProb = 0.1, int maximumCrunch = 4,
                          PitchRange range = { lowPitch, highPitch });
    // vector<ChordScale> onePerShortForLong(HarmonyApproach approach, Duration longDuration, Duration shortDuration);
}

