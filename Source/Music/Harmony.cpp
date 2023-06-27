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


Phrase harmony::chordSteps(Phrase chords) {
  vector<Timed<Note>*> currentVertical;
  vector<Timed<Note>*> previousVertical;
  Direction d = flipCoin() ? Direction::up : Direction::down;
  bool fastStep = Probability(0.3);
  sort(chords.notes);
  for (Timed<Note> &note : chords.notes) {
    if (currentVertical.empty() || note.startTime == currentVertical.back()->startTime) {
      currentVertical.push_back(&note);
    } else {
      previousVertical = currentVertical;
      sort(previousVertical);
      vector<double> gravitys = mapp<Timed<Note>*, double>(previousVertical, [](Timed<Note>* note) {
        return note->item.pitch.gravity();
      });
      double totalGravity = accumulate(gravitys.begin(), gravitys.end(), 0.);
      double averageGravity = totalGravity / max(1, (int)gravitys.size());
      d = Probability(0.5 + averageGravity) ? Direction::down : Direction::up;
      fastStep = Probability(0.3);
      currentVertical.clear();
      currentVertical.push_back(&note);
    }

    if (!previousVertical.empty()) {
      Timed<ChordScale> previousHarmony = chords.chordScales.drawByPosition(previousVertical.back()->startTime);
      Timed<ChordScale> currentHarmony = chords.chordScales.drawByPosition(note.startTime);
      if (previousHarmony.item.harmony == currentHarmony.item.harmony) {
        Timed<Note>* previousNote = previousVertical.back();
        if (currentVertical.size() <= previousVertical.size()) {
          previousNote = previousVertical[currentVertical.size() - 1];
          note.item = previousNote->item;
        } else {
          DBG("why does this happen");
        }
        if ((note.duration > thirtySeconds && previousNote->duration > thirtySeconds) || fastStep) {
          note.item.pitch = currentHarmony.item.harmony.step(note.item.pitch, d);
        }
      }
    }
  }
  return chords;
}


ChordScale harmony::selectApproachAndGenerate(HarmonicArguments &args) {
    if (args.modulationPoints.empty()) {
        args.modulationPoints.push_back(args.upcomingTime.startTime);
    } else {
      Duration timeSinceLastModulation = args.upcomingTime.startTime - args.modulationPoints.back();
      args.modulationProbability = timeSinceLastModulation / Bars(2);
    }

    ChordScale newChordScale;
    if (args.chordScales.empty()) {
      newChordScale = randomChordScale();
    }
    switch (args.approach) {
      case HarmonyApproach::random:
        newChordScale = randomChordScale();
        break;
      case HarmonyApproach::diatonic:
        newChordScale = newChordSameScale(args);
        break;
      case HarmonyApproach::smoothishModulations:
        newChordScale = subtleModulations(args);
        break;
      case HarmonyApproach::harmonyApproachCount:
          break;
    }
    
    if (!args.chordScales.empty()) {
      if (args.previousChordScale.scale != newChordScale.scale) {
        args.modulationPoints.push_back(args.upcomingTime.startTime);
      }
    }
    args.chordScales.push_back(Timed<ChordScale>(args.upcomingTime, newChordScale));

    return newChordScale;
};


ChordScale harmony::randomChordScale(HarmonicArguments args) {
    PitchClass root = draw<PitchClass>(pitches);
    vector<Interval> scale = draw<vector<Interval>>(diatonicModes);
    ChordScale harm(Tonality(root, scale));
    return harm;
}

