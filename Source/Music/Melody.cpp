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

void melody::stepwiseMotion(vector<Timed<Note>*> notes,
                    Sequence<ChordScale>& scales,
                    PitchRange range
                    // Interval maxJump
                    ) {
  Pitch lastPitch(uniformInt(range.low, range.high));
  for (Timed<Note> *timedNote : notes) {
      Note& note = timedNote->item;
      Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
      Tonality scale = scales.drawByPosition(timedNote->startTime).item.scale;
      Pitch candidatePitch = scale.step(lastPitch, direction);
      // if (!scale.containsPitch(candidatePitch)) { // TODO: make stuff like this into asserts
      //   int i = 1;
      // }
      if (candidatePitch < range.low) {
        candidatePitch = scale.step(lastPitch, Direction::up);
      // if (!scale.containsPitch(candidatePitch)) {
      //   int i = 1;
      // }
      }
      if (candidatePitch > range.high) {
        candidatePitch = scale.step(lastPitch, Direction::down);
      // if (!scale.containsPitch(candidatePitch)) {
      //   int i = 1;
      // }
      }
      note.pitch = candidatePitch;
      lastPitch = note.pitch;
  }
}
void melody::stepwiseMotion(vector<Timed<Note>*> notes,
                    PitchRange range) {
  Pitch lastPitch(uniformInt(range.low, range.high));
  for (Timed<Note> *timedNote : notes) {
      Note& note = timedNote->item;
      Direction direction = flipCoin() ? Direction::down : Direction::up;
      Interval interval = flipCoin() ? M2 : m2;
      Pitch candidatePitch = lastPitch + (int) direction * interval;
      if (candidatePitch < range.low) {
        candidatePitch = lastPitch + (int)interval;
      }
      if (candidatePitch > range.high) {
        candidatePitch = lastPitch - (int)interval;
      }
      note.pitch = candidatePitch;
      lastPitch = note.pitch;
  }
}



void bassPitches(vector<Timed<Note>*> notes,
                    Sequence<ChordScale>& harmonies,
                    Position cursor = 0,
                    PitchRange range = melody::bassRange) {
    for (int i = 0; i <notes.size(); i++) {
      Timed<Note> *timedNote = notes[i];
      Timed<Note> *previousNote = notes[max(i - 1,0)];
      ChordScale chordScale = harmonies.drawByPosition(cursor + timedNote->startTime);
      PitchClass root = chordScale.harmony.root;
      PitchClass chordTone = draw<PitchClass>(chordScale.harmony.getPitchClasses());
      PitchClass bassPitchClass = draw<PitchClass>({root, chordTone}, {0.6, 0.4});
      Note& note = timedNote->item;
      note.pitch = Pitch::randomInRange(bassPitchClass, range);
      note.pitch.makeCloserKeepPitchClass(previousNote->item.pitch, 0.7);
      note.pitch.keepInRange(range);
    }
}
//
//void melody::applyPitchSelector(vector<Timed<Note>*> notes,
//                    Sequence<ChordScale>& harmonies,
//                    function<PitchClass(ChordScale)> pitchClassSelector,
//                    function<int()> octaveSelector,
//                    Position cursor = 0) {
//    for (Timed<Note> *note : notes) {
//      note->pitch = Pitch(pitchClassSelector(harmonies.drawByPosition(cursor + note->startTime)),
//                          octaveSelector());
//    }
//}

