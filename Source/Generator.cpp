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

bool Generator::setPhraseLengthBars(const float bars) {
    if (bars < 0 || bars + phraseLengthBeats.asBars() <= 0) {
        return false;
    }
    phraseLengthBars = bars;
    return true;
}

bool Generator::setPhraseLengthBeats(const float beats) {
    if (beats < 0 || phraseLengthBars.asBeats() + beats <= 0) {
        return false;
    }
    phraseLengthBeats = beats;
    return true;
}

Playable Generator::cascara() {
    updateTimeSignature();
    
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomCascara();
    cascaraPhrase = tempPhrase;
    Playable result = Playable(tempPhrase, cascaraChannel); // this works
    queuePlayable("cascara", result); // this works
    return result; // but after it goes out of scope, i think the phrase's sequences' parent references lose the scope and die
}

Playable Generator::clave() {
    updateTimeSignature();
    
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomClave();
    this->clavePhrase = tempPhrase;
    Playable result = Playable(tempPhrase, claveChannel);
    return result;
}

Playable Generator::cascaraFromClave() {
    updateTimeSignature();
    
    if (clavePhrase.notes.empty()) {
        this->clave();
    }
    
    auto tempPhrase = clavePhrase.cascaraFromClave();
    this->cascaraPhrase = tempPhrase;
    Playable result = Playable(tempPhrase, cascaraChannel);
    return result;
}


Playable Generator::claveFromCascara() {
    updateTimeSignature();
    
    auto tempPhrase = cascaraPhrase.claveFromCascara();
    clavePhrase = tempPhrase;
    Playable result = Playable(tempPhrase, claveChannel);
    return result;
}

void Generator::roll(string phraseKey,
                         Probability rollProb,
                         Probability associationProb,
                         Probability rollLengthProb) {
    updateTimeSignature();
    auto phrasePlayableIt = playQueue->find(phraseKey);
    if (phrasePlayableIt == playQueue->end()) {
        cout << "No phrase with that key";
        return;
    }
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
    updateTimeSignature();
    auto phrasePlayableIt = playQueue->find(phraseKey);
    if (phrasePlayableIt == playQueue->end()) {
        cout << "No phrase with that key";
        return;
    }
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
