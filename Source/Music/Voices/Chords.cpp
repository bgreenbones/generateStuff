/*
  ==============================================================================

    Chords.cpp
    Created: 28 May 2023 5:58:21pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Chords.h"
#include "Ensemble.h"


Phrase Chords::newPhrase() const {
  juce::String harmonyApproach = ensemble.editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent = ensemble.editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = ensemble.editorState.getKnobValue(harmonyDensityKey);
  
  Phrase harmony = harmony::generateChordScales(ensemble.emptyPhrase(harmonyKey),
    getHarmonyApproach(harmonyApproach), 
    chordProbabilityPerAccent,
    harmonicDensity);

  Phrase phrase = harmony::randomVoicings(harmony);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  phrase.voice = name;
  return phrase;
}

Phrase Chords::phraseFrom() const {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = ensemble.editorState.getPhraseLength();
  VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
  
  if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
    return newPhrase();      
  }
  Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
  auto generateFromPhrase = generateFromVoice.atOrEmpty(startTime);

  generateFromPhrase = generateFromPhrase.loop(phraseLength);
  
  juce::String harmonyApproach = ensemble.editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent = ensemble.editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = ensemble.editorState.getKnobValue(harmonyDensityKey);

  Phrase harmony = generateFromPhrase.chordScales.empty() 
    ? harmony::generateChordScales(generateFromPhrase, 
        getHarmonyApproach(harmonyApproach), 
        chordProbabilityPerAccent, 
        harmonicDensity) 
    : generateFromPhrase;
  // harmony.chordScales.clear();

  Phrase phrase = harmony::randomVoicings(harmony);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  phrase.voice = name;
  return phrase;
}

Phrase Chords::variation() const {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = atOrEmpty(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
