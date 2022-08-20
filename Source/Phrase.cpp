//
//  Phrase.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 7/29/22.
//

#include "Phrase.hpp"

Phrase Phrase::defaultPhrase = Phrase();

Phrase::Phrase(float offset, short bar, float length) {
    this->offset = offset;
    this->bar = bar;
    this->length = length; // todo: derive length from time sig, or at least make an arg
}

Phrase& Phrase::operator=(Phrase other) {
    swap(offset, other.offset);
    swap(bar, other.bar);
    swap(length, other.length);
    return *this;
};


double Phrase::ppqUntilBarStart(double ppqPosition) {
    return (double) bar - ppqPosition;
}
