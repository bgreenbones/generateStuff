/*
  ==============================================================================

    Lead.cpp
    Created: 28 May 2023 5:59:04pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Lead.h"
#include "Ensemble.h"


Phrase Lead::newPhrase() {
  Position startTime = ensemble.editorState.getStartTime();
  Duration phraseLength = ensemble.editorState.getPhraseLength();
  
  if (ensemble.doesntHavePhrase(harmonyKey, startTime, phraseLength)) { 
    return newPhrase();      
  }
  Voice &generateFromVoice = ensemble.getVoice(harmonyKey);
  auto generateFromPhrase = generateFromVoice.schedule.at(startTime);

  Phrase harmony = generateFromPhrase.chordScales.empty() ? harmony::generateChordScales(generateFromPhrase, ensemble, ensemble.editorState) : generateFromPhrase;

  Phrase phrase = melody::melody(harmony);
  
  
  dynamics::randomAccents(phrase.notes, mf, ff);
  dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
  return phrase;
}

Phrase Lead::phraseFrom() {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
    
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
      return newPhrase();      
    }
    Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
    auto generateFromPhrase = generateFromVoice.schedule.at(startTime);

    int minNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMinNoteLengthKey);
    int maxNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMaxNoteLengthKey);

    Phrase phrase = rhythm::claveFrom(
        generateFromPhrase, 
        minNoteLengthInSubdivisions, 
        maxNoteLengthInSubdivisions);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    return phrase;
}

Phrase Lead::variation() {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = schedule.at(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