ChordScale harmony::newChordSameScale(HarmonicArguments args) {
    Tonality scale = args.previousChordScale.scale;
    ChordScale nextChordScale(scale);
    
    if (scale.intervalsUpFromRoot.size() > 1) {
      vector<Tonality> limitingCandidates;
      vector<Tonality> nonlimitingCandidates;
      vector<PitchClass> candidateRoots = scale.getPitchClasses();

      for (PitchClass newChordRoot : candidateRoots) {
        for (vector<Interval> quality : args.chordQualities) {
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

ChordScale harmony::subtleModulations(HarmonicArguments args) {
  // vector<vector<Interval>> chordQualities = { majorAdd2};
  // vector<vector<Interval>> chordQualities = augmentedAndDiminished;
  // vector<vector<Interval>> chordQualities = majorMinorOneColorTone;


  if (args.modulationProbability) {
    args.previousChordScale.scale = args.previousChordScale.scale
      .smoothModulation(1,
                        draw<Direction>({ up, down }),
                        args.chordQualities);
  }
  return newChordSameScale(args);

}



Phrase harmony::generateChordScales(Phrase fromPhrase,
                                    HarmonicArguments args) {
    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.assignEvents(filter<Timed<Note>>(notes, [](Timed<Note> note) { return note.item.accented; }));
    accents.legato();

    if (accents.empty()) {
        vector<Time> times = rhythm::onePerShortForLong(Bars(1), fromPhrase.getDuration());
        for(Time time : times) {
          args.upcomingTime = time;
          ChordScale chordScale = selectApproachAndGenerate(args);
        }
    } else {
        for (Timed<Note> accent : accents) {
            if (args.chordProbabilityPerAccent) {
                bool firstChord = args.chordScales.empty();
                double previousChordLength = firstChord 
                  ? 1. / args.harmonicDensity
                  : (accent.startTime - args.chordScales.back().startTime).asSeconds();
                if (Probability(args.harmonicDensity * previousChordLength)) {
                  args.upcomingTime = accent;
                  ChordScale chordScale = selectApproachAndGenerate(args);
                }
            }
        }
    }
    fromPhrase.chordScales.assignEvents(args.chordScales);
    fromPhrase.chordScales.tie(true);
    return fromPhrase;
}


Phrase harmony::randomVoicings(Phrase phrase) {
    phrase = phrase.toPolyphonic();
    phrase.notes.clear();
    
    for (Timed<ChordScale> chordScale : phrase.chordScales) {
        for (Pitch pitchToAdd : chordScale.item.harmony.randomVoicing()) {
            Timed<Note> noteToAdd(chordScale, Note(pitchToAdd, 70));
            phrase.addNote(noteToAdd);
        }
    }
    
    return phrase;
};

Phrase harmony::smoothVoicings(Phrase harmony, Phrase rhythm, Probability randomVoicingProb, int maximumCrunch,
                                PitchRange range) {
    harmony = harmony.toPolyphonic();
    harmony.notes.clear();
    rhythm = rhythm.toMonophonic();

    if (harmony.chordScales.empty()) {
      return harmony;
    }
    // set<Timed> harmonicKeyPoints;
    // set<Timed> rhythmicKeyPoints;
    set<Position> keyPoints;
    vector<Position> sortedKeyPoints;
    
    for (Timed<Note> note : rhythm.notes) {
      // rhythmicKeyPoints.emplace(note);
      keyPoints.emplace(note.startTime); }
    for (Timed<ChordScale> tonality : harmony.chordScales) {
      // harmonicKeyPoints.emplace(tonality);
      keyPoints.emplace(tonality.startTime); }
    sortedKeyPoints.insert(sortedKeyPoints.begin(), keyPoints.begin(), keyPoints.end());
    sort(sortedKeyPoints);
    
    vector<Pitch> lastVoicing = harmony.chordScales[0].item.harmony.randomVoicing();
    vector<Timed<ChordScale>> voiced;
    for (Position keyPoint : sortedKeyPoints) {
        Timed<ChordScale> chordScale = harmony.chordScales.drawByPosition(keyPoint);
        if (contains(voiced, chordScale)) {
          continue;
        }
        voiced.push_back(chordScale);
        vector<Pitch> voicing = flipWeightedCoin(1 - randomVoicingProb) // can't be too smooth!
            ? chordScale.item.harmony.smoothVoicing(lastVoicing, range)
            : chordScale.item.harmony.randomVoicing(range);

            // TODO: TAKE range into account in the next 3 calls.
        while (voicing::crunch(voicing) >= maximumCrunch) { // control clusters
            voicing::decreaseCrunch(voicing);
        }
        voicing::decreaseSpread(voicing);
        voicing::preventRumble(voicing);
        for (Pitch pitchToAdd : voicing) {
            // int velocity = 70;
            int velocity = 127 - pitchToAdd.pitchValue; // todo: this is cool but maybe we should be aware and control it somehwere
            Note noteToAdd(pitchToAdd.pitchValue, velocity);
            Time time(keyPoint, chordScale.endTime() - keyPoint);
            harmony.notes.add(Timed<Note>(time, noteToAdd), PushBehavior::ignorePush, OverwriteBehavior::insert);
        }
        lastVoicing = voicing;
    }
    
    return harmony;
};



vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
