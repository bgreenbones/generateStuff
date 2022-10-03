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
//    cascaraPhrase
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomCascara();
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    cascaraPhrase = tempPhrase;
//    tempSeq = tempSeq
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempPhrase, 1);
    return result;
}

Playable Generator::clave() {
    updateTimeSignature();
    auto tempPhrase = Phrase(subdivision, phraseStartTime, phraseLength())
        .randomClave();
    this->clavePhrase = tempPhrase;
    Playable result = Playable(tempPhrase, 2);
    return result;
}

Playable Generator::cascaraFromClave() {
    updateTimeSignature();
    if (clavePhrase.notes.empty()) {
        this->clave();
    }
    
    auto tempPhrase = clavePhrase.cascaraFromClave();
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    this->cascaraPhrase = tempPhrase;
    Playable result = Playable(tempPhrase, 1);
    return result;
}


Playable Generator::claveFromCascara() {
    updateTimeSignature();
    auto tempPhrase = cascaraPhrase.claveFromCascara();
    clavePhrase = tempPhrase;
//    tempSeq = tempSeq
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempPhrase, 2);
    return result;
}
