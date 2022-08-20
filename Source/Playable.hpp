//
//  Playable.hpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/16/22.
//
#pragma once

#ifndef Playable_hpp
#define Playable_hpp

#include <stdio.h>
#include "Sequence.hpp"
#include "Phrase.hpp"

class Playable {
public:
    Playable(Sequence &sequence, Phrase &phrase, short midiChannel) {
        this->sequence = sequence;
        this->phrase = phrase;
        this->midiChannel = midiChannel;
    };
    Sequence sequence; // actual content
    Phrase phrase; // scheduling info
    short midiChannel;
};

#endif /* Playable_hpp */
