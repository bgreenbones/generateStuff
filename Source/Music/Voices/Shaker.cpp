/*
  ==============================================================================

    Shaker.cpp
    Created: 28 May 2023 3:30:10pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Shaker.h"
#include "Ensemble.h"


Phrase Shaker::phrase() {
    Phrase phrase = ensemble.emptyPhrase(name); 
    phrase = phrase.randomGhostSubdivision(0.9,1.);
    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    return phrase;
}

Phrase Shaker::phraseFrom() {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    VoiceName generateFromPhraseKey = ensemble.editorState.useAsSourcePhraseKey;
    
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { 
      return phrase();      
    }
    return phrase;
}

Phrase Shaker::variation() {
  
    Position phraseStartTime = ensemble.editorState.phraseStartTime;
    Duration phraseLength = ensemble.editorState.getPhraseLength();

    Phrase source = schedule.at(phraseStartTime);

    return rhythm::rhythmicVariation(source);
}
