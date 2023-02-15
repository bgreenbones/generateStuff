//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"
#include "Pitch.h"

Phrase Generator::cascara() {
    auto phrase = Phrase(editorState->getSubdivision(),
                         editorState->getStartTime(),
                         editorState->getPhraseLength()).randomCascara();
    playQueue->queuePhrase(cascaraKey, phrase);
    return phrase;
}

Phrase Generator::chords() {
    vector<Pitch> chordToAdd = randomChord();
    Phrase phrase = Phrase(editorState->getSubdivision(),
                           editorState->getStartTime(),
                           editorState->getPhraseLength());
    phrase.notes.monophonic = false;
    for (Pitch pitchToAdd : chordToAdd) {
        Note noteToAdd(pitchToAdd.pitchValue, 70, 0, (quarters)(editorState->getPhraseLength()));
        phrase.addNote(noteToAdd);
    }
    playQueue->queuePhrase(harmonyKey, phrase);
    return phrase;
}

Phrase Generator::clave() {
    auto phrase = Phrase(editorState->getSubdivision(),
                         editorState->getStartTime(),
                         editorState->getPhraseLength()).randomClave();
    playQueue->queuePhrase(claveKey, phrase);
    return phrase;
}

Phrase Generator::cascaraFromClave() {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(claveKey, startTime, phraseLength)) { this->clave(); }
    PhraseAssociation phraseAssociation = playQueue->getAssociatedPhrases(claveKey, startTime, phraseLength);
    auto cascaraPhrase = phraseAssociation.base.cascaraFromClave();
    playQueue->queuePhrase(cascaraKey, cascaraPhrase);
    return cascaraPhrase;
}

Phrase Generator::flipClave(string phraseKey) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) { return Phrase(); } // TODO: use std::optional in failure cases.
    PhraseAssociation phraseAssociation = playQueue->getAssociatedPhrases(cascaraKey, startTime, phraseLength);
    auto flipped = phraseAssociation.base.flip();
    playQueue->queuePhrase(phraseKey, flipped);
    
    bool hasOrnaments = false; // TODO: make this possible
    bool hasRolls = false;
    if (hasOrnaments) {
        auto ornamentsFlipped = phraseAssociation.ornaments.flip();
        playQueue->queueOrnamentation(phraseKey, ornamentsFlipped);
    }
    
    if (hasRolls) {
        auto rollsFlipped = phraseAssociation.rolls.flip();
        playQueue->queueRoll(phraseKey, rollsFlipped);
    }
    
    return flipped;
}

Phrase Generator::claveFromCascara() {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(cascaraKey, startTime, phraseLength)) { this->cascara(); }
    PhraseAssociation phraseAssociation = playQueue->getAssociatedPhrases(cascaraKey, startTime, phraseLength);
    Phrase cascaraPhrase = phraseAssociation.base;
    auto clavePhrase = cascaraPhrase.claveFromCascara();
    playQueue->queuePhrase(claveKey, clavePhrase);
    return clavePhrase;
}

void Generator::roll(string phraseKey,
                     Probability rollProb, // TODO: just get all this stuff from editor state instead of passing it in
                     Probability associationProb,
                     Probability rollLengthProb) {
    Position startTime = editorState->getStartTime();
    Duration phraseLength = editorState->getPhraseLength();
    if (playQueue->doesntHavePhrase(phraseKey, startTime, phraseLength)) return;
    PhraseAssociation phraseAssociation = playQueue->getAssociatedPhrases(phraseKey, startTime, phraseLength);
    Phrase phrasePhrase = phraseAssociation.base;
    Phrase rollPhrase = phrasePhrase.fillWithRolls(rollProb, associationProb, rollLengthProb);
    playQueue->queueRoll(phraseKey, rollPhrase);
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
    PhraseAssociation phraseAssociation = playQueue->getAssociatedPhrases(phraseKey, startTime, phraseLength);
    Phrase phrasePhrase = phraseAssociation.base;
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    if (possibleOrnaments.empty()) { return; }
    Phrase ornamentsPhrase = phrasePhrase.addOrnaments(possibleOrnaments, prob, breadth);
    playQueue->queueOrnamentation(phraseKey, ornamentsPhrase);
}


string Generator::rollsKey(string phraseKey) {
    return phraseKey + "Rolls";
}

string Generator::ornamentsKey(string phraseKey) {
    return phraseKey + "Ornaments";
}
