//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"
#include "Pitch.h"

Phrase Generator::fromNothing(string phraseKey, GenerationFunction phraseFunction) {
    auto phrase = Phrase(editorState->getSubdivision(),
                         editorState->getStartTime(),
                         editorState->getPhraseLength());
    phrase = phraseFunction(phrase, *editorState.get(), phraseKey);
    playQueue->queuePhrase(phraseKey, phrase);
    return phrase;
}

Phrase Generator::from(string generatePhraseKey, string generateFromPhraseKey, GenerationFunction phraseFunction) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { this->generate(generateFromPhraseKey); }
    Voice voice = playQueue->getVoice(generateFromPhraseKey);
    auto result = phraseFunction(voice.base, *editorState.get(), generatePhraseKey);
    playQueue->queuePhrase(generatePhraseKey, result);
    return result;
}

Phrase Generator::flipClave(string phraseKey) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) { return Phrase(); } // TODO: use std::optional in failure cases.
    Voice voice = playQueue->getVoice(phraseKey );
    auto flipped = rhythm::flip(voice.base); // TODO: segment the phrase by relevant start and duration
    playQueue->queuePhrase(phraseKey, flipped);
    
    return flipped;
}

void Generator::connecting(string phraseKey,
                     Probability connectingProb, // TODO: just get all this stuff from editor state instead of passing it in
                     Probability associationProb,
                     Probability connectingLengthProb) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) return;
    Voice voice = playQueue->getVoice(phraseKey);
    Phrase phrasePhrase = voice.base;
    Phrase connectingPhrase = phrasePhrase.fillWithRolls(connectingProb, associationProb, connectingLengthProb);
    playQueue->queuePhrase(phraseKey, connectingPhrase);
}

vector<OrnamentSimple> getOrnamentVector(bool flams, bool drags, bool ruffs) {
    vector<OrnamentSimple> result;
    if (flams) { result.push_back(flam); }
    if (drags) { result.push_back(drag); }
    if (ruffs) { result.push_back(ruff); }
    return result;
}

void Generator::ornament(string phraseKey,
                             Probability prob,
                             double breadth,
                             bool flams,
                             bool drags,
                             bool ruffs) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) return;
    Voice voice = playQueue->getVoice(phraseKey);
    Phrase phrasePhrase = voice.base;
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    if (possibleOrnaments.empty()) { return; }
    Phrase ornamentsPhrase = phrasePhrase.addOrnaments(possibleOrnaments, prob, breadth);
    playQueue->queuePhrase(phraseKey, ornamentsPhrase);
}


string Generator::connectingKey(string phraseKey) {
    return phraseKey + "Connecting";
}

string Generator::ornamentsKey(string phraseKey) {
    return phraseKey + "Ornaments";
}
