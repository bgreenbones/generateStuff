/*
  ==============================================================================

    Harmony.cpp
    Created: 8 Mar 2023 10:50:55pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Harmony.h"
#include "Rhythm.h"
#include "Utility.h"



ChordScale harmony::selectApproachAndGenerate(juce::String approach, vector<ChordScale> chordScales, Position startTime, Duration chordLength) {
    if (approach == randomHarmonyApproachKey || chordScales.empty()) {
        return randomChordScale(startTime, chordLength);
    }
    if (approach == diatonicHarmonyApproachKey) {
        return newChordSameScale(chordScales.back(), startTime, chordLength);
    }
    if (approach == smoothishModulationsHarmonyApprachKey) {
        return subtleModulations(chordScales.back(), startTime, chordLength);
    }
    return randomChordScale(startTime, chordLength);
};


ChordScale harmony::randomChordScale(Position startTime, Duration duration) {
    PitchClass root = draw<PitchClass>(pitches);
    vector<Interval> scale = draw<vector<Interval>>(diatonicModes);
    ChordScale harm(root, scale, startTime, duration);
    return harm;
}

ChordScale harmony::newChordSameScale(ChordScale previousChordScale, 
                                        Position startTime, 
                                        Duration duration, 
                                        vector<vector<Interval>> limitChordQualities) {
    Tonality scale = previousChordScale.scale;
    ChordScale nextChordScale(scale, startTime, duration);
    
    if (scale.intervalsUpFromRoot.size() > 1) {
      vector<Tonality> limitingCandidates;
      vector<Tonality> nonlimitingCandidates;
      vector<PitchClass> candidateRoots = scale.getPitchClasses();

      for (PitchClass newChordRoot : candidateRoots) {
        for (vector<Interval> quality : limitChordQualities) {
            Tonality newChord = Tonality(newChordRoot, quality);
            if (scale.includes(newChord)) {
              limitingCandidates.push_back(newChord);
            }
        }
        
        Pitch rootPitch = Pitch(newChordRoot, 0);
        Pitch thirdish = scale.multiStep(rootPitch, 2);
        Pitch fifthish = scale.multiStep(rootPitch, 4);
        
        Interval thirdInterval = thirdish - rootPitch;
        Interval fifthInterval = fifthish - rootPitch;

        vector<Interval> triad = { unison, thirdInterval, fifthInterval };
        nonlimitingCandidates.push_back(Tonality(newChordRoot, triad));
      }

      if (limitingCandidates.empty()) {
        nextChordScale.harmony = draw<Tonality>(nonlimitingCandidates);
      } else {
        nextChordScale.harmony = draw<Tonality>(limitingCandidates);
      }

      
    }

    return nextChordScale;
}

ChordScale harmony::subtleModulations(ChordScale previousChordScale, Position startTime, Duration duration) {
    ChordScale newChordScale(previousChordScale.scale.smoothModulation(1, draw<Direction>({ up, down })), 
        previousChordScale.harmony, 
        startTime, 
        duration);
    return newChordSameScale(newChordScale, startTime, duration, { major9chord, minor9chord });
}


vector<ChordScale> harmony::timedChordScales(vector<Timed> times, HarmonyApproach approach) {
    vector<ChordScale> chords;
    for(Timed time : times) {
        ChordScale chordScale = selectApproachAndGenerate(harmonyApproaches[(int)approach].toStdString(),
            chords,
            time.startTime,
            time.duration);
        chords.push_back(chordScale);
    }
    return chords;
}

Phrase harmony::generateChordScales(Phrase fromPhrase, HarmonyApproach approach, Probability chordProbabilityPerAccent, double harmonicDensity) {
// Phrase harmony::generateChordScales(Phrase fromPhrase, string harmonyApproach, Probability chordProbabilityPerAccent, double harmonicDensity) {
    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.assignEvents(filter<Note>(notes, [](Note note) { return note.accented; }));
    accents.legato();
    

    if (accents.empty()) {
        vector<Timed> times = rhythm::onePerShortForLong(Bars(1), fromPhrase.getDuration());
        vector<ChordScale> harmonies = harmony::timedChordScales(times, approach);
        fromPhrase.chordScales.assignEvents(harmonies);
    } else {
        for (Note accent : accents) {
            if (chordProbabilityPerAccent) {
                bool firstChord = fromPhrase.chordScales.empty();
                double previousChordLength = firstChord ? 1. / harmonicDensity : (accent.startTime - fromPhrase.chordScales.back().startTime).asSeconds();
                if (Probability(harmonicDensity * previousChordLength)) {
                  ChordScale chordScale = selectApproachAndGenerate(harmonyApproaches[(int)approach].toStdString(), fromPhrase.chordScales, accent.startTime, accent.duration);
                  fromPhrase.chordScales.add(chordScale);
                }
            }
        }
    }
    fromPhrase.chordScales.tie();
    return fromPhrase;
}


Phrase harmony::randomVoicings(Phrase phrase) {
    phrase = phrase.toPolyphonic();
    phrase.notes.clear();
    
    for (ChordScale chordScale : phrase.chordScales) {
        for (Pitch pitchToAdd : chordScale.harmony.randomVoicing()) {
            Note noteToAdd(pitchToAdd.pitchValue, 70, chordScale.startTime, chordScale.duration);
            phrase.addNote(noteToAdd);
        }
    }
    
    return phrase;
};

Phrase harmony::smoothVoicings(Phrase harmony, Phrase rhythm) {
    harmony = harmony.toPolyphonic();
    harmony.notes.clear();
    rhythm = rhythm.toMonophonic();
    
    if (harmony.chordScales.empty()) {
      return harmony;
    }
    vector<Pitch> lastVoicing = harmony.chordScales[0].harmony.randomVoicing();
    vector<ChordScale> voiced;
    for (Note note : rhythm.notes) {
        ChordScale chordScale = harmony.chordScales.drawByPosition(note.startTime);
        if (contains(voiced, chordScale)) {
          continue;
        }
        voiced.push_back(chordScale);
        vector<Pitch> voicing = flipWeightedCoin(0.9) // can't be too smooth!
            ? chordScale.harmony.smoothVoicing(lastVoicing)
            : chordScale.harmony.randomVoicing();
        while (voicing::crunch(voicing) >= 4) { // control clusters
            voicing::decreaseCrunch(voicing);
        }
        voicing::decreaseSpread(voicing);
        voicing::preventRumble(voicing);
        for (Pitch pitchToAdd : voicing) {
            Note noteToAdd(pitchToAdd.pitchValue, 70, note.startTime, chordScale.endTime() - note.startTime);
            harmony.addNote(noteToAdd);
        }
        lastVoicing = voicing;
    }
    
    return harmony;
};



vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
