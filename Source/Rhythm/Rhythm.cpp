//
//  Rhythm.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/4/22.
//

#include "Rhythm.hpp"
#include <algorithm>
#include <random>

Rhythm Rhythm::defaultRhythm = Rhythm();

Rhythm::Rhythm(float subdivision) { // todo: this is really just rhythmic functions over the phrase class...so maybe shouldn't be a basically duplicated class
    gen = mt19937(rd());
    
    this->subdivision = subdivision; // 8th notes by default rn - todo: derive based on tempo and context
}

Rhythm::Rhythm(Rhythm const& r) {
    gen = mt19937(rd());
    
    subdivision = r.subdivision;
};
Rhythm& Rhythm::operator=(Rhythm other) {
    swap(subdivision, other.subdivision);
    return *this;
};

Sequence Rhythm::pulseAndDisplace(Sequence sequence,
                                  float pulse,
                                  float displacement, // 1/3 to displace by a triplet, 1/4 for a sixteenth, etc.
                                  float pDisplace,
                                  float pDouble)
{
    Sequence rhythm = Sequence(); // todo: we need to get phrasings that we want into here!
    const auto length = rhythm.phrasing.length();
    
    bernoulli_distribution displaceCoin(pDisplace);
    bernoulli_distribution doubleCoin(pDouble);
    for (float cursor = 0; cursor < length; cursor += pulse) {
        bool displace = displaceCoin(gen);
        if (displace) {
            bool displaceWithDouble = doubleCoin(gen);
            if (displaceWithDouble) {
                Note extraNote = Note();
                extraNote.startTime = cursor;
                extraNote.duration = displacement;
                rhythm.notes.push_back(extraNote);
            }
            cursor += displacement;
        }
        
        Note note = Note();
        note.startTime = cursor;
        note.duration = pulse;
        rhythm.notes.push_back(note);
    }
    
    return rhythm;
}

