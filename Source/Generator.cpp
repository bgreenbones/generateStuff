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
    Playable result = Playable(tempPhrase, cascaraChannel);
    return result;
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

Playable Generator::rollCascara(Probability rollProb,
                                Probability associationProb,
                                Probability rollLengthProb) {
    updateTimeSignature();
    
    return Playable(cascaraPhrase.fillWithRolls(rollProb, associationProb, rollLengthProb), cascaraChannel);
}

Playable Generator::rollClave(Probability rollProb,
                              Probability associationProb,
                              Probability rollLengthProb) {
    updateTimeSignature();
    
    return Playable(clavePhrase.fillWithRolls(rollProb, associationProb, rollLengthProb), cascaraChannel);
}


vector<OrnamentSimple> getOrnamentVector(bool flams, bool drags, bool ruffs) {
    vector<OrnamentSimple> result;
    if (flams) { result.push_back(flam); }
    if (drags) { result.push_back(drag); }
    if (ruffs) { result.push_back(ruff); }
    return result;
}

Playable Generator::ornamentCascara(Probability prob, double breadth, bool flams, bool drags, bool ruffs) {
    updateTimeSignature();
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    return Playable(cascaraPhrase.addOrnaments(possibleOrnaments, prob, breadth), cascaraChannel);
}

Playable Generator::ornamentClave(Probability prob, double breadth, bool flams, bool drags, bool ruffs) {
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    updateTimeSignature();
    return Playable(clavePhrase.addOrnaments(possibleOrnaments, prob, breadth), cascaraChannel);
}
