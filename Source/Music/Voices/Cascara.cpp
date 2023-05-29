/*
  ==============================================================================

    Cascara.cpp
    Created: 28 May 2023 3:28:40pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Cascara.h"
#include "Ensemble.h"


Phrase Cascara::newPhrase() {
    float pDisplace = editorState.getKnobValue(cascaraDisplaceProbabilityKey);
    float pDouble = editorState.getKnobValue(cascaraDoubleProbabilityKey);

    Phrase phrase = rhythm::randomCascara(
        ensemble.emptyPhrase(name), 
        pDisplace, 
        pDouble);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    return phrase;
}

Phrase Cascara::phraseFrom() {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
    
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
      return newPhrase();      
    }
    Voice &generateFromVoice = ensemble.getVoice(generateFromPhraseKey);
    auto generateFromPhrase = generateFromVoice.schedule.at(startTime);

    Phrase phrase = rhythm::cascaraFrom(generateFromPhrase);

    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    return phrase;
}

Phrase Cascara::variation() {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = schedule.at(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
