//
//  Latin.cpp
//  generateStuff
//
//  Created by Benjamin Greenwood on 8/19/22.
//

#include <stdio.h>
#include <JuceHeader.h>
#include "Phrase.hpp"
#include "Random.h"


void applyCascaraAccents(Sequence<Note> &cascara, Duration displacement) {
    const short accentVelocity = 120; // todo: move these out somewhere else.
    const short unaccentedVelocity = 60;
    
    for (auto noteIt = cascara.begin();
         noteIt != cascara.end();
         noteIt++) {
        if (noteIt->duration == displacement) { // beginning of a double
            noteIt->velocity = unaccentedVelocity;
        } else {
            noteIt->accented = true;
            noteIt->velocity = accentVelocity;
            noteIt->ornamented = 0.5;
        }
    }
}


Phrase Phrase::randomCascara(Probability pDisplace,
                                 Probability pDouble) const {
//    float pulse = 1.0, displacement = 0.5;
//    float displacement = this->subdivision;
//    float pulse = displacement * 2;
//    float pulse = 0.5, displacement = 0.25; // todo: choose pulse and subdivisions based on tempo and time sig and stuff?

    Duration displacement(this->primarySubdivision());
    Duration pulse(displacement * 2.);
    Phrase cascara = this->pulseAndDisplace(pulse, displacement, pDisplace, pDouble);
    
    applyCascaraAccents(cascara.notes, displacement);
    
    return cascara;
}

// todo: some way of preventing 0 syncopation from happening
Phrase Phrase::randomClave() const {
//    const auto length = clave.phrasing.length(); // todo: evaluate how we're really deciding these stuffs
//    clave.rhythm = *this; // todo: evaluate how we're really deciding these stuffs
//    const auto subdivision = this->subdivision;
//    const auto subdivision = clave.subdivision;

    Phrase clave(*this);
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
    auto numNotesTemp = uniformInt(minNumNotes, maxNumNotes); // todo: parameterize, but keep random option
    if (numNotesTemp % 2 == 0) { // force odd nums for 2-3, 3-2, 3-4, 4-3, etc.
        if (numNotesTemp + 1 > maxNumNotes) {
            numNotesTemp--;
        } else if (numNotesTemp - 1 < minNumNotes) {
            numNotesTemp++;
        } else {
            if (rollDie(3) != 1) { // more likely to subtract - gives more space to rhythms
                numNotesTemp--;
            } else {
                numNotesTemp++;
            }
        }
    }
    const auto numNotes = numNotesTemp;
    
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = flipCoin();
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
    std::array<double,17> weights = {0,1,1,3,1,2,2,2,1,2,1,1,1,1,1,1,1}; // three is most likely
    for (int i = 0; i < minNoteLengthInSubdivisions; i++) { weights[i] = 0; }
    for (int i = weights.size(); i > maxNoteLengthInSubdivisions; i--) { weights[i] = 0; }
    std::discrete_distribution<int> randomNoteLengthInSubdivisions (weights.begin(), weights.end());
//    uniform_int_distribution<> randomNoteLengthInSubdivisions(minNoteLengthInSubdivisions,
//                                                maxNoteLengthInSubdivisions);
    bool constraintsBroken = false;
    do {
        constraintsBroken = false;
        clave.notes.clear();
        float notePosition = subdivision * (rollDie(maxNoteLengthInSubdivisions) - 1);
        for (int noteInd = 0; noteInd < numNotes; noteInd++) {
//            std::string notation = "X";
//            auto setNotationLength = [&notation](int notationLength) {
//                for (int i = 1; i < notationLength; i++) {
//                    notation.append(".");
//                }
//            }
//            auto noteLength = randomNoteLengthInSubdivisions(clave.gen);
//            setNotationLength(noteLength);
            Note note = Note();
            note.velocity = 100;
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
                auto randomNum = randomNoteLengthInSubdivisions(getGen());
                note.duration = randomNum * subdivision;
                notePosition += note.duration;
            }
            
            clave.addNote(note);
        }
    } while (constraintsBroken);
    
    return clave;
}


