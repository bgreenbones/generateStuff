//
//  Note.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#include "Note.hpp"


Note::Note(short pitch,
           short velocity,
           float startTime,
           float duration) {
    this->pitch = pitch;
    this->velocity = velocity;
    this->startTime = startTime;
    this->duration = duration;
    this->accent = false;
    this->ornamented = false;
    this->ornament = false;
}
