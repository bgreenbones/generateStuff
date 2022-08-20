//
//  Sequence.hpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#pragma once

#ifndef Sequence_hpp
#define Sequence_hpp

#include <vector>
#include "Note.hpp"
#include "Phrase.hpp"

class Rhythm;
using namespace std;

class Sequence
{
public:
    Sequence();
    Sequence(Rhythm &rhythm, Phrase &phrasing);
    Sequence& operator=(Sequence other);
    
    vector<Note> notes; // todo: make private and access through methods that keep it sorted?
    Rhythm &rhythm;
    Phrase &phrasing;
    bool addNote(Note toAdd);
};

#endif /* Sequence_hpp */
