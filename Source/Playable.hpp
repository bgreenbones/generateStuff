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
#include "Phrase.hpp"

class Playable {
public:
    Playable(Phrase phrase, short midiChannel): phrase(phrase), midiChannel(midiChannel) { };
    Phrase phrase;
    short midiChannel;
    bool mute = false;
};

#endif /* Playable_hpp */
