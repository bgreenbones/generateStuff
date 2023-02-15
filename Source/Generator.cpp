//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"
#include "Pitch.h"

bool Generator::hasPhrase(string phraseKey) {
    auto phrasePlayableIt = playQueue->find(phraseKey);
    return (phrasePlayableIt != playQueue->end());
}

Playable Generator::cascara() {
    auto phrase = Phrase(editorState->getSubdivision(),
                         editorState->getStartTime(),
                         editorState->getPhraseLength()).randomCascara();
    Playable result = Playable(phrase);
    queuePlayable("cascara", result);
    return result;
}

Playable Generator::chords() {
    vector<Pitch> chordToAdd = randomChord();
    Phrase phrase = Phrase(editorState->getSubdivision(),
                           editorState->getStartTime(),
                           editorState->getPhraseLength());
    phrase.notes.monophonic = false;
    for (Pitch pitchToAdd : chordToAdd) {
        Note noteToAdd(pitchToAdd.pitchValue, 70, 0, (quarters)(editorState->getPhraseLength()));
        phrase.addNote(noteToAdd);
    }
    Playable result = Playable(phrase);
    queuePlayable("chords", result);
    return result;
}

Playable Generator::clave() {
    auto phrase = Phrase(editorState->getSubdivision(),
                         editorState->getStartTime(),
                         editorState->getPhraseLength()).randomClave();
    Playable result = Playable(phrase);
    queuePlayable("clave", result);
    return result;
}

Playable Generator::cascaraFromClave() {
    if (!hasPhrase("clave")) { this->clave(); }
    auto tempPhrase = playQueue->at("clave").phrase.cascaraFromClave();
    Playable result = Playable(tempPhrase);
    queuePlayable("cascara", result);
    return result;
}

Playable Generator::flipClave(string phraseKey) {
    if (!hasPhrase(phraseKey)) { return Playable(Phrase()); } // TODO: use std::optional in failure cases.
    auto flipped = playQueue->at(phraseKey).phrase.flip();
    Playable result = Playable(flipped);
    queuePlayable(phraseKey, result);
    return result;
}

Playable Generator::claveFromCascara() {
    if (!hasPhrase("cascara")) { this->cascara(); }
    auto tempPhrase = playQueue->at("cascara").phrase.claveFromCascara();
    Playable result = Playable(tempPhrase);
    queuePlayable("clave", result);
    return result;
}

void Generator::roll(string phraseKey,
                     Probability rollProb, // TODO: just get all this stuff from editor state instead of passing it in
                     Probability associationProb,
                     Probability rollLengthProb) {
    if (!hasPhrase(phraseKey)) return;
    Playable phrasePlayable = playQueue->at(phraseKey);
    Phrase rollPhrase = phrasePlayable.phrase.fillWithRolls(rollProb, associationProb, rollLengthProb);
    queuePlayable(rollsKey(phraseKey), Playable(rollPhrase));
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
    if (!hasPhrase(phraseKey)) return;
    Playable phrasePlayable = playQueue->at(phraseKey);
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    if (possibleOrnaments.empty()) { return; }
    Phrase ornamentsPhrase = phrasePlayable.phrase.addOrnaments(possibleOrnaments, prob, breadth);
    queuePlayable(ornamentsKey(phraseKey), Playable(ornamentsPhrase));
}


string Generator::rollsKey(string phraseKey) {
    return phraseKey + "Rolls";
}

string Generator::ornamentsKey(string phraseKey) {
    return phraseKey + "Ornaments";
}

void Generator::removePlayable(string id) {
    playQueue->erase(id);
}

void Generator::toggleMutePlayable(string id) {
    if (playQueue->find(id) == playQueue->end()) {
        return;
    };
    Playable &playable = playQueue->at(id);
    playQueue->at(id).mute = !(playable.mute);
}


void Generator::queuePlayable(string id, Playable playable) {
    //    for (auto it = playQueue.begin(); it < playQueue.end();) {
    //        if ((*it).midiChannel == playable.midiChannel) {
    //            playQueue.erase(it);
    //        } else {
    //            ++it;
    //        }
    //    }
    auto result = playQueue->emplace(id, playable);
    if (result.second) return;
    playQueue->at(id) = playable;
}


void Generator::setMidiChannel(string voiceName, int newMidiChannel) {
    if (midiChannels.find(voiceName) == midiChannels.end()) {
        return;
    }
    midiChannels[voiceName] = newMidiChannel;
}

int Generator::getMidiChannel(string voiceName) {
    if (midiChannels.find(voiceName) == midiChannels.end()) {
        return -1;
    }
    return midiChannels[voiceName];
}


