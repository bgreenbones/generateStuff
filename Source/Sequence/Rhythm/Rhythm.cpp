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
//    for (float cursor = 0; cursor < length; cursor += pulse) {
//        bool displace = displaceCoin(resultSequence.gen);
//        if (displace) {
//            bool displaceWithDouble = doubleCoin(resultSequence.gen);
//            if (displaceWithDouble) {
//                Note extraNote = Note();
//                extraNote.startTime = cursor;
//                extraNote.duration = displacement;
//                resultSequence.addNote(extraNote);
//            }
//            cursor += displacement;
//        }
//        Note note = Note();
//        note.startTime = cursor;
//        note.duration = pulse;
//        resultSequence.addNote(note);
//    }
    
    // generate
    Note currentNote = resultSequence.notes.size() > 0 ? resultSequence.notes.back() : Note(Position(0), Duration(0));
    do {
        if (displaceCoin(resultSequence.gen)) {
            if (doubleCoin(resultSequence.gen)) {
                resultSequence = resultSequence.concat(parseMininotation("x", displacement), true, true);
            }
        }
        resultSequence = resultSequence.concat(parseMininotation("x", pulse), true, true);
        currentNote = resultSequence.notes.size() > 0 ? resultSequence.notes.back() : Note(Position(0), Duration(0));
    } while (currentNote.endTime() < length);
    
    // clean up.
    Note &lastNote = resultSequence.notes.back();
    while (lastNote.startTime >= length) {
        DBG ("added too many notes...???");
        resultSequence.notes.pop_back();
        lastNote = resultSequence.notes.back();
    }
    if (lastNote.endTime() > length) {
        Duration lastNoteLength = length - lastNote.startTime;
        lastNote.duration = lastNoteLength;
    }
    if (lastNote.endTime() != length) {
        DBG ("sequence doesn't end at right time...");
    }
    
    return resultSequence;
}
