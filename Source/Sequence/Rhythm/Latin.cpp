//
//  Latin.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/19/22.
//

#include <stdio.h>
#include <JuceHeader.h>
#include "Sequence.hpp"

Sequence Sequence::randomCascara(Probability pDisplace,
                                 Probability pDouble) const {
//    float pulse = 1.0, displacement = 0.5;
//    float displacement = this->subdivision;
//    float pulse = displacement * 2;
//    float pulse = 0.5, displacement = 0.25; // todo: choose pulse and subdivisions based on tempo and time sig and stuff?

    Duration displacement(this->primarySubdivision());
    Duration pulse(displacement * 2.);
    Sequence cascara = this->pulseAndDisplace(pulse, displacement, pDisplace, pDouble);
    const short accentVelocity = 120; // todo: move these out somewhere else.
    const short unaccentedVelocity = 60;
    
    for (auto noteIt = cascara.notes.begin();
         noteIt != cascara.notes.end();
         noteIt++) {
        Note note = *noteIt;
        if (note.duration == displacement) { // beginning of a double
            noteIt->velocity = unaccentedVelocity;
        } else {
            noteIt->accented = true;
            noteIt->velocity = accentVelocity;
            noteIt->ornamented = 0.5;
        }
    }
    return cascara;
}

// todo: some way of preventing 0 syncopation from happening
Sequence Sequence::randomClave() const {
//    const auto length = clave.phrasing.length(); // todo: evaluate how we're really deciding these stuffs
//    clave.rhythm = *this; // todo: evaluate how we're really deciding these stuffs
//    const auto subdivision = this->subdivision;
//    const auto subdivision = clave.subdivision;

    Sequence clave(*this);
    const double length = clave.duration;
    const auto subdivision = clave.primarySubdivision();
    
    int minNoteLengthInSubdivisions = 2; // todo: parameterizing these would be fun
    int maxNoteLengthInSubdivisions = 4;
    float minNoteLength = minNoteLengthInSubdivisions * subdivision;
    float maxNoteLength = maxNoteLengthInSubdivisions * subdivision;
    
    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    auto sideLength = length / 2.0;
    int maxNumNotes = floor(length / minNoteLength);
    int minNumNotes = ceil(length / maxNoteLength);
    const auto numNotesRange = maxNumNotes - minNumNotes;
    auto numNotesTemp = (rand() % numNotesRange) + minNumNotes; // todo: parameterize, but keep random option
    if (numNotesTemp % 2 == 0) { // force odd nums for 2-3, 3-2, 3-4, 4-3, etc.
        if (numNotesTemp + 1 > maxNumNotes) {
            numNotesTemp--;
        } else if (numNotesTemp - 1 < minNumNotes) {
            numNotesTemp++;
        } else {
            if (rand() % 3) { // more likely to subtract - gives more space to rhythms
                numNotesTemp--;
            } else {
                numNotesTemp++;
            }
        }
    }
    const auto numNotes = numNotesTemp;
    
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = rand() % 2;
    auto notesOnLeft = floor (float(numNotes) / 2.0);
    auto notesOnRight = ceil (float(numNotes) / 2.0);
    if (moreOnLeft) {
        auto temp = notesOnRight;
        notesOnRight = notesOnLeft;
        notesOnLeft = temp;
    }
    if (notesOnLeft + notesOnRight != numNotes) { throw exception(); }
    
    // choose a random arrangement  of that num notes on each side such that
    // space between any two notes (even across sides, both ways) is 2, 3, or 4
    uniform_int_distribution<> randomNoteLengthInSubdivisions(minNoteLengthInSubdivisions,
                                                maxNoteLengthInSubdivisions);
    bool constraintsBroken = false;
    do {
        constraintsBroken = false;
        clave.notes = vector<Note>();
        float notePosition = (rand() % maxNoteLengthInSubdivisions) * subdivision;
        for (int noteInd = 0; noteInd < numNotes; noteInd++) {
            Note note = Note();
            note.startTime = notePosition;
            // note doesn't fall within its side, scrap it and try again
            if ((noteInd < notesOnLeft && note.startTime >= sideLength) ||
                (noteInd >= notesOnLeft && (note.startTime < sideLength || note.startTime >= (double) length))) {
                constraintsBroken = true;
                break;
            }
            
            if (noteInd == numNotes - 1) { // last note
                note.duration = (length - note.startTime) + clave.notes.front().startTime;
                if (note.duration < (double) minNoteLength || // last note is bad length
                    note.duration > (double) maxNoteLength) {
                    constraintsBroken = true;
                    break;
                }
            } else {
                note.duration = randomNoteLengthInSubdivisions(clave.gen) * subdivision;
                notePosition += note.duration;
            }
            
            clave.addNote(note);
        }
    } while (constraintsBroken);
    
    return clave;
}


