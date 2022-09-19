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
//    cascaraSequence
    auto tempSequence = Sequence(subdivision, phraseStartTime, phraseLength())
        .randomCascara();
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    cascaraSequence = tempSequence;
//    tempSeq = tempSeq
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempSequence, 1);
    return result;
}

Playable Generator::clave() {
    auto tempSequence = Sequence(subdivision, phraseStartTime, phraseLength())
        .randomClave();
    this->claveSequence = tempSequence;
    Playable result = Playable(tempSequence, 2);
    return result;
}

Playable Generator::cascaraFromClave() {
    if (claveSequence.notes.empty()) {
        this->clave();
    }
    
    auto tempSequence = claveSequence.cascaraFromClave();
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    this->cascaraSequence = tempSequence;
    Playable result = Playable(tempSequence, 1);
    return result;
}


Playable Generator::claveFromCascara() {
    auto tempSequence = cascaraSequence.claveFromCascara();
    claveSequence = tempSequence;
//    tempSeq = tempSeq
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempSequence, 2);
    return result;
}