Phrase melody::bass(Phrase harmony, Phrase rhythm,
                    // int minimumRepeats, int maximumRepeats, vector<float> burstNoteLengthChoices,
                    PitchRange range, float density) {
  Phrase phrase(harmony);
  phrase.notes = harmony.notes.toMonophonic();
  phrase.notes.clear();
  
  set<Time> harmonicKeyPoints;
  set<Time> rhythmicKeyPoints;
  set<Time> keyPoints;
  

  int minimumRepeats = 1 + density;
  int maximumRepeats = round(1 + 5 * density);
  vector<float> burstNoteLengthChoices = { 1, (float) round(1 + 0.9 * (1 - density)), (float) round(1 + 1.9 * (1 - density)) };


  for (Timed<Note> note : rhythm.notes) {
      note.trim(phrase.getTime());
      rhythmicKeyPoints.emplace(note);
      keyPoints.emplace(note);
  }
  for (Timed<ChordScale> tonality : phrase.chordScales) { harmonicKeyPoints.emplace(tonality); keyPoints.emplace(tonality); }
  
  vector<Position> rhythmicPositions;
  for (Time keyPoint : keyPoints) {
    if (!contains<Time>(harmonicKeyPoints, keyPoint) && flipWeightedCoin(0.4)) { continue; }
    rhythmicPositions.push_back(keyPoint.startTime);
    
    vector<Time> times = rhythm::stabilityBased(keyPoint, rhythm.subdivisions, 0.1, 0.4);
    bool barIsEven = (int)floor(keyPoint.startTime.asBars()) % 2;
    if (barIsEven) {
      Duration subdiv = rhythm.subdivisions.drawByPosition(keyPoint.startTime);
      Duration noteLength = draw<float>(burstNoteLengthChoices) * subdiv;
      
      int numberOfNotes = uniformInt(minimumRepeats, maximumRepeats);
      times = rhythm::nOfLengthM(numberOfNotes, noteLength);
    }

    double portionToModify = (1 - density) * 0.7;
    vector<vector<Time*>> toDoubleOrTriple = rhythm::distinctSubsets<Time>(times, portionToModify, {0.7, 0.3});
    // vector<vector<Time*>> toDoubleOrTriple = rhythm::distinctSubsets<Time>(times, 0.4, {0.7, 0.3});
    vector<Time*> toDouble = toDoubleOrTriple[0];
    vector<Time*> toTriple = toDoubleOrTriple[1];
    rhythm::multiplyTimeLength(times, toDouble, 2);
    rhythm::multiplyTimeLength(times, toTriple, 3);
    
    vector<Timed<Note>> notes = Sequence<Note>::fromTimes(times);
    bassPitches(toPointerVector<Timed<Note>>(notes), phrase.chordScales, keyPoint.startTime, range);

    phrase.notes.insertVector(notes, keyPoint.startTime, PushBehavior::truncate, OverwriteBehavior::cutoff);
  }
  
  dynamics::followAccents(phrase.notes, rhythmicPositions, mf, ff);

  return phrase;
};


Phrase melody::streamOfConsciousness(Phrase harmony) {
    Phrase phrase(harmony);
    phrase.notes = harmony.notes.toMonophonic();
    phrase.notes.clear();
    
    Position cursor = 0;
    while (cursor < phrase.getDuration()) {
        
        // get underlying phrase structures.
        // vector<ChordScale> chordScales = phrase.chordScales.byPosition(cursor);
        // if (chordScales.empty()) {
        //     cursor += 1;
        //     continue;
        // }
        // ChordScale chordScale = chordScales.at(0);
        vector<Timed<Duration>> subdivs = phrase.subdivisions.byPosition(cursor);
        Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
        
        // get stream of times
        int burstLength = 1 + rollDie(flipWeightedCoin(0.35) ? 5 : 12);
        vector<Time> burstOfTimes = rhythm::nOfLengthM(burstLength, subdiv);
        // choose times to modify
        vector<vector<Time*>> toDoubleOrHalf = rhythm::distinctSubsets<Time>(burstOfTimes, 0.25, {0.3, 0.1});
        vector<Time*> toDouble = toDoubleOrHalf[0];
        vector<Time*> toHalf = toDoubleOrHalf[1];
        // // modify times
        rhythm::multiplyTimeLength(burstOfTimes, toDouble, 2);
        rhythm::repeat(burstOfTimes, toHalf, 0.5);
        // convert to notes
        vector<Timed<Note>> burstOfNotes = Sequence<Note>::fromTimes(burstOfTimes);
        
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
        cursor = phrase.notes.last()->endTime() + betweenBursts;
        // cursor += burstLength * subdiv + betweenBursts;
    }

    // choose notes
    stepwiseMotion(toPointerVector<Timed<Note>>(phrase.notes), phrase.chordScales);
    dynamics::randomAccents(phrase.notes, fffff);
    
    return phrase;
};

// todo: doesn't work yet for max jump less than and octave
void melody::decreaseSpread(vector<Timed<Note>> & melody, Interval maxJump) {
  if (melody.size() < 2) {
    return;
  }
  Interval firstRoundInterval = max(maxJump, octave);
  for (int i = 0; i < melody.size() - 1; i++) {
    int j = i + 1;
    int transpose = 0;
    while (melody[i].item.pitch - melody[j].item.pitch >= firstRoundInterval) {
      int pitchDifference = melody[j].item.pitch.pitchValue - melody[i].item.pitch.pitchValue;
      int sign = pitchDifference / abs(pitchDifference);
      transpose += sign * octave; 
      melody[j].item.pitch -= sign * octave;
    }
    while (++j < melody.size()) {
      melody[j].item.pitch -= transpose;
    }
  }
}
vector<Timed<Note>> melody::shape(Duration shapeLength, Duration subdivision, PitchRange range, float density) {
    int subdivsPerBeat = Beats(1) / subdivision.asBeats();
    Duration leadingRest = uniformInt(0, subdivsPerBeat) * subdivision;
    Position cursor = leadingRest;
    vector<Timed<Note>> result;
    while (cursor < shapeLength) {
      int maxDurationInSubdivisions = (shapeLength - cursor) / subdivision;
      
        int longestPossibleBurst = (subdivsPerBeat * 2) + density * subdivsPerBeat * 2;
      // get stream of times
      int longestBurst = min(longestPossibleBurst, maxDurationInSubdivisions);
      // int longestBurst = min(12, maxDurationInSubdivisions);
      int shortBurst = min(5, longestBurst);

      int burstLength = rollDie(flipWeightedCoin(0.35) ? shortBurst : longestBurst);
      
      double modifyProportion = uniformDouble(0.15, 0.35);
      double doubleProportion = 0.6 * (1. - density); 
      double halfProportion = 0.2 * density;
      vector<Time> times = rhythm::doublesAndDiddles(rhythm::nOfLengthM(burstLength, subdivision), modifyProportion, doubleProportion, halfProportion);
      vector<Timed<Note>> notes = Sequence<Note>::fromTimes(times, cursor);
      dynamics::shape(notes);
      // choose notes
      stepwiseMotion(toPointerVector<Timed<Note>>(notes), range);
      result.insert(result.end(), notes.begin(), notes.end());

      Duration betweenBursts = rollDie(4) * subdivision;
      cursor = result.back().endTime() + betweenBursts;
    }
    dynamics::randomAccents(result, fffff);
    melody::decreaseSpread(result, octave);
    return result;
};


