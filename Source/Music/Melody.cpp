/*
  ==============================================================================

    Melody.cpp
    Created: 8 Mar 2023 10:51:07pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Melody.h"
#include "PlayQueue.h"
#include "Utility.h"



Phrase melody::bassFromFunction(Phrase fromPhrase, shared_ptr<PlayQueue> playQueue, GenerateStuffEditorState const& editorState) {
//        Parameter(bassBurstLengthMinKey, "min burst length", burstLengthRange, 1, " notes"),
//        Parameter(bassBurstLengthMaxKey, "max burst length", burstLengthRange, 4, " notes"),
//        Parameter(bassBurstNoteLengthHalfKey, "burst note length 1/2", false, " subdivisions"),
//        Parameter(bassBurstNoteLengthOneKey, "burst note length 1", true, " subdivisions"),
//        Parameter(bassBurstNoteLengthTwoKey, "burst note length 2", false, " subdivisions"),
//        Parameter(bassBurstNoteLengthThreeKey, "burst note length 3", false, " subdivisions")
  
  int burstLengthMin = editorState.getKnobValue(bassBurstLengthMinKey);
  int burstLengthMax = editorState.getKnobValue(bassBurstLengthMaxKey);

  Duration phraseLength = editorState.getPhraseLength();
  fromPhrase = fromPhrase.loop(phraseLength);
  
  vector<float> burstNoteLengthChoices;
  if (editorState.getButtonValue(bassBurstNoteLengthHalfKey)) { burstNoteLengthChoices.push_back(0.5); };
  if (editorState.getButtonValue(bassBurstNoteLengthOneKey)) { burstNoteLengthChoices.push_back(1); };
  if (editorState.getButtonValue(bassBurstNoteLengthTwoKey)) { burstNoteLengthChoices.push_back(2); };
  if (editorState.getButtonValue(bassBurstNoteLengthThreeKey)) { burstNoteLengthChoices.push_back(3); };
  
  Phrase phrase(fromPhrase);
  phrase.notes = fromPhrase.notes.toMonophonic();
  phrase = phrase.chordScales.empty() ? harmony::generateChordScales(phrase, playQueue, editorState) : phrase;
  phrase.notes.clear();
  
  // set<Position> keyPoints { phrase.startTime };
  set<TimedEvent> harmonicKeyPoints;
  set<TimedEvent> rhythmicKeyPoints;
  
  auto notes = fromPhrase.toMonophonic();
  auto accents = notes.accents();
  Phrase clave = playQueue->at(0, claveKey);
  clave = clave.loop(phraseLength);
  
  // for (Note note : notes.notes) { keyPoints.emplace(note.startTime); }
  // for (Note note : clave.notes) { keyPoints.emplace(note.startTime); }
  // for (ChordScale tonality : phrase.chordScales) { keyPoints.emplace(tonality.startTime); }
  // for (Note note : notes.notes) { keyPoints.emplace(note); }
  for (Note note : clave.notes) { rhythmicKeyPoints.emplace(note); }
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
