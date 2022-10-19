//
//  Generator.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Generator.hpp"
#include "Misc.h"


bool Generator::setSubdivision(const float subdivision) {
    this->subdivision = Subdivision(Beats(subdivision), phraseStartTime, phraseLength());
    return true;
}

bool Generator::setPhraseLengthBars(const float bars) {
    if (bars < 0) {
        throw exception();
    }
    phraseLengthBars = bars;
    return true;
}

bool Generator::setPhraseLengthBeats(const float beats) {
    if (beats < 0) {
        throw exception();
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

Playable Generator::rollCascara() {
    updateTimeSignature();
    return Playable(cascaraPhrase.fillWithRolls(), cascaraChannel);
}

Playable Generator::rollClave() {
    updateTimeSignature();
    return Playable(clavePhrase.fillWithRolls(), cascaraChannel);
}


vector<OrnamentSimple> getOrnamentVector(bool flams, bool drags, bool ruffs) {
    vector<OrnamentSimple> result;
    if (flams) { result.push_back(flam); }
    if (drags) { result.push_back(drag); }
    if (ruffs) { result.push_back(ruff); }
    return result;
}

Playable Generator::ornamentCascara(bool flams, bool drags, bool ruffs) {
    updateTimeSignature();
    return Playable(cascaraPhrase.addOrnaments(getOrnamentVector(flams, drags, ruffs)), cascaraChannel);
}

Playable Generator::ornamentClave(bool flams, bool drags, bool ruffs) {
    updateTimeSignature();
    return Playable(clavePhrase.addOrnaments(getOrnamentVector(flams, drags, ruffs)), cascaraChannel);
}
