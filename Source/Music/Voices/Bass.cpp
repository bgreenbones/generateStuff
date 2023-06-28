/*
  ==============================================================================

    Bass.cpp
    Created: 28 May 2023 3:29:05pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Bass.h"
#include "Ensemble.h"

Phrase Bass::newPhrase() const {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = ensemble.editorState.getPhraseLength();

  juce::String harmonyApproach = ensemble.editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent = ensemble.editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = ensemble.editorState.getKnobValue(harmonyDensityKey);

  Phrase harmony = ensemble.at(startTime, harmonyKey);
  harmony = harmony.chordScales.empty() 
    ? harmony::generateChordScales(harmony,
    {
      .approach = getHarmonyApproach(harmonyApproach),
      .chordProbabilityPerAccent = chordProbabilityPerAccent,     
      .harmonicDensity = harmonicDensity
    })
    : harmony;
  harmony = harmony.loop(phraseLength);
  
  Phrase rhythm = ensemble.at(startTime, claveKey);
  rhythm = rhythm.loop(phraseLength);
  
  int burstLengthMin = ensemble.editorState.getKnobValue(bassBurstLengthMinKey);
  int burstLengthMax = ensemble.editorState.getKnobValue(bassBurstLengthMaxKey);

  vector<float> burstNoteLengthChoices;
  if (ensemble.editorState.getButtonValue(bassBurstNoteLengthHalfKey)) { burstNoteLengthChoices.push_back(0.5); };
  if (ensemble.editorState.getButtonValue(bassBurstNoteLengthOneKey)) { burstNoteLengthChoices.push_back(1); };
  if (ensemble.editorState.getButtonValue(bassBurstNoteLengthTwoKey)) { burstNoteLengthChoices.push_back(2); };
  if (ensemble.editorState.getButtonValue(bassBurstNoteLengthThreeKey)) { burstNoteLengthChoices.push_back(3); };
  
  Phrase phrase = melody::bass(
      harmony, 
      rhythm
                               );
//      burstLengthMin,
//      burstLengthMax,
//      burstNoteLengthChoices);

  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  phrase.voice = name;
  return phrase;
}

Phrase Bass::phraseFrom() const {
  return newPhrase(); 
}

Phrase Bass::variation() const {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = atOrEmpty(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
