/*
  ==============================================================================

    Voice.cpp
    Created: 26 May 2023 12:17:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Voice.h"
#include "PlayQueue.h"

void Voice::variation() {
    Position startTime = playQueue.editorState.getStartTime();
    Duration phraseLength = playQueue.editorState.getPhraseLength();
    
    
    
    // if (playQueue.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { this->generate(generateFromPhraseKey); }
    // Voice voice = playQueue.getVoice(generateFromPhraseKey);
    
    
    // auto result = variationFunction(schedule.at(playQueue.editorState.phraseStartTime), playQueue, playQueue.editorState);
    auto result = variationFunction(*this);
    // dynamics::randomFlux(result.notes); // why the heck not give everything a little life.
    result.voice = name;
    playQueue.queuePhrase(Form(), result);
    // schedule.schedulePhrase(Form(), result);

}