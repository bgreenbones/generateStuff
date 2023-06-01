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

Phrase melody::bass(Phrase harmony, Phrase rhythm, int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices) {
  Phrase phrase(harmony);
  phrase.notes = harmony.notes.toMonophonic();
  phrase.notes.clear();
  
  // set<Position> keyPoints { phrase.startTime };
  set<Timed> harmonicKeyPoints;
  set<Timed> rhythmicKeyPoints;
  
  auto notes = harmony.toMonophonic();
  auto accents = notes.accents();
  
  // for (Note note : notes.notes) { keyPoints.emplace(note.startTime); }
  // for (Note note : rhythm.notes) { keyPoints.emplace(note.startTime); }
  // for (ChordScale tonality : phrase.chordScales) { keyPoints.emplace(tonality.startTime); }
  // for (Note note : notes.notes) { keyPoints.emplace(note); }
  for (Note note : rhythm.notes) { rhythmicKeyPoints.emplace(note); }
  for (ChordScale tonality : phrase.chordScales) { harmonicKeyPoints.emplace(tonality); }
  

  // for (Position keyPoint : keyPoints) {
  for (Timed keyPoint : harmonicKeyPoints) {
    vector<ChordScale> chordScales = phrase.chordScales.byPosition(keyPoint.startTime);
    if (chordScales.empty()) { continue; }
    ChordScale chordScale = chordScales.at(0);
    Tonality tonality = chordScale.harmony;
    
    float burstNoteLengthInSubdivisions = draw<float>(burstNoteLengthChoices);
    
    // Note noteToAdd(Pitch(tonality.root, 3), 70, keyPoint, chordScale.endTime() - keyPoint);
    Note noteToAdd(Pitch(tonality.root, 3), 70, keyPoint.startTime, keyPoint.duration);
    phrase = rhythm::burst(phrase, noteToAdd, burstLengthMin, burstLengthMax, burstNoteLengthInSubdivisions);
  }

  
  vector<Position> rhythmicPositions;
  for (Timed keyPoint : rhythmicKeyPoints) {
    rhythmicPositions.push_back(keyPoint.startTime);
    
    vector<ChordScale> chordScales = phrase.chordScales.byPosition(keyPoint.startTime);
    if (chordScales.empty() || flipWeightedCoin(0.4)) { continue; }
    ChordScale chordScale = chordScales.at(0);
    Tonality tonality = chordScale.harmony;
    
    float burstNoteLengthInSubdivisions = draw<float>(burstNoteLengthChoices);
    
    // Note noteToAdd(Pitch(tonality.root, 3), 70, keyPoint, chordScale.endTime() - keyPoint);
    Note noteToAdd(Pitch(tonality.root, 3), 70, keyPoint.startTime, keyPoint.duration);
    phrase = rhythm::burst(phrase, noteToAdd, burstLengthMin, burstLengthMax, burstNoteLengthInSubdivisions);
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
        vector<Timed> burstOfTimes = rhythm::nOfLengthM(1 + rollDie(flipCoin() ? 3 : 7), subdiv);
        // choose times to modify
        vector<Timed*> toDoubleOrHalf = rhythm::selectAtRandom(burstOfTimes, 0.25);
        // modify times
        rhythm::multiplyTimeLength(burstOfTimes, toDoubleOrHalf, 1);
        // convert to notes
        vector<Note> burstOfNotes = Sequence<Note>::fromTimed(burstOfTimes);
        // choose notes
        lastPitch = stepwiseMotion(burstOfNotes, phrase.chordScales, lastPitch, cursor);
        // insert to phrase
        phrase.notes.insertVector(burstOfNotes, cursor, PushBehavior::truncate);
        
        cursor = (phrase.notes.end() - 1)->endTime() + rollDie(3) * subdiv;
    }
    
    return phrase;
};
