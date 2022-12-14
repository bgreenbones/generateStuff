//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"

bool Generator::setSubdivision(const float subdivision) {
    this->subdivision = Subdivision(Beats(subdivision), phraseStartTime, phraseLength());
    return true;
}

bool Generator::setPhraseLength(const float bars, const float beats) {
    phraseLengthBars = bars;
    phraseLengthBeats = beats;
    return true;
}


bool Generator::hasPhrase(string phraseKey) {
    auto phrasePlayableIt = playQueue->find(phraseKey);
    return (phrasePlayableIt != playQueue->end());
}

Playable Generator::cascara() {
//    updateTimeSignature();
    
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomCascara();
    Playable result = Playable(tempPhrase, cascaraChannel);
    queuePlayable("cascara", result); // TODO: make these phrase type strings available to...everyone?
    return result;
}

Playable Generator::clave() {
//    updateTimeSignature();
    
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomClave();
    Playable result = Playable(tempPhrase, claveChannel);
    return result;
}

Playable Generator::cascaraFromClave() {
//    updateTimeSignature();
    if (!hasPhrase("clave")) { this->clave(); }
    auto tempPhrase = playQueue->at("clave").phrase.cascaraFromClave();
    Playable result = Playable(tempPhrase, cascaraChannel);
    return result;
}


Playable Generator::claveFromCascara() {
//    updateTimeSignature();
    if (!hasPhrase("cascara")) { this->cascara(); }
    auto tempPhrase = playQueue->at("cascara").phrase.claveFromCascara();
    Playable result = Playable(tempPhrase, claveChannel);
    return result;
}

void Generator::roll(string phraseKey,
                         Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb) {
//    updateTimeSignature();
    if (!hasPhrase(phraseKey)) return;
    Playable phrasePlayable = playQueue->at(phraseKey);
    Phrase rollPhrase = phrasePlayable.phrase.fillWithRolls(rollProb, associationProb, rollLengthProb);
    queuePlayable(rollsKey(phraseKey), Playable(rollPhrase, phrasePlayable.midiChannel));
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
//    updateTimeSignature();
    if (!hasPhrase(phraseKey)) return;
    Playable phrasePlayable = playQueue->at(phraseKey);
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    Phrase ornamentsPhrase = phrasePlayable.phrase.addOrnaments(possibleOrnaments, prob, breadth);
    queuePlayable(ornamentsKey(phraseKey), Playable(ornamentsPhrase, phrasePlayable.midiChannel));
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
    Playable playable = playQueue->at(id);
    playable.mute = !(playable.mute);
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
//    auto result = playQueue->insert(pair<string, unique_ptr<Playable>>(id, playable)); // TODO:
    if (result.second) return;
    playQueue->at(id) = playable;
}
