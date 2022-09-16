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

class Playable {
public:
    Playable(Sequence sequence, short midiChannel): sequence(sequence), midiChannel(midiChannel) { };
    Sequence sequence;
    short midiChannel;
};

#endif /* Playable_hpp */