double melody::slope(vector<Timed<Note>> shape) {
  if (shape.size() < 2) {
    return 0;
  }
  if (shape.size() == 1) {
    return 0;
  }
//  double shapeWidth = shape.back().startTime - shape.front().endTime();
  double shapeHeight = shape.back().item.pitch.pitchValue - shape.front().item.pitch.pitchValue;
  // return shapeHeight / shapeWidth;
  return shapeHeight;
}

Phrase melody::streamWithThemes(Phrase harmony, PitchRange range, float density) {
    Phrase phrase(harmony);
    phrase.notes = harmony.notes.toMonophonic();
    phrase.notes.clear();

    Duration subdivision = harmony.subdivisions.primary();
    int numberOfThemes = 4;
    vector<vector<Timed<Note>>> themes;
    while (numberOfThemes-- > 0) {
      Duration themeLength = Beats(3 + numberOfThemes);
      themes.push_back(shape(themeLength, subdivision, range, density));
    }

    Position cursor = 0;
    // Tonality firstScale = harmony.chordScales[0].scale;
    while (cursor < phrase.getDuration()) {
        vector<Timed<Note>> themeToInsert = draw<vector<Timed<Note>>>(themes);
        if (themeToInsert.empty()) {
          continue;
        }
        if (!phrase.notes.empty()) {
          int noJumpTranspose = phrase.notes.back().item.pitch.pitchValue - themeToInsert[0].item.pitch.pitchValue;
          for (Timed<Note>& note : themeToInsert) {
              note.item.pitch += noJumpTranspose;
          }
          double gravity = themeToInsert.back().item.pitch.gravity(range);
          double slope = melody::slope(themeToInsert);
          int avoidExtremesTranspose = gravity * rollDie(24) + slope / rollDie(3);
          for (Timed<Note>& note : themeToInsert) {
              note.item.pitch -= avoidExtremesTranspose;
          }
        }
        // insert to phrase
        phrase.notes.insertVector(themeToInsert, cursor, PushBehavior::truncate);

        Duration betweenBursts = rollDie(8) * subdivision;
        cursor = quantize(phrase.notes.last()->endTime() + betweenBursts, Beats(1));
        // cursor += burstLength * subdiv + betweenBursts;
    }
    phrase.pitchQuantize();

    return phrase;
}

Phrase melody::repeatingShape(Phrase harmony, Duration shapeLength) {
    Phrase phrase(harmony);
    phrase.notes = harmony.notes.toMonophonic();
    phrase.notes.clear();

    Phrase shortHarmony(harmony);
    shortHarmony.setDuration(shapeLength);

    Phrase shape = streamOfConsciousness(shortHarmony);
    
    Phrase result = shape.loop(harmony.getDuration());
    result.chordScales = harmony.chordScales;
    if (result.notes.empty()) {
      return result;
    }
    
    Tonality firstScale = harmony.chordScales.drawByPosition(result.notes[0].startTime).item.scale;
    int transpose = 0;
    int repetition = 1;
    for (Timed<Note>& note : result.notes) {
        if (note.startTime >= repetition * shapeLength) {
          repetition++;
          Tonality scale = harmony.chordScales.drawByPosition(note.startTime).item.scale;
          // if (scale.root != firstScale.root) {
              Interval goUp = pitchClassInterval(firstScale.root, scale.root);
              transpose = (int)goUp;
              if (flipCoin()) {
                  Interval goDown = invert(goUp);
                  transpose = -(int)goDown;
              }
          // }
        }
        note.item.pitch += transpose;
    }
    result.pitchQuantize();
    return result;
}
