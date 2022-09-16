/*
  ==============================================================================

    Rhythm.cpp
    Created: 15 Sep 2022 9:24:27pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Sequence.hpp"

Sequence Sequence::pulseAndDisplace(Duration pulse,
                                  Duration displacement, // 1/3 to displace by a triplet, 1/4 for a sixteenth, etc.
                                  Probability pDisplace,
                                  Probability pDouble) const
{
    const Duration length(duration);
    Sequence resultSequence(*this);
    
    resultSequence.notes.clear();
    
    bernoulli_distribution displaceCoin(pDisplace);
    bernoulli_distribution doubleCoin(pDouble);
    for (float cursor = 0; cursor < length; cursor += pulse) {
        bool displace = displaceCoin(resultSequence.gen);
        if (displace) {
            bool displaceWithDouble = doubleCoin(resultSequence.gen);
            if (displaceWithDouble) {
                Note extraNote = Note();
                extraNote.startTime = cursor;
                extraNote.duration = displacement;
                resultSequence.addNote(extraNote);
            }
            cursor += displacement;
        }
        
        Note note = Note();
        note.startTime = cursor;
        note.duration = pulse;
        resultSequence.addNote(note);
    }
    
    return resultSequence;
}
