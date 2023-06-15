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



Phrase harmony::voicingFills(Phrase unfilledVoicings, vector<Phrase> competingVoices) {
  unfilledVoicings.subdivisions.tie(true);

  
  Phrase filledVoicings(unfilledVoicings);
  vector<Timed> spaces = rhythm::gaps(filledVoicings, competingVoices);
  // Duration totalSpaceToFill = 0;

  // if (totalSpaceToFill / unfilledVoicings.getDuration() < 1./6.
  //       || spaces.empty()) {
  if (spaces.empty()) {
    // in this case we might just add rhythms on top of other active voices 
    // instead of trying to fill in space
    return unfilledVoicings;
  }

  sort(spaces.begin(), spaces.end(), 
        [](Timed const &a, Timed const &b) { return a.duration > b.duration; }); // longest to shortest


  for (int i = 0; i < spaces.size() / 2 + 1; i++) {
    if (flipWeightedCoin(0.3)) {// TODO: let's do better random
      continue;
    }
    Timed spaceToFill = spaces[i];

    
    // vector<Timed> times = rhythm::stabilityBased(spaceToFill, unfilledVoicings.subdivisions, 0, 0.7);
    Subdivision subdivision = unfilledVoicings.subdivisions.drawByPosition(spaceToFill.startTime);
    int subdivisionsInSpace = spaceToFill.duration / subdivision;
    int lengthInSubdivisions = rollDie(subdivisionsInSpace);
    double displacementInSubdivisions = uniformInt(0, subdivisionsInSpace - lengthInSubdivisions);
    Duration displacement = displacementInSubdivisions * subdivision;
    vector<Timed> times = rhythm::nOfLengthM(lengthInSubdivisions, subdivision);
    vector<vector<Timed*>> toDoubleOrHalf = rhythm::distinctSubsets<Timed>(times, 0.5, {0.3, 0.1});
    vector<Timed*> toDouble = toDoubleOrHalf[0];
    vector<Timed*> toHalf = toDoubleOrHalf[1];
    // // modify times
    rhythm::multiplyTimeLength(times, toDouble, 2);
    rhythm::repeat(times, toHalf, 0.5);
    // convert to notes
    // vector<Note> notes = Sequence<Note>::fromTimed(times);
    vector<Note*> voicing;
    for (Timed time : times) {
        Position realStartTime = spaceToFill.startTime + displacement + time.startTime;
        Position realEndTime = spaceToFill.startTime + displacement + time.endTime();
        
        // vector<Note*> possibleVoicing = unfilledVoicings.notes.pointersByPosition(realStartTime);
        vector<Note*> possibleVoicing = filledVoicings.notes.pointersByPosition(realStartTime);
        if (possibleVoicing.size() > 0) {
          voicing = possibleVoicing;
//          if (voicing.size() < 3) {
//            int i = 1;
//          }
          // get out of the way
          // todo: only do this once for the new rhythm instead of for every note.
          // for (Note* note : possibleVoicing) { // TODO: this should probably abstracted out and refined
          //     // TODO: this should maybe happen with a call to sequence.insert?
          //   if (note->startTime < realStartTime) {
          //       note->setEndTime(realStartTime);
          //   }
          //   if (note->startTime >= realStartTime ) {
          //     note->setStartTime(realEndTime);
          //   }
          //   // todo: what if the voicing is both before AND after time? as in, it's long?
          // }
        }
//        vector<Pitch> pitches;
        for (Note* note : voicing) {
//            if (contains(pitches, note->pitch)) {
//              int i = 1;
//            }
//            pitches.push_back(note->pitch);
            Note toAdd = Note(*note);
            toAdd.startTime = realStartTime;
            toAdd.duration = time.duration;
            filledVoicings.notes.add(toAdd, PushBehavior::ignore, OverwriteBehavior::cutoff);
        }
    }
  }

  return filledVoicings;
}

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
    fromPhrase.chordScales.tie(true);
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
            // int velocity = 70;
            int velocity = 127 - pitchToAdd.pitchValue; // todo: this is cool but maybe we should be aware and control it somehwere
            Note noteToAdd(pitchToAdd.pitchValue, velocity, note.startTime, chordScale.endTime() - note.startTime);
            harmony.addNote(noteToAdd);
        }
        lastVoicing = voicing;
    }
    
    return harmony;
};



vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
