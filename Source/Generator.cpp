//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"
#include "Pitch.h"
#include "FormMusical.h"

Phrase Generator::fromNothing(string phraseKey, GenerationFunction phraseFunction) {
    auto phrase = Phrase(editorState.getSubdivision(),
                         editorState.getStartTime(),
                         editorState.getPhraseLength());
    // phrase = phraseFunction(phrase, editorState);
    phrase = phraseFunction(phrase, ensemble, editorState);
    dynamics::randomFlux(phrase.notes); // why the heck not give everything a little life.
    phrase.voice = phraseKey;
    ensemble.queuePhrase(Form(), phrase);
    return phrase;
}

Phrase Generator::from(string generatePhraseKey, string generateFromPhraseKey, GenerationFunction phraseFunction) {
    Position startTime = editorState.getStartTime();
    Duration phraseLength = editorState.getPhraseLength();
    if (ensemble.doesntHavePhrase(generateFromPhraseKey, startTime, phraseLength)) { this->generate(generateFromPhraseKey); }
    Voice &voice = ensemble.getVoice(generateFromPhraseKey);
    auto result = phraseFunction(voice.schedule.at(editorState.phraseStartTime), ensemble, editorState);
    dynamics::randomFlux(result.notes); // why the heck not give everything a little life.
    result.voice = generatePhraseKey;
    ensemble.queuePhrase(Form(), result);
    return result;
}


void Generator::connecting(string phraseKey,
                     Probability connectingProb, // TODO: just get all this stuff from editor state instead of passing it in
                     Probability associationProb,
                     Probability connectingLengthProb) {
    Position startTime = editorState.getStartTime();
    Duration phraseLength = editorState.getPhraseLength();
    if (ensemble.doesntHavePhrase(phraseKey, startTime, phraseLength)) return;
    Voice &voice = ensemble.getVoice(phraseKey);
    Phrase phrasePhrase = voice.schedule.at(editorState.phraseStartTime);
    Phrase connectingPhrase = phrasePhrase.fillWithRolls(connectingProb, associationProb, connectingLengthProb);
    ensemble.queuePhrase(Form(), connectingPhrase);
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
    Position startTime = editorState.getStartTime();
    Duration phraseLength = editorState.getPhraseLength();
    if (ensemble.doesntHavePhrase(phraseKey, startTime, phraseLength)) return;
    Voice &voice = ensemble.getVoice(phraseKey);
    Phrase phrasePhrase = voice.schedule.at(editorState.phraseStartTime);
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    if (possibleOrnaments.empty()) { return; }
    Phrase ornamentsPhrase = phrasePhrase.addOrnaments(possibleOrnaments, prob, breadth);
    ensemble.queuePhrase(Form(), ornamentsPhrase);
}


string Generator::connectingKey(string phraseKey) {
    return phraseKey + "Connecting";
}

string Generator::ornamentsKey(string phraseKey) {
    return phraseKey + "Ornaments";
}
