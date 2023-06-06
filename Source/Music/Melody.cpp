/*
  ==============================================================================

    Melody.cpp
    Created: 8 Mar 2023 10:51:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Melody.h"
#include "Ensemble.h"
#include "Utility.h"

Pitch melody::stepwiseMotion(vector<Note>& notes,
                    Sequence<ChordScale>& harmonies,
                    Pitch lastPitch,
                    Position cursor) {
  for (Note &note : notes) {
      Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
      note.pitch = harmonies.drawByPosition(cursor + note.startTime).scale
          .step(lastPitch, direction);
      lastPitch = note.pitch;
  }
  return lastPitch;
}

void melody::stepwiseMotion(vector<Note>& notes,
                    Sequence<ChordScale>& scales,
                    Pitch rangeMinimum, Pitch rangeMaximum
                    // Interval maxJump
                    ) {
  Pitch lastPitch(uniformInt(rangeMinimum.pitchValue, rangeMaximum.pitchValue));
  for (Note &note : notes) {
      Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
      Pitch candidatePitch = scales.drawByPosition(note.startTime).scale
          .step(lastPitch, direction);
      if (candidatePitch < rangeMinimum) {
        candidatePitch = scales.drawByPosition(note.startTime).scale
            .step(lastPitch, Direction::up);
      }
      if (candidatePitch > rangeMaximum) {
        candidatePitch = scales.drawByPosition(note.startTime).scale
            .step(lastPitch, Direction::down);
      }
      note.pitch = candidatePitch;
      lastPitch = note.pitch;
  }
}

// void melody::arpeggiator(vector<Note>& notes,
//                     Sequence<ChordScale>& harmonies,
//                     Position cursor) {
//   vector<Note*> note_p;
//   for (Note& note : notes) {
//     note_p.push_back(&note);
//   }
//   return arpeggiator(note_p, harmonies, cursor);
// }
// void melody::arpeggiator(vector<Note*> notes,
//                     Sequence<ChordScale>& harmonies,
//                     Position cursor) {
//   for (Note *note : notes) {
//       note->pitch = draw<Pitch>(harmonies.drawByPosition(cursor + note->startTime).harmony.getPitches());
//   }
// }
// void melody::arpeggiator(vector<Note>& notes,
//                     Tonality tonality) {
//   for (Note &note : notes) {
//       // Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
//       note.pitch = flipWeightedCoin(0.6) 
//         ? Pitch(tonality.root, flipCoin() ? draw<int>({2, 3}) : draw<int>({3, 4}))
//         : draw<Pitch>(tonality.getPitches());
//   }
// }



PitchClass selectHarmonicRoot(ChordScale chordScale) {return chordScale.harmony.root;}
PitchClass randomChordTone(ChordScale chordScale) {return draw<PitchClass>(chordScale.harmony.getPitchClasses());}
int selectBassOctave() { return flipCoin() ? draw<int>({2, 3}) : draw<int>({3, 4}); }




Pitch bassMelody(vector<Note*> notes,
                    Sequence<ChordScale>& harmonies,
                    Position cursor = 0) {
    // for (Note *note : notes) {
    for (int i = 0; i <notes.size(); i++) {
      Note *note = notes[i];
      Note *previousNote = notes[max(i - 1,0)];
      ChordScale chordScale = harmonies.drawByPosition(cursor + note->startTime);
      PitchClass root = chordScale.harmony.root;
      PitchClass chordTone = draw<PitchClass>(chordScale.harmony.getPitchClasses());
      PitchClass bassPitchClass = draw<PitchClass>({root, chordTone}, {0.6, 0.4});
      note->pitch = Pitch::randomInRange(bassPitchClass,
                            Pitch(Gb, 2), Pitch(F, 4));
      note->pitch.makeCloserKeepPitchClass(previousNote->pitch, 0.7);
      
    }
}

void melody::applyPitchSelector(vector<Note*> notes,
                    Sequence<ChordScale>& harmonies,
                    function<PitchClass(ChordScale)> pitchClassSelector,
                    function<int()> octaveSelector,
                    Position cursor = 0) {
    for (Note *note : notes) {
      note->pitch = Pitch(pitchClassSelector(harmonies.drawByPosition(cursor + note->startTime)),
                          octaveSelector());
    }
}

Phrase melody::bass(Phrase harmony, Phrase rhythm, int minimumRepeats, int maximumRepeats, vector<float> burstNoteLengthChoices) {
  Phrase phrase(harmony);
  phrase.notes = harmony.notes.toMonophonic();
  phrase.notes.clear();
  
  set<Timed> harmonicKeyPoints;
  set<Timed> rhythmicKeyPoints;
  set<Timed> keyPoints;
  
  for (Note note : rhythm.notes) { rhythmicKeyPoints.emplace(note); keyPoints.emplace(note); }
  for (ChordScale tonality : phrase.chordScales) { harmonicKeyPoints.emplace(tonality); keyPoints.emplace(tonality); }
  
  vector<Position> rhythmicPositions;
  int i = 0;
  for (Timed keyPoint : keyPoints) {
    if (!contains<Timed>(harmonicKeyPoints, keyPoint) && flipWeightedCoin(0.4)) { continue; }
    rhythmicPositions.push_back(keyPoint.startTime);
    
    // ChordScale chordScale = phrase.chordScales.drawByPosition(keyPoint.startTime);
    // Tonality tonality = chordScale.harmony;

    vector<Timed> times = rhythm::stabilityBased(keyPoint, rhythm.subdivisions, 0.1, 0.4);
    // if (i > 6) {
    bool barIsEven = (int)floor(keyPoint.startTime.asBars()) % 2;
    if (barIsEven) {
    // if (i <= 3) {
      Duration subdiv = rhythm.subdivisions.drawByPosition(keyPoint.startTime);
      Duration noteLength = draw<float>(burstNoteLengthChoices) * subdiv;
      
      int numberOfNotes = uniformInt(minimumRepeats, maximumRepeats);
      times = rhythm::nOfLengthM(numberOfNotes, noteLength);
      // vector<Timed> times = rhythm::nOfLengthM(numberOfNotes, noteLength);

    }
    if (i < 6) {
      i++;
    } else {
      i = 0;
    }
    vector<vector<Timed*>> toDoubleOrTriple = rhythm::distinctSubsets<Timed>(times, 0.4, {0.7, 0.3});
    vector<Timed*> toDouble = toDoubleOrTriple[0];
    vector<Timed*> toTriple = toDoubleOrTriple[1];
    rhythm::multiplyTimeLength(times, toDouble, 2);
    rhythm::multiplyTimeLength(times, toTriple, 3);
    
    vector<Note> notes = Sequence<Note>::fromTimed(times);
    vector<Note*> notes_p;
    for (Note &note : notes) { notes_p.push_back(&note); }
    // vector<vector<Note*>> rootOrChordTones = rhythm::distinctSubsets<Note>(notes, 1, {0.4, 0.6});
    // vector<Note*> toArpeggiate = rootOrChordTones[0];
    // vector<Note*> toRoot = rootOrChordTones[1];
    // applyPitchSelector(toArpeggiate, phrase.chordScales, randomChordTone, selectBassOctave, keyPoint.startTime);
    // applyPitchSelector(toRoot, phrase.chordScales, selectHarmonicRoot, selectBassOctave, keyPoint.startTime);
    bassMelody(notes_p, phrase.chordScales, keyPoint.startTime);

    phrase.notes.insertVector(notes, keyPoint.startTime, PushBehavior::truncate, OverwriteBehavior::cutoff);
  }
  
  dynamics::followAccents(phrase.notes, rhythmicPositions, mf, ff);

  return phrase;
};


Phrase melody::melody(Phrase harmony) {
    Phrase phrase(harmony);
    phrase.notes = harmony.notes.toMonophonic();
    phrase.notes.clear();
    
    Position cursor = 0;
    Pitch lastPitch(uniformInt(55, 75));
    while (cursor < phrase.getDuration()) {
        
        // get underlying phrase structures.
        vector<ChordScale> chordScales = phrase.chordScales.byPosition(cursor);
        if (chordScales.empty()) {
            cursor += 1;
            continue;
        }
        ChordScale chordScale = chordScales.at(0);
        vector<Subdivision> subdivs = phrase.subdivisions.byPosition(cursor);
        Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
        
        // get stream of times
        int burstLength = 1 + rollDie(flipWeightedCoin(0.35) ? 5 : 12);
        vector<Timed> burstOfTimes = rhythm::nOfLengthM(burstLength, subdiv);
        // choose times to modify
        vector<vector<Timed*>> toDoubleOrHalf = rhythm::distinctSubsets<Timed>(burstOfTimes, 0.25, {0.3, 0.1});
        vector<Timed*> toDouble = toDoubleOrHalf[0];
        vector<Timed*> toHalf = toDoubleOrHalf[1];
        // // modify times
        rhythm::multiplyTimeLength(burstOfTimes, toDouble, 2);
        rhythm::repeat(burstOfTimes, toHalf, 0.5);
        // convert to notes
        vector<Note> burstOfNotes = Sequence<Note>::fromTimed(burstOfTimes);
        // choose notes
        // lastPitch = stepwiseMotion(burstOfNotes, phrase.chordScales, lastPitch, cursor);
        
        Dynamics d = {
          .range = {
            .high = draw<DynamicLevel>({ff, fff, ffff}),
            .median = draw<DynamicLevel>({p, mp, mf, f}),
            .low = draw<DynamicLevel>({pp, ppp, pppp}),
          },
          .shape = draw<DynamicShape>({cresc, decresc, steady})
        };
        dynamics::shape(burstOfNotes, d);
        // insert to phrase
        phrase.notes.insertVector(burstOfNotes, cursor, PushBehavior::truncate);

        Duration betweenBursts = rollDie(12) * subdiv;
        // cursor = (phrase.notes.end() - 1)->endTime() + betweenBursts;
        cursor += burstLength * subdiv + betweenBursts;
    }
    stepwiseMotion(phrase.notes, phrase.chordScales);

    dynamics::randomAccents(phrase.notes, fffff);
    
    return phrase;
};
