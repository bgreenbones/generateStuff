/*
  ==============================================================================

    Rhythm.cpp
    Created: 15 Sep 2022 9:24:27pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Phrase.hpp"

Phrase Phrase::pulseAndDisplace(Duration pulse,
                                  Duration displacement, // 1/3 to displace by a triplet, 1/4 for a sixteenth, etc.
                                  Probability pDisplace,
                                  Probability pDouble) const
{
    const Duration length(duration);
    Phrase resultPhrase(*this);
    resultPhrase.notes.clear();
    Sequence<Note> &sequence = resultPhrase.notes;
    
    bernoulli_distribution displaceCoin(pDisplace);
    auto decideToDisplace = [&]() { return displaceCoin(resultPhrase.gen); };
    bernoulli_distribution doubleCoin(pDouble);
    auto decideToDouble = [&]() { return doubleCoin(resultPhrase.gen); };
    
    
// VERSION 3:
//
    // generate
    do {
        if (decideToDisplace()) {
            if (decideToDouble()) {
                sequence.append("x", displacement);
            }
        }
        sequence.append("x", pulse);
    } while (sequence.endTime() < length);

    // clean up.
    sequence.chopAfterDuration(length);
    
    // delete this 'test' code eventually
    if (sequence.endTime() != length) {
        DBG ("phrase doesn't end at right time...");
    }
    if (!resultPhrase.notes.equals(sequence.events)) {
        DBG ("why not?");
    }
    
    return resultPhrase;
}



// VERSION 1:

//    for (float cursor = 0; cursor < length; cursor += pulse) {
//        bool displace = displaceCoin(resultPhrase.gen);
//        if (displace) {
//            bool displaceWithDouble = doubleCoin(resultPhrase.gen);
//            if (displaceWithDouble) {
//                Note extraNote = Note();
//                extraNote.startTime = cursor;
//                extraNote.duration = displacement;
//                resultPhrase.addNote(extraNote);
//            }
//            cursor += displacement;
//        }
//        Note note = Note();
//        note.startTime = cursor;
//        note.duration = pulse;
//        resultPhrase.addNote(note);
//    }


// VERSION 2:
//    // generate
//    Note currentNote = resultPhrase.notes.size() > 0 ? resultPhrase.notes.back() : Note(Position(0), Duration(0));
//    do {
//        if (displaceCoin(resultPhrase.gen)) {
//            if (doubleCoin(resultPhrase.gen)) {
//                resultPhrase = resultPhrase.concat(parseMininotation("x", displacement), true, true);
//                resultPhrase.noteSeq.concat(noteSeq.parseMininotation("x", displacement), true);
//            }
//        }
//        resultPhrase = resultPhrase.concat(parseMininotation("x", pulse), true, true);
//        resultPhrase.noteSeq.concat(noteSeq.parseMininotation("x", pulse), true);
//        currentNote = resultPhrase.notes.size() > 0 ? resultPhrase.notes.back() : Note(Position(0), Duration(0));
//    } while (currentNote.endTime() < length);
//
//    // clean up.
//    Note &lastNote = resultPhrase.notes.back();
//    Note &otherLastNote = resultPhrase.noteSeq.back();
//    while (lastNote.startTime >= length) {
//        DBG ("added too many notes...???");
//        resultPhrase.notes.pop_back();
//        lastNote = resultPhrase.notes.back();
//
//        resultPhrase.noteSeq.pop_back();
//    }
//    if (lastNote.endTime() > length) {
//        Duration lastNoteLength = length - lastNote.startTime;
//        lastNote.duration = lastNoteLength;
//    }
//    if (otherLastNote.endTime() > length) {
//        Duration lastNoteLength = length - otherLastNote.startTime;
//        otherLastNote.duration = lastNoteLength;
//    }
//    if (lastNote.endTime() != length) {
//        DBG ("phrase doesn't end at right time...");
//    }
//
//    if (!resultPhrase.noteSeq.equals(resultPhrase.notes)) {
//        DBG ("why not?");
//    }

//    return resultPhrase;
