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
    auto tempSeq = Sequence(subdivision, phraseStartTime, phraseLength())
        .randomCascara();
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    cascaraSequence = tempSeq;
//    tempSeq = tempSeq
//        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempSeq, 1);
    return result;
}

Playable Generator::claveFromCascara() {
    auto tempSeq = cascaraSequence.claveFromCascara();
    claveSequence = tempSeq;
    tempSeq = tempSeq
        .addOrnaments({ flam, drag, ruff }, HostSettings::instance().getTempo());
    Playable result = Playable(tempSeq, 2);
    return result;
}
