/*
  ==============================================================================

    Lead.cpp
    Created: 28 May 2023 5:59:04pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Lead.h"
#include "Ensemble.h"


Phrase Lead::newPhrase() const {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = ensemble.editorState.getPhraseLength();
  
  // if (ensemble.doesntHavePhrase(harmonyKey, startTime, phraseLength)) { 
  //   return newPhrase();      
  // }
  Voice &generateFromVoice = ensemble.getVoice(harmonyKey);
  auto generateFromPhrase = generateFromVoice.atOrEmpty(startTime);

  juce::String harmonyApproach = ensemble.editorState.getChoiceValue(harmonyApproachKey);
  Probability chordProbabilityPerAccent = ensemble.editorState.getKnobValue(harmonyProbabilityKey);
  double harmonicDensity = ensemble.editorState.getKnobValue(harmonyDensityKey);
  
  Phrase harmony = generateFromPhrase.chordScales.empty() 
    ? harmony::generateChordScales(generateFromPhrase,
        getHarmonyApproach(harmonyApproach),
        chordProbabilityPerAccent, 
        harmonicDensity) 
    : generateFromPhrase;

  Phrase phrase = melody::melody(harmony);
  
  
  dynamics::randomAccents(phrase.notes, mf, ff);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  phrase.voice = name;
  return phrase;
}

Phrase Lead::phraseFrom() const {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
    
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
      return newPhrase();      
    }
    Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
    auto generateFromPhrase = generateFromVoice.atOrEmpty(startTime);


    juce::String harmonyApproach = ensemble.editorState.getChoiceValue(harmonyApproachKey);
    Probability chordProbabilityPerAccent = ensemble.editorState.getKnobValue(harmonyProbabilityKey);
    double harmonicDensity = ensemble.editorState.getKnobValue(harmonyDensityKey);
    
    Phrase harmony = generateFromPhrase.chordScales.empty() 
      ? harmony::generateChordScales(generateFromPhrase,
          getHarmonyApproach(harmonyApproach),
          chordProbabilityPerAccent, 
          harmonicDensity) 
      : generateFromPhrase;

    Phrase phrase = melody::melody(harmony);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    phrase.voice = name;
    return phrase;
}

Phrase Lead::variation() const {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = atOrEmpty(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
