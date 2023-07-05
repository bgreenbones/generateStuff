/*
  ==============================================================================

    Clave.cpp
    Created: 28 May 2023 3:28:21pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Clave.h"
#include "Ensemble.h"


Phrase Clave::newPhrase() const {
    // int minNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMinNoteLengthKey);
    // int maxNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMaxNoteLengthKey);

    Phrase phrase = ensemble.emptyPhrase(name); 
    // Phrase phrase = rhythm::randomClave(
    //     ensemble.emptyPhrase(name), 
    //     minNoteLengthInSubdivisions, 
    //     maxNoteLengthInSubdivisions);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    phrase.voice = name;
    return phrase;
}

Phrase Clave::phraseFrom() const {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
    
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
      return newPhrase();      
    }
    Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
    auto generateFromPhrase = generateFromVoice.atOrEmpty(startTime);

    int minNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMinNoteLengthKey);
    int maxNoteLengthInSubdivisions = ensemble.editorState.getKnobValue(claveMaxNoteLengthKey);

    Phrase phrase = rhythm::claveFrom(
        generateFromPhrase, 
        minNoteLengthInSubdivisions, 
        maxNoteLengthInSubdivisions);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    phrase.voice = name;
    return phrase;
}

Phrase Clave::variation() const {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = atOrEmpty(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
