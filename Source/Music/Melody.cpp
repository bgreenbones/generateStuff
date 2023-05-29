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



Phrase melody::bass(Phrase harmony, Phrase rhythm, int burstLengthMin, int burstLengthMax, vector<float> burstNoteLengthChoices) {
  Phrase phrase(harmony);
  phrase.notes = harmony.notes.toMonophonic();
  phrase.notes.clear();
  
  // set<Position> keyPoints { phrase.startTime };
  set<TimedEvent> harmonicKeyPoints;
  set<TimedEvent> rhythmicKeyPoints;
  
  auto notes = harmony.toMonophonic();
  auto accents = notes.accents();
  
  // for (Note note : notes.notes) { keyPoints.emplace(note.startTime); }
  // for (Note note : rhythm.notes) { keyPoints.emplace(note.startTime); }
  // for (ChordScale tonality : phrase.chordScales) { keyPoints.emplace(tonality.startTime); }
  // for (Note note : notes.notes) { keyPoints.emplace(note); }
  for (Note note : rhythm.notes) { rhythmicKeyPoints.emplace(note); }
  for (ChordScale tonality : phrase.chordScales) { harmonicKeyPoints.emplace(tonality); }
  

  // for (Position keyPoint : keyPoints) {
  for (TimedEvent keyPoint : harmonicKeyPoints) {
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
  for (TimedEvent keyPoint : rhythmicKeyPoints) {
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
        vector<ChordScale> chordScales = phrase.chordScales.byPosition(cursor);
        if (chordScales.empty()) {
            cursor += 1;
            continue;
        }
        
        ChordScale chordScale = chordScales.at(0);
        // Tonality tonality = chordScale.scale;
        
        vector<Subdivision> subdivs = phrase.subdivisions.byPosition(cursor);
        Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
        
        // vector<Note> burstOfNotes = Sequence<Note>::burst(subdiv, min(1 + rollDie(4), (int) ((chordScale.endTime() - cursor) / subdiv)));
        vector<Note> burstOfNotes = Sequence<Note>::burst(subdiv, 1 + rollDie(flipCoin() ? 3 : 7));

        // fun with durations
        for (auto noteIter = burstOfNotes.begin(); noteIter < burstOfNotes.end(); noteIter++) {
            Note& note = *noteIter;
            
            if (flipWeightedCoin(0.75)) {
                continue;
            }
            
            if (flipCoin()) {
                // lengthen note by 2, erase next overlap
                note.duration = 2 * note.duration;
                if (noteIter + 1 == burstOfNotes.end()) { continue; }
                else if ((noteIter + 1)->startTime < note.endTime()) {
                    burstOfNotes.erase(noteIter + 1);
                }
            } else {
                // double stroke
                note.duration = 0.5 * note.duration;
                Note doubleNote(note);
                doubleNote.startTime = note.endTime();
                noteIter = burstOfNotes.insert(noteIter + 1, doubleNote);
            }
        }

        // stepwise motion
        for (Note &note : burstOfNotes) {
            Direction direction = rollDie(2) == 2 ? Direction::down : Direction::up;
            note.pitch = phrase.chordScales.drawByPosition(cursor + note.startTime).scale
                .step(lastPitch, direction);
            lastPitch = note.pitch;
        }
        
        phrase.notes.insertVector(burstOfNotes, cursor, PushBehavior::truncate);
        
        cursor = (phrase.notes.end() - 1)->startTime + rollDie(4) * subdiv;
        // cursor += (7 + rollDie(4)) * subdiv;
    }
    
    // This was a check for monophonicity... don't think we need it.
    // vector<Position> startTimes;
    // for (Note note : phrase.notes) {
    //     if (find(startTimes.begin(), startTimes.end(), note.startTime) != startTimes.end()) {
    //         DBG("bad");
    //     }
    //     startTimes.push_back(note.startTime);
    // }


    return phrase;
};