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

void melody::arpeggiator(vector<Note>& notes,
                    Tonality tonality) {
  for (Note &note : notes) {
      // Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
      note.pitch = flipWeightedCoin(0.6) 
        ? Pitch(tonality.root, flipCoin() ? draw<int>({2, 3}) : draw<int>({3, 4}))
        : draw<Pitch>(tonality.getPitches());
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
  for (Timed keyPoint : keyPoints) {
    if (!contains<Timed>(harmonicKeyPoints, keyPoint) && flipWeightedCoin(0.4)) { continue; }
    rhythmicPositions.push_back(keyPoint.startTime);
    
    ChordScale chordScale = phrase.chordScales.drawByPosition(keyPoint.startTime);
    Tonality tonality = chordScale.harmony;
    
    Duration subdiv = rhythm.subdivisions.drawByPosition(keyPoint.startTime);
    Duration noteLength = draw<float>(burstNoteLengthChoices) * subdiv;
    
    int numberOfNotes = uniformInt(minimumRepeats, maximumRepeats);
    vector<Timed> times = rhythm::nOfLengthM(numberOfNotes, noteLength);

    vector<Timed*> toHalf = rhythm::selectAtRandom(times, 0.25);
    rhythm::multiplyTimeLength(times, toHalf, 2);
    
    // Note note(Pitch(tonality.root, 3), 70, keyPoint.startTime, noteLength);
    // vector<Note> notes = Sequence<Note>::fromTimed(times, note);
    vector<Note> notes = Sequence<Note>::fromTimed(times);
    arpeggiator(notes, tonality);
    phrase.notes.insertVector(notes, keyPoint.startTime, PushBehavior::ignore, OverwriteBehavior::cutoff);
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
        vector<Timed> burstOfTimes = rhythm::nOfLengthM(1 + rollDie(flipWeightedCoin(0.35) ? 5 : 12), subdiv);
        // choose times to modify
        // vector<vector<Timed*>> toDoubleOrHalf = rhythm::distinctSubsets(burstOfTimes, 2, 0.25);
        vector<Timed*> toDouble = rhythm::selectAtRandom(burstOfTimes, 0.25);
        vector<Timed*> toHalf = rhythm::selectAtRandom(burstOfTimes, 0.1);
        // // modify times
        // rhythm::multiplyTimeLength(burstOfTimes, toDoubleOrHalf[0], 2);
        // rhythm::repeat(burstOfTimes, toDoubleOrHalf[1], 0.5);
        rhythm::repeat(burstOfTimes, toHalf, 0.5);
        rhythm::multiplyTimeLength(burstOfTimes, toDouble, 2);
        // convert to notes
        vector<Note> burstOfNotes = Sequence<Note>::fromTimed(burstOfTimes);
        // choose notes
        lastPitch = stepwiseMotion(burstOfNotes, phrase.chordScales, lastPitch, cursor);
        
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
        
        cursor = (phrase.notes.end() - 1)->endTime() + rollDie(12) * subdiv;
    }

     dynamics::randomAccents(phrase.notes, fffff);
    
    return phrase;
};
