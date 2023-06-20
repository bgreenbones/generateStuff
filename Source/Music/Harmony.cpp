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
  for (Timed<Note> &note : chords.notes) {
    if (currentVertical.empty() || note.startTime == currentVertical.back()->startTime) {
      currentVertical.push_back(&note);
    } else {
      previousVertical = currentVertical;
        d = flipCoin() ? Direction::up : Direction::down;
      currentVertical.clear();
      currentVertical.push_back(&note);
    }

    if (!previousVertical.empty()) {
      Timed<ChordScale> previousHarmony = chords.chordScales.drawByPosition(previousVertical.back()->startTime);
      Timed<ChordScale> currentHarmony = chords.chordScales.drawByPosition(note.startTime);
      if (previousHarmony.item.harmony == currentHarmony.item.harmony) {
        note.item.pitch = previousVertical[currentVertical.size() - 1]->item.pitch;
        // for (Timed<Note> *previousNote : previousVertical) {
          // if (previousNote->item.pitch == note.item.pitch) {
            // note.item.pitch = currentHarmony.item.harmony.step(previousNote->item.pitch, Direction::down);
            note.item.pitch = currentHarmony.item.harmony.step(note.item.pitch, d);
            // break;
          // }
        // }
      }
    }
  }
  return chords;
}


ChordScale harmony::selectApproachAndGenerate(juce::String approach, vector<Timed<ChordScale>> chordScales) {
    if (approach == randomHarmonyApproachKey || chordScales.empty()) {
        return randomChordScale();
    }
    if (approach == diatonicHarmonyApproachKey) {
        return newChordSameScale(chordScales.back());
    }
    if (approach == smoothishModulationsHarmonyApprachKey) {
        return subtleModulations(chordScales.back());
    }
    return randomChordScale();
};


ChordScale harmony::randomChordScale() {
    PitchClass root = draw<PitchClass>(pitches);
    vector<Interval> scale = draw<vector<Interval>>(diatonicModes);
    ChordScale harm(Tonality(root, scale));
    return harm;
}

ChordScale harmony::newChordSameScale(ChordScale previousChordScale,
                                        vector<vector<Interval>> limitChordQualities) {
    Tonality scale = previousChordScale.scale;
    ChordScale nextChordScale(scale);
    
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

ChordScale harmony::subtleModulations(ChordScale previousChordScale) {
    ChordScale newChordScale(previousChordScale.scale.smoothModulation(1, draw<Direction>({ up, down })), 
        previousChordScale.harmony);
    return newChordSameScale(newChordScale, { major9chord, minor9chord });
}


vector<Timed<ChordScale>> harmony::timedChordScales(vector<Time> times, HarmonyApproach approach) {
    vector<Timed<ChordScale>> chords;
    for(Time time : times) {
        ChordScale chordScale = selectApproachAndGenerate(harmonyApproaches[(int)approach].toStdString(),
                                                          chords);
        chords.push_back(Timed<ChordScale>(time, chordScale));
    }
    return chords;
}

Phrase harmony::generateChordScales(Phrase fromPhrase, HarmonyApproach approach, Probability chordProbabilityPerAccent, double harmonicDensity) {
// Phrase harmony::generateChordScales(Phrase fromPhrase, string harmonyApproach, Probability chordProbabilityPerAccent, double harmonicDensity) {
    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.assignEvents(filter<Timed<Note>>(notes, [](Timed<Note> note) { return note.item.accented; }));
    accents.legato();
    

    if (accents.empty()) {
        vector<Time> times = rhythm::onePerShortForLong(Bars(1), fromPhrase.getDuration());
        vector<Timed<ChordScale>> harmonies = harmony::timedChordScales(times, approach);
        fromPhrase.chordScales.assignEvents(harmonies);
    } else {
        for (Timed<Note> accent : accents) {
            if (chordProbabilityPerAccent) {
                bool firstChord = fromPhrase.chordScales.empty();
                double previousChordLength = firstChord ? 1. / harmonicDensity : (accent.startTime - fromPhrase.chordScales.back().startTime).asSeconds();
                if (Probability(harmonicDensity * previousChordLength)) {
                  ChordScale chordScale = selectApproachAndGenerate(harmonyApproaches[(int)approach].toStdString(), fromPhrase.chordScales);
                  fromPhrase.chordScales.add(Timed<ChordScale>(accent, chordScale));
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
    
    for (Timed<ChordScale> chordScale : phrase.chordScales) {
        for (Pitch pitchToAdd : chordScale.item.harmony.randomVoicing()) {
            Timed<Note> noteToAdd(chordScale, Note(pitchToAdd, 70));
            phrase.addNote(noteToAdd);
        }
    }
    
    return phrase;
};

Phrase harmony::smoothVoicings(Phrase harmony, Phrase rhythm, Probability randomVoicingProb, int maximumCrunch) {
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
            ? chordScale.item.harmony.smoothVoicing(lastVoicing)
            : chordScale.item.harmony.randomVoicing();
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