Phrase Phrase::cascaraFromClave() const {
    bool isClave = true; // todo: how do we know
    if (!isClave) {
//        throw exception();
        DBG ("not a clave. can't generate cascara from it");
    }
    Phrase cascara(*this);
    cascara.notes.clear();
    Duration subdivision = cascara.primarySubdivision();
    
    if (this->notes.size() <= 0) {
        DBG ("no notes to generate a cascara from");
        return cascara;
    }
    
    Note firstCascaraNote = this->notes.front();
    firstCascaraNote.duration = subdivision;
    cascara.addNote(firstCascaraNote);
    
    for (auto noteIt = this->notes.begin();
         noteIt < this->notes.end();
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
        
        double subdivisionsBetweenClaveNotes = timeBetweenNotes.asBeats() / subdivision.asBeats();
        
        Note lastCascaraNote = cascara.notes.back();
        Duration timeSinceLastCascaraNote = noteIt->startTime - lastCascaraNote.startTime;
        double subdivisionsSinceLastCascaraNote = timeSinceLastCascaraNote.asBeats() / subdivision.asBeats();
        
        if (subdivisionsBetweenClaveNotes == 2.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // x . x
                cascara.notes.append(".x", subdivision, PushBehavior::wrap);
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // . x x
                    cascara.notes.append(".xx", subdivision, PushBehavior::wrap);
                } else { // x . x
                    cascara.notes.append("x.x", subdivision, PushBehavior::wrap);
                }
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 3.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit. just add next note.
                auto choice = rollDie(3);
                if (choice == 1) { // x x . x
                    cascara.notes.append("x.x", subdivision, PushBehavior::wrap);
                } else if (choice == 2) { // x . x x
                    cascara.notes.append(".xx", subdivision, PushBehavior::wrap);
                } else if (choice == 3) { // x . x . misses next note!!
                    cascara.notes.append(".x.", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                // only one option: . x . x
                cascara.notes.append(".x.x", subdivision, PushBehavior::wrap);
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 4.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit.
                if (flipCoin()) { // x . x . x
                    cascara.notes.append(".x.x", subdivision, PushBehavior::wrap);
                } else { // x x . x x
                    cascara.notes.append("x.xx", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // // . x x . x
                    cascara.notes.append(".xx.x", subdivision, PushBehavior::wrap);
                } else { // . x . x x
                    cascara.notes.append(".x.xx", subdivision, PushBehavior::wrap);
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
    
//    cascara.notes.legato();
    applyCascaraAccents(cascara.notes, subdivision);
    
    return cascara;
}




    
//    This was code from cascaraToClave that was used to test the mininotation stuff.
//
//    Note lastCascaraNoteToCompare = cascaraToCompare.notes.back();
//    Duration timeSinceLastCascaraNoteToCompare = noteIt->startTime - lastCascaraNoteToCompare.startTime;
//    double subdivisionsSinceLastCascaraNoteToCompare = timeSinceLastCascaraNoteToCompare.asBeats() / subdivision.asBeats();
//
//    Note note = Note();
////        note.accent = true;
////        note.duration = 1; // todo: method to fill out durations between notes
////        note.ornamented = 0.5; // todo: don't just make accented notes ornamented.
//
//
//    if (lastCascaraNote.startTime != lastCascaraNoteToCompare.startTime) {
//        DBG ("last notes differ");
//    }
//
//    if (subdivisionsSinceLastCascaraNote != subdivisionsSinceLastCascaraNoteToCompare) {
//        DBG ("time differences differ");
//    }
//
//
//    Phrase newNotes;
//    if (subdivisionsBetweenClaveNotes == 2.) {
//        if (subdivisionsSinceLastCascaraNote == 0.) { // x . x
//            newNotes = Phrase::parseMininotation("~x", subdivision);
//            branch = 0;
//        } else if (subdivisionsSinceLastCascaraNote == 1.0) {
//            if (rand() % 2) { // . x x
//                newNotes = Phrase::parseMininotation("~xx", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + subdivision * 2.;
//                note.duration = subdivision;
//                cascaraToCompare.addNote(note);
//
//                branch = 1;
//            } else { // x . x
//                newNotes = Phrase::parseMininotation("x~x", subdivision);
//
//                note.startTime = noteIt->startTime;
//                note.duration = subdivision * 2.;
//                cascaraToCompare.addNote(note);
//
//                branch = 2;
//            }
//        } else {
//            DBG ("cascara has some weird note lengths??");
//        }
//        Note nextCascaraNote = *nextNote;
//        cascaraToCompare.addNote(nextCascaraNote);
//    } else if (subdivisionsBetweenClaveNotes == 3.) {
//        if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit. just add next note.
//            auto choice = rand() % 3;
//            if (choice == 0) { // x x . x
//                newNotes = Phrase::parseMininotation("x~x", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + subdivision;
//                cascaraToCompare.addNote(note);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 3;
//            } else if (choice == 1) { // x . x x
//                newNotes = Phrase::parseMininotation("~xx", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + pulse;
//                cascaraToCompare.addNote(note);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 4;
//            } else if (choice == 2) { // x . x . misses next note!!
//                newNotes = Phrase::parseMininotation("~x~", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + pulse;
//                note.duration = pulse;
//                cascaraToCompare.addNote(note);
//
//                branch = 5;
//            }
//        } else if (subdivisionsSinceLastCascaraNote == 1.0) {
//            // only one option: . x . x
//            newNotes = Phrase::parseMininotation("~x~x", subdivision);
//
//            note.startTime = lastCascaraNote.startTime + pulse;
//            note.duration = pulse;
//            cascaraToCompare.addNote(note);
//            Note nextCascaraNote = *nextNote;
//            cascaraToCompare.addNote(nextCascaraNote);
//
//            branch = 6;
//        } else {
//            DBG ("cascara has some weird note lengths??");
//        }
//    } else if (subdivisionsBetweenClaveNotes == 4.) {
//        if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit.
//            auto choice = rand() % 2;
//            if (choice == 0) { // x . x . x
//                newNotes = Phrase::parseMininotation("~x~x", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + pulse;
//                note.duration = pulse;
//                cascaraToCompare.addNote(note);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 7;
//            } else if (choice == 1) { // x x . x x
//                newNotes = Phrase::parseMininotation("x~xx", subdivision);
//
//                note.startTime = lastCascaraNote.startTime + subdivision;
//                note.duration = pulse;
//                cascaraToCompare.addNote(note);
//                Note anotherNote = Note();
//                anotherNote.startTime = note.startTime + pulse;
//                anotherNote.duration = subdivision;
//                cascaraToCompare.addNote(anotherNote);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 8;
//            }
//        } else if (subdivisionsSinceLastCascaraNote == 1.0) {
//            note.startTime = lastCascaraNote.startTime + pulse;
//            auto choice = rand() % 2;
//            if (choice == 0) { // // . x x . x
//                newNotes = Phrase::parseMininotation("~xx~x", subdivision);
//
//                note.duration = subdivision;
//                cascaraToCompare.addNote(note);
//                Note anotherNote = Note();
//                anotherNote.startTime = note.startTime + subdivision;
//                anotherNote.duration = pulse;
//                cascaraToCompare.addNote(anotherNote);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 9;
//            } else if (choice == 1) { // . x . x x
//                newNotes = Phrase::parseMininotation("~x~xx", subdivision);
//
//                note.duration = pulse;
//                cascaraToCompare.addNote(note);
//                Note anotherNote = Note();
//                anotherNote.startTime = note.startTime + pulse;
//                anotherNote.duration = subdivision;
//                cascaraToCompare.addNote(anotherNote);
//                Note nextCascaraNote = *nextNote;
//                cascaraToCompare.addNote(nextCascaraNote);
//
//                branch = 10;
//            }
//        } else {
//            DBG ("cascara has some weird note lengths??");
//        }
//    } else {
//        DBG ("weird amount of subdivisions between notes in clave.");
//    }
//    cascara = cascara.concat(newNotes, true, true);
//
//
//    if (cascara.notes.size() != cascaraToCompare.notes.size()) {
//        DBG ("different number of notes");
//    }
//
//    bool allNotesHaveMatch = true;
//    for (auto cascaraNote : cascara.notes) {
//        bool match = false;
//        for (auto compareNote : cascaraToCompare.notes) {
//            match = match || cascaraNote.startTime == compareNote.startTime;
//        }
//        allNotesHaveMatch = allNotesHaveMatch && match;
//    }
//
//    if (!allNotesHaveMatch) {
//        DBG ("not all notes in mininotation way have match in old way");
//    }
//
//    bool allNotesHaveMatch2 = true;
//    for (auto cascaraNote : cascaraToCompare.notes) {
//        bool match = false;
//        for (auto compareNote : cascara.notes) {
//            match = match || cascaraNote.startTime == compareNote.startTime;
//        }
//        allNotesHaveMatch2 = allNotesHaveMatch2 && match;
//    }
//
//    if (!allNotesHaveMatch2) {
//        DBG ("not all notes in old way have match in mininotationw way");
//    }
//
//    if (cascara.notes.size() != cascaraToCompare.notes.size() ||
//        !allNotesHaveMatch ||
//        !allNotesHaveMatch2) {
//        problemBranches.push_back(branch);
//    }

Phrase Phrase::claveFromCascara() const {
//    Phrase clave = Phrase(cascara.rhythm, cascara.phrasing);
    
    bool isCascara = true; // todo: how do we know
    if (!isCascara) {
        throw exception();
    }
    Phrase cascara(*this); // for readability
    Phrase clave(*this);
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
    auto numNotes = uniformInt(minNumNotes, maxNumNotes); // todo: parameterize, but keep random option
    if (numNotes % 2 == 0) { // force odd nums for 2-3, 3-2, 3-4, 4-3, etc.
        if (numNotes + 1 > maxNumNotes) {
            numNotes--;
        } else if (numNotes - 1 < minNumNotes) {
            numNotes++;
        } else {
            if (rollDie(3) == 1) { // more likely to subtract - gives more space to rhythms
                numNotes--;
            } else {
                numNotes++;
            }
        }
    }
    if (numNotes > maxNumNotes) { throw exception(); }
    
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = flipCoin();
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
        clave.notes.clear();
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
                    if (flipCoin()) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                        note.startTime = noteIt->startTime;
                        note.accented = true;
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
                    if (flipCoin()) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                        note.startTime = noteIt->startTime;
                        note.accented = true;
                        note.ornamented = 0.5;
                        notesNeededOnRight--;
                        clave.addNote(note);
                    }
                }
            }
            
//
//            if (notesNeededOnRight == 0) { // last note
//                clave.notes.back().duration = (phraseLength - note.startTime) + clave.notes.front().startTime;
//                if (clave.notes.back().duration < (double) minNoteLength || // last note is bad length
//                    clave.notes.back().duration > (double) maxNoteLength) {
//                    constraintsBroken = true;
//                    break;
//                }
//            }
        }
        
        if (notesNeededOnLeft > 0 || notesNeededOnRight > 0) {
            constraintsBroken = true;
        }
                
        if (!constraintsBroken) {
            clave.notes.legato();
            for (auto note : clave.notes.events) {
                if (note.duration < (double) minNoteLength || // bad length
                    note.duration > (double) maxNoteLength) {
                    constraintsBroken = true;
                    break;
                }
            }
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

