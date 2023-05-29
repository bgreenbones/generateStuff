/*
  ==============================================================================

    Voice.cpp
    Created: 26 May 2023 12:17:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Voice.h"
#include "Ensemble.h"

//void Voice::variation() {
//    Position startTime = ensemble.editorState.getStartTime();
//    Duration phraseLength = ensemble.editorState.getPhraseLength();
//    
//    
//    
//    // if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { this->generate(generateFromPhraseKey); }
//    // Voice voice = ensemble.getVoice(generateFromPhraseKey);
//    
//    
//    // auto result = variationFunction(schedule.at(ensemble.editorState.phraseStartTime), ensemble, ensemble.editorState);
//    auto result = variationFunction(*this);
//    // dynamics::randomFlux(result.notes); // why the heck not give everything a little life.
//    result.voice = name;
//    ensemble.queuePhrase(Form(), result);
//    // schedule.schedulePhrase(Form(), result);
//
//}


// Phrase Voice::newPhrase() {
//     // auto phrase = Phrase(editorState.getSubdivision(),
//     //                      editorState.getStartTime(),
//     //                      editorState.getPhraseLength());
//     Phrase phrase = newPhraseFunction(*this);
//     dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
//     phrase.voice = name;
//     // ensemble.queuePhrase(Form(), phrase);
//     schedule.schedulePhrase(Form(), phrase);
//     return phrase;
// }

// Phrase Voice::from(string generatePhraseKey, string generateFromPhraseKey, GenerationFunction phraseFunction) {
//     Position startTime = editorState.getStartTime();
//     Duration phraseLength = editorState.getPhraseLength();
//     if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { this->generate(generateFromPhraseKey); }
//     Voice voice = ensemble.getVoice(generateFromPhraseKey);
//     auto result = phraseFunction(voice.schedule.at(editorState.phraseStartTime), ensemble, editorState);
//     dynamics::randomFlux(result.notes); // why the heck not give everything a little life.
//     result.voice = generatePhraseKey;
//     ensemble.queuePhrase(Form(), result);
//     return result;
// }