Sequence Sequence::cascaraFromClave() const {
//    Sequence cascara = Sequence(clave.rhythm, clave.phrasing);
    
    bool isClave = true; // todo: how do we know
    if (!isClave) {
//        throw exception();
        DBG ("not a clave. can't generate cascara from it");
    }
    Sequence cascara(*this);
    cascara.notes.clear();
    Duration subdivision = cascara.primarySubdivision();
    Duration pulse = subdivision * 2.;
    
    if (this->notes.size() <= 0) {
        DBG ("no notes to generate a cascara from");
        return cascara;
    }
    
    cascara.addNote(this->notes.front());
    for (auto noteIt = this->notes.begin();
         noteIt != --this->notes.end();
         noteIt++)
    {
        auto nextNote = noteIt + 1;
        
        Duration timeBetweenNotes;
        if (nextNote == this->notes.end()) {
            nextNote = this->notes.begin();
            timeBetweenNotes = nextNote->startTime + this->duration - noteIt->startTime;
        } else {
            timeBetweenNotes = nextNote->startTime - noteIt->startTime;
        }
        
        double subdivisionsBetweenNotes = timeBetweenNotes.asBeats() / subdivision.asBeats();
        
        Note lastCascaraNote = cascara.notes.back();
        Duration timeSinceLastCascaraNote = lastCascaraNote.startTime - noteIt->startTime;
        double subdivisionsSinceCascaraNote = timeSinceLastCascaraNote.asBeats() / subdivision.asBeats();
        
        Note note = Note();
//        note.accent = true;
//        note.duration = 1; // todo: method to fill out durations between notes
//        note.ornamented = 0.5; // todo: don't just make accented notes ornamented.
        
        
        if (subdivisionsBetweenNotes == 2.) {
            if (subdivisionsSinceCascaraNote == 0.) { // x . x
            } else if (subdivisionsSinceCascaraNote == 1.0) {
                if (rand() % 2) { // . x x
                    note.startTime = lastCascaraNote.startTime + subdivision * 2.;
                    note.duration = subdivision;
                    
                    auto cascaraCopy = cascara;
                    cascaraCopy.concat(Sequence::parseMininotation(".xx", subdivision));
                    // todo: compare results of this against the other way.
                    // hopefully they are equivalent.
                } else { // x . x
                    note.startTime = noteIt->startTime;
                    note.duration = subdivision * 2.;
                }
                cascara.addNote(note);
            } else {
                DBG ("cascara has some weird note lengths??");
            }
            Note nextCascaraNote = *nextNote;
            cascara.addNote(nextCascaraNote);
        } else if (subdivisionsBetweenNotes == 3.) {
            if (subdivisionsSinceCascaraNote == 0.) { // this note already hit. just add next note.
                auto choice = rand() % 3;
                if (choice == 0) { // x x . x
                    note.startTime = lastCascaraNote.startTime + subdivision;
                    cascara.addNote(note);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                } else if (choice == 1) { // x . x x
                    note.startTime = lastCascaraNote.startTime + pulse;
                    cascara.addNote(note);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                } else if (choice == 2) { // x . x . misses next note!!
                    note.startTime = lastCascaraNote.startTime + pulse;
                    note.duration = pulse;
                    cascara.addNote(note);
                }
            } else if (subdivisionsSinceCascaraNote == 1.0) {
                // only one option: . x . x
                note.startTime = lastCascaraNote.startTime + pulse;
                note.duration = pulse;
                cascara.addNote(note);
                Note nextCascaraNote = *nextNote;
                cascara.addNote(nextCascaraNote);
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenNotes == 4.) {
            if (subdivisionsSinceCascaraNote == 0.) { // this note already hit.
                auto choice = rand() % 2;
                if (choice == 0) { // x . x . x
                    note.startTime = lastCascaraNote.startTime + pulse;
                    note.duration = pulse;
                    cascara.addNote(note);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                } else if (choice == 1) { // x x . x x
                    note.startTime = lastCascaraNote.startTime + subdivision;
                    note.duration = pulse;
                    cascara.addNote(note);
                    Note anotherNote = Note();
                    anotherNote.startTime = note.startTime + pulse;
                    anotherNote.duration = subdivision;
                    cascara.addNote(anotherNote);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                }
            } else if (subdivisionsSinceCascaraNote == 1.0) {
                note.startTime = lastCascaraNote.startTime + pulse;
                auto choice = rand() % 2;
                if (choice == 0) { // // . x x . x
                    note.duration = subdivision;
                    cascara.addNote(note);
                    Note anotherNote = Note();
                    anotherNote.startTime = note.startTime + subdivision;
                    anotherNote.duration = pulse;
                    cascara.addNote(anotherNote);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                } else if (choice == 1) { // . x . x x
                    note.duration = pulse;
                    cascara.addNote(note);
                    Note anotherNote = Note();
                    anotherNote.startTime = note.startTime + pulse;
                    anotherNote.duration = subdivision;
                    cascara.addNote(anotherNote);
                    Note nextCascaraNote = *nextNote;
                    cascara.addNote(nextCascaraNote);
                }
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else {
            DBG ("weird amount of subdivisions between notes in clave.");
        }
    }

    // maybe 2/3 - 3/4 of cascara accents end up being clave notes - the rest fall right next to clave notes. actually acheive these ratios - don't just use them as probabilities that you don't enforce.
    
    // connect length 4 with multiple length 2 or two doubles in a row.
    
    // length 3 can connect with a double on other side but you can also avoid hitting with
    // the clave when there are notes of length 3. maybe enforce these as ratios:
    // connecting 3/4s of the time, avoiding hitting together 1/4th

    return cascara;
}

Sequence Sequence::claveFromCascara() const {
//    Sequence clave = Sequence(cascara.rhythm, cascara.phrasing);
    
    bool isCascara = true; // todo: how do we know
    if (!isCascara) {
        throw exception();
    }
    Sequence cascara(*this); // for readability
    Sequence clave(*this);
    clave.notes.clear();
//    const auto subdivision = clave.rhythm.subdivision;
    const auto subdivision = clave.primarySubdivision();
//    const auto phraseLength = clave.phrasing.length();
    const auto phraseLength = clave.duration;
    
    // NEW APPROACH:
    // calculate notes on left and right like other clave method.
    // chose that many from cascara, maybe shifting a little
    int minNoteLengthInSubdivisions = 2; // todo: parameterizing these would be fun
    int maxNoteLengthInSubdivisions = 4;
    float minNoteLength = minNoteLengthInSubdivisions * subdivision;
    float maxNoteLength = maxNoteLengthInSubdivisions * subdivision;
    
    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    auto sideLength = phraseLength / 2.0;
    int maxNumNotes = min<float>(phraseLength, cascara.notes.size()); // diff from other clave method
    int minNumNotes = ceil(phraseLength / maxNoteLength);
    auto numNotesRange = maxNumNotes - minNumNotes;
    if (numNotesRange <= 0) { throw exception(); }
    auto numNotes = (rand() % numNotesRange) + minNumNotes; // todo: parameterize, but keep random option
    if (numNotes % 2 == 0) { // force odd nums for 2-3, 3-2, 3-4, 4-3, etc.
        if (numNotes + 1 > maxNumNotes) {
            numNotes--;
        } else if (numNotes - 1 < minNumNotes) {
            numNotes++;
        } else {
            if (rand() % 3) { // more likely to subtract - gives more space to rhythms
                numNotes--;
            } else {
                numNotes++;
            }
        }
    }
    if (numNotes > maxNumNotes) { throw exception(); }
    
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = rand() % 2;
    auto notesOnLeft = floor (float(numNotes) / 2.0);
    auto notesOnRight = ceil (float(numNotes) / 2.0);
    if (moreOnLeft) {
        auto temp = notesOnRight;
        notesOnRight = notesOnLeft;
        notesOnLeft = temp;
    }
    if (notesOnLeft + notesOnRight != numNotes) { throw exception(); }
    // todo: check if there are enough notes on each side for the assigned number of notes
    // notesOnLeft <= actual # notes on left of cascara, etc.
    
    int attempts = 0;
    bool constraintsBroken = false;
    do {
        attempts++;
        constraintsBroken = false;
        clave.notes = vector<Note>();
        short notesNeededOnLeft = notesOnLeft;
        short notesNeededOnRight = notesOnRight;
        if (cascara.notes.size() < notesNeededOnLeft + notesNeededOnRight) { throw exception(); }
        for (auto noteIt = cascara.notes.begin();
             noteIt != cascara.notes.end();
             noteIt++)
        {
            // todo:
            // make us land inbetween cascara notes sometimes. probably if there's a double coming up or something.
            
            bool isNoteOnLeft = noteIt->startTime < sideLength;
            Note note = Note();
            if (notesNeededOnLeft > 0) {
                if (isNoteOnLeft) {
                    if (rand() % 2) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                        note.startTime = noteIt->startTime;
                        note.accented = true;
                        note.duration = 1; // todo: method to fill out durations between notes
                        note.ornamented = 0.5; // todo: don't just make accented notes ornamented.
                        notesNeededOnLeft--;
                        clave.addNote(note);
                    }
                } else {
                    constraintsBroken = true;
                    break;
                }
            }
            
            if (notesNeededOnRight > 0) {
                if (!isNoteOnLeft) {
                    if (rand() % 2) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                        note.startTime = noteIt->startTime;
                        note.accented = true;
                        note.duration = 1; // todo: method to fill out durations between notes
                        note.ornamented = 0.5;
                        notesNeededOnRight--;
                        clave.addNote(note);
                    }
                }
            }
            
            // todo: check note length of other notes too
            
            if (notesNeededOnRight == 0) { // last note
                clave.notes.back().duration = (phraseLength - note.startTime) + clave.notes.front().startTime;
                if (clave.notes.back().duration < (double) minNoteLength || // last note is bad length
                    clave.notes.back().duration > (double) maxNoteLength) {
                    constraintsBroken = true;
                    break;
                }
            }
        }
        
        if (notesNeededOnLeft > 0 || notesNeededOnRight > 0) {
            constraintsBroken = true;
        }
        
        if (attempts > 2000) {
            // give up
            constraintsBroken = false;
            break;
        }
    } while (constraintsBroken);
    
    return clave;

    
    
    
    
    
    
    
    
    
    // remove all doubles -- choose later note with higher probability
//    vector<float> noteTimes {};
//    vector<float> doubleTimes {};
//    vector<float> accentTimes {};
//    for (auto noteIt = cascara.mNotes.begin();
//         noteIt != cascara.mNotes.end();
//         noteIt++)
//    {
//        noteTimes.push_back(noteIt->mTime);
//        if (noteIt->mDuration == clave.mSubdivision) {
//            doubleTimes.push_back(noteIt->mTime);
//        }
//        if (noteIt->mAccent) {
//            accentTimes.push_back(noteIt->mTime);
//        }
//    }
//
//    function<float(float, float)> dosomething = [&](float noteTime1, float noteTime2) -> float {
//        return 0;
//    };
    
    
//    auto prevNote = prev(noteIt);
//    auto nextNote = next(noteIt);
//
        // omg base it on accents too but even there there's an element of randomness
        // maybe 2/3 - 3/4 of cascara accents end up being clave notes - the rest fall right next to clave notes. actually acheive these ratios - don't just use them as probabilities that you don't enforce.
    // if two doubles in a row (xx.xx) maybe choose first x of first double and second x of second double, so it's a note of length 4 subdivs
    
    // string notes of length 2 subdivs? probably remove the middle one so it's a note of length 4
    // careful cos there could be like 5 ina row really
    
//    return clave;
}

