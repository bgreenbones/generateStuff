/*
  ==============================================================================

    Bass.cpp
    Created: 28 May 2023 3:29:05pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Bass.h"
#include "Ensemble.h"

Phrase Bass::newPhrase() {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = editorState.getPhraseLength();

  Phrase harmony = ensemble.at(startTime, harmonyKey);
  harmony = harmony.chordScales.empty() 
    ? harmony::generateChordScales(harmony, ensemble, ensemble.editorState) 
    : harmony;
  harmony = harmony.loop(phraseLength);
  
  Phrase rhythm = ensemble.at(startTime, claveKey);
  rhythm = rhythm.loop(phraseLength);
  
  int burstLengthMin = editorState.getKnobValue(bassBurstLengthMinKey);
  int burstLengthMax = editorState.getKnobValue(bassBurstLengthMaxKey);

  vector<float> burstNoteLengthChoices;
  if (editorState.getButtonValue(bassBurstNoteLengthHalfKey)) { burstNoteLengthChoices.push_back(0.5); };
  if (editorState.getButtonValue(bassBurstNoteLengthOneKey)) { burstNoteLengthChoices.push_back(1); };
  if (editorState.getButtonValue(bassBurstNoteLengthTwoKey)) { burstNoteLengthChoices.push_back(2); };
  if (editorState.getButtonValue(bassBurstNoteLengthThreeKey)) { burstNoteLengthChoices.push_back(3); };
  
  Phrase phrase = melody::bass(
      harmony, 
      rhythm, 
      burstLengthMin,
      burstLengthMax,
      burstNoteLengthChoices);

  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  return phrase;
}

Phrase Bass::phraseFrom() {
  return newPhrase(); 
}

Phrase Bass::variation() {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = schedule.at(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}