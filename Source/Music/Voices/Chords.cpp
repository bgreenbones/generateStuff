/*
  ==============================================================================

    Chords.cpp
    Created: 28 May 2023 5:58:21pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Chords.h"
#include "Ensemble.h"


Phrase Chords::newPhrase() {
  juce::String harmonyApproach = editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent =editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = editorState.getKnobValue(harmonyDensityKey);
  
  Phrase harmony = harmony::generateChordScales(ensemble.emptyPhrase(),
    harmonyApproach,
    chordProbabilityPerAccent,
    harmonicDensity);

  Phrase phrase = harmony::randomVoicings(harmony);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  return phrase;
}



Phrase Chords::phraseFrom() {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = ensemble.editorState.getPhraseLength();
  VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
  
  if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
    return newPhrase();      
  }
  Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
  auto generateFromPhrase = generateFromVoice.schedule.at(startTime);

  generateFromPhrase = generateFromPhrase.loop(phraseLength);
  
  juce::String harmonyApproach = editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent =editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = editorState.getKnobValue(harmonyDensityKey);

  Phrase harmony = generateFromPhrase.chordScales.empty() 
    ? generateChordScales(generateFromPhrase, 
      harmonyApproach, 
      chordProbabilityPerAccent, 
      harmonicDensity) 
    : generateFromPhrase;
  harmony.chordScales.clear();

  Phrase phrase = harmony::randomVoicings(harmony);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  return phrase;
}

Phrase Chords::variation() {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = schedule.at(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
