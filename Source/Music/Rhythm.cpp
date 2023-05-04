/*
  ==============================================================================

    Rhythm.cpp
    Created: 7 Mar 2023 10:09:04pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Rhythm.h"


double rhythm::beatWiseStability(Position position) {
    Beats startingPoint = floor(position.asBeats());
    Beats positionInBeat = position - startingPoint;
    if (positionInBeat == Beats(0.)) { positionInBeat += Beats(1); }
    
    bool inFirstHalf = positionInBeat <= Beats(0.5);
    Beats halfWisePosition = inFirstHalf ? positionInBeat : positionInBeat - Beats(0.5);
    double halfWiseStability = halfWisePosition / Beats(0.5);

    // TODO: we need to use this to get -- lower stability scores in first half of beat, higher stability scores in second half of beat (highest of those is on the half way marker though) and highest scores are ON the beat
    double secondHalfStablenessBoost = 0.4;
    double stability = inFirstHalf ? halfWiseStability : halfWiseStability + secondHalfStablenessBoost; // higher stabilities for second half
    stability /= (1. + secondHalfStablenessBoost); // normalize between 0 and 1
    
    return stability;
}


Phrase rhythm::stabilityBased(Phrase fromPhrase, Probability filter)
{
    fromPhrase.notes.clear();
    Position cursor = fromPhrase.startTime;
    while (cursor < fromPhrase.endTime()) {
        Subdivision subdiv = fromPhrase.subdivisions.drawByPosition(cursor);
        
        if(Probability(rhythm::beatWiseStability(cursor)) && filter) {
            fromPhrase.addNote(Note(cursor, subdiv));
        }
        
        cursor += subdiv;
    }
    
    return fromPhrase;
}


Phrase rhythm::stabilityFilter(Phrase fromPhrase, Direction direction) {
    // TODO: maybe parameterize number of notes to move, or likelihood of any note moving
    // TODO: think about what part of the phrase to move a note...based on idea of moving from tension to resolution or vice versa
    // TODO: make a cool thing that moves around startTimes in a chord so we get a sweep effect.
    bool searchingForMovableNote = true;
    
    Sequence<Note> monoNotesCopy = fromPhrase.notes.toMonophonic();
    while (searchingForMovableNote && !monoNotesCopy.empty()) {
        shuffle(monoNotesCopy.begin(), monoNotesCopy.end(), getGen());
        Note note = monoNotesCopy.back(); // sample without replacement.
        monoNotesCopy.pop_back();
        double noteStability = rhythm::beatWiseStability(note.startTime);

        Position next = fromPhrase.nextSubdivisionPosition(note.startTime);
        double nextStability = rhythm::beatWiseStability(next);
        Position previous = fromPhrase.previousSubdivisionPosition(note.startTime);
        double previousStability = rhythm::beatWiseStability(previous);
        bool noNotesAfter = fromPhrase.notes.byStartPosition(next).empty();
        bool noNotesBefore = fromPhrase.notes.byStartPosition(previous).empty();
        
        bool canMoveForward = noNotesAfter && direction * nextStability > direction * noteStability;
        bool canMoveBackward = noNotesBefore && direction * previousStability > direction * noteStability;
        
        if (!(canMoveForward || canMoveBackward)) { continue; }
        searchingForMovableNote = false;
        bool moveForward = canMoveForward;
        if (canMoveForward && canMoveBackward) {
            bool moveForward = flipCoin();
        }
        
        Position newStartTime = moveForward ? next : previous;
        Position nextNoteStartTime = monoNotesCopy.nextStartTime(newStartTime);
        Duration newDuration = min(nextNoteStartTime - newStartTime, note.duration);
        
        for(Note &otherNote : fromPhrase.notes) {
            if (otherNote.startTime == note.startTime) {
                otherNote.startTime = newStartTime;
                otherNote.duration = newDuration;
            }
        }
    }
    return fromPhrase;
}



Phrase rhythm::burst(Phrase fromPhrase, Note note, int minimumRepeats, int maximumRepeats, float noteLengthInSubdivisions) {
    vector<Subdivision> subdivs = fromPhrase.subdivisions.byPosition(note.startTime);
    Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
    Duration noteLength = noteLengthInSubdivisions * subdiv;
    int numberOfPossibleBurstLengths = maximumRepeats - minimumRepeats;
    int numberOfNotes = rollDie(numberOfPossibleBurstLengths);
    
    for (double repeat = 0; repeat < numberOfNotes; repeat++) {
        Position position = note.startTime + repeat * noteLength;
        if (fromPhrase.notes.byStartPosition(position).empty()) {
            Note repeatNote(note.pitch, note.velocity, position, noteLength);
            fromPhrase.notes.add(repeatNote, PushBehavior::ignore, OverwriteBehavior::cutoff);
        }
    }
    
    return fromPhrase;
}

Phrase rhythm::flip(Phrase fromPhrase) {
    fromPhrase.notes.flip();
    fromPhrase.ornamentationNotes.flip();
    fromPhrase.connectingNotes.flip();
    fromPhrase.subdivisions.flip();
    return fromPhrase;
}

Phrase rhythm::pulseAndDisplace(Phrase fromPhrase,
                                Duration pulse,
                                Duration displacement, // 1/3 to displace by a triplet, 1/4 for a sixteenth, etc.
                                Probability pDisplace,
                                Probability pDouble)
{
    fromPhrase.notes = fromPhrase.notes.pulseAndDisplace(pulse, displacement, pDisplace, pDouble, fromPhrase.duration);
    return fromPhrase;
}



Probability ornamentProbabilityClave = 0.75;
int clavePitch = 60;
DynamicLevel claveVelocity = ff;

Note claveNote(Position startTime, Duration duration = 1) {
    Note newClaveNote = Note(clavePitch, claveVelocity, startTime, duration).accent();
    newClaveNote.ornamented = ornamentProbabilityClave;
    return newClaveNote;
}

void applyCascaraAccents(Sequence<Note> &cascara, Duration displacement) { // TODO: should maybe take an accompanying clave to determine where accents are? or should just note the accents on the cascara itself when we generate...
    
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


Phrase rhythm::fillCascara(Phrase fromPhrase) {
    Phrase cascara(fromPhrase);
    Duration subdivision = cascara.primarySubdivision();
    Phrase filled(cascara);
    for (auto noteIt = cascara.notes.begin();
         noteIt < cascara.notes.end();
         noteIt++)
    {
        auto nextNote = next<Note>(cascara.notes, noteIt);
        Duration timeBetweenNotes = timeBetween<Note>(*noteIt, *nextNote, cascara);
        
        Position spaceStartTime = noteIt->startTime;
        if (timeBetweenNotes > (2 * subdivision)) {
            filled.notes.insertSequence(filled.notes.pulseAndDisplace(2 * subdivision, subdivision, 0.75, 0.5, timeBetweenNotes - subdivision),
                                         spaceStartTime + subdivision);
        }
    }
            
    return filled;
};


int rhythm::getPotentialClaveNoteCount(Phrase fromPhrase, Duration minNoteLength, Duration maxNoteLength) {
    cout << "get potential clave count" << std::endl;
    Phrase clave(fromPhrase);
    Duration subdivision = clave.primarySubdivision();
    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    int maxNumNotes = floor(clave.duration / minNoteLength) - 1;
    int minNumNotes = ceil(clave.duration / maxNoteLength) + 1;
    auto numNotesRange = maxNumNotes - minNumNotes;
    if (numNotesRange < 0) { throw exception(); }
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
    return numNotes;
}

int rhythm::chooseNumberOfNotesOnLeft(double numNotes) {
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = flipCoin();
    double notesOnLeft = moreOnLeft ? ceil (numNotes / 2.) : floor (numNotes / 2.);
    return notesOnLeft;
}

Phrase rhythm::fillClave(Phrase fromPhrase,
                         int notesNeededOnLeft,
                         int notesNeededOnRight,
                         Duration minNoteLength,
                         Duration maxNoteLength) {
    Phrase clave(fromPhrase);
    Duration subdivision = clave.primarySubdivision();
    
    Phrase filledClave(clave);
    if (notesNeededOnLeft > 0 || notesNeededOnRight > 0) {
        for (auto noteIt = clave.notes.begin();
             noteIt < clave.notes.end();
             noteIt++)
        {
            auto nextNote = next<Note>(clave.notes, noteIt);
            Duration timeBetweenNotes = timeBetween<Note>(*noteIt, *nextNote, filledClave);
            
            if (timeBetweenNotes <= maxNoteLength) { continue; }
            
            int iters = 0;
            Note currentNote = *noteIt;
            while (timeBetweenNotes > maxNoteLength && (notesNeededOnLeft > 0 || notesNeededOnRight > 0))
            {
                Duration wrapAround = (nextNote->startTime < currentNote.startTime) ? clave.duration : Duration(0.);
                Position earliestNoteTime = currentNote.startTime + minNoteLength; // need to wrap around phrase bounds
                Position latestNoteTime = nextNote->startTime + wrapAround - minNoteLength;
                // TODO: some validation on possible note times?g
                int numberOfPossibleNoteTimes = ((latestNoteTime - earliestNoteTime).asQuarters() / subdivision.asQuarters()) + 1;
                Position chosenNoteTime = earliestNoteTime + (rollDie(numberOfPossibleNoteTimes) - 1) * subdivision;
                chosenNoteTime = (chosenNoteTime > clave.duration) ? chosenNoteTime - clave.duration : chosenNoteTime;
                Note newNote = claveNote(chosenNoteTime, nextNote->startTime - chosenNoteTime);
                
                bool newNoteIsOnLeft = clave.isNoteOnLeft(newNote);
                bool newNoteIsOnRight = !newNoteIsOnLeft;
                if (newNoteIsOnLeft && notesNeededOnLeft > 0) {
                    notesNeededOnLeft--;
                    filledClave.notes.add(newNote, PushBehavior::wrap, OverwriteBehavior::cutoff);
                } else if (newNoteIsOnRight && notesNeededOnRight > 0) {
                    notesNeededOnRight--;
                    filledClave.notes.add(newNote, PushBehavior::wrap, OverwriteBehavior::cutoff);
                }
                timeBetweenNotes = timeBetween<Note>(newNote, *nextNote, filledClave);
                currentNote = newNote;
                
                if (timeBetweenNotes <= maxNoteLength) { break; }
                if (++iters > 100) {
                    DBG ("we've tried too many times, something's wrong");
                    filledClave.clear();
                    break;
                }
            }
        }
    }
    return filledClave;
}

Phrase rhythm::randomCascara(Phrase fromPhrase,
                             Probability pDisplace,
                             Probability pDouble) {
//    float pulse = 1.0, displacement = 0.5;
//    float displacement = fromPhrase.subdivision;
//    float pulse = displacement * 2;
//    float pulse = 0.5, displacement = 0.25; // todo: choose pulse and subdivisions based on tempo and time sig and stuff?

    Duration displacement(fromPhrase.primarySubdivision());
    Duration pulse(displacement * 2.);
    Phrase cascara = pulseAndDisplace(fromPhrase, pulse, displacement, pDisplace, pDouble);
    
    applyCascaraAccents(cascara.notes, displacement);
    
    return cascara;
}

Phrase rhythm::cascaraFrom(Phrase fromPhrase) {
    Phrase clave(fromPhrase);
    if (clave.notes.size() <= 0) {
        DBG ("no notes to generate a cascara from");
        return randomCascara(fromPhrase);
    }

    if (clave.notes.isPolyphonic()) { clave.notes = clave.notes.toMonophonic(); }
    Phrase cascara(fromPhrase);
    Duration subdivision = cascara.primarySubdivision();
    cascara.notes.clear();
    cascara.addNote(clave.notes.front()
                    .withAccent()
                    .withDuration(subdivision));

    for (auto noteIt = clave.notes.begin();
         noteIt < clave.notes.end();
         noteIt++)
    {
        auto nextNote = next<Note>(clave.notes, noteIt);
        Duration timeBetweenNotes = timeBetween<Note>(*noteIt, *nextNote, clave);
        double subdivisionsBetweenClaveNotes = timeBetweenNotes.asBeats() / subdivision.asBeats();
        Note& lastCascaraNote = cascara.notes.back();
        Duration timeSinceLastCascaraNoteStart = noteIt->startTime - lastCascaraNote.startTime;
        double subdivisionsSinceLastCascaraNote = timeSinceLastCascaraNoteStart.asBeats() / subdivision.asBeats();
        
        if (subdivisionsBetweenClaveNotes == 2.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // x . x
                cascara.notes.append(".X", subdivision, PushBehavior::wrap);
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // . x x
                    cascara.notes.append(".xX", subdivision, PushBehavior::wrap);
                } else { // x . x
                    cascara.notes.append("x.X", subdivision, PushBehavior::wrap);
                }
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 3.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit. just add next note.
                auto choice = rollDie(3);
                if (choice == 1) { // x x . x
                    cascara.notes.append("x.X", subdivision, PushBehavior::wrap);
                } else if (choice == 2) { // x . x x
                    cascara.notes.append(".xX", subdivision, PushBehavior::wrap);
                } else if (choice == 3) { // x . x . misses next note!!
                    cascara.notes.append(".x.", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                // only one option: . x . x
                cascara.notes.append(".x.X", subdivision, PushBehavior::wrap);
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 4.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit.
                if (flipCoin()) { // x . x . x
                    cascara.notes.append(".x.X", subdivision, PushBehavior::wrap);
                } else { // x x . x x
                    cascara.notes.append("x.xX", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // // . x x . x
                    cascara.notes.append(".xx.X", subdivision, PushBehavior::wrap);
                } else { // . x . x x
                    cascara.notes.append(".x.xX", subdivision, PushBehavior::wrap);
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
    
    cascara = fillCascara(cascara);
    dynamics::stretch(cascara.notes, unaccentedVelocity, accentVelocity);
    cascara.notes.legato();
//    applyCascaraAccents(cascara.notes, subdivision);
    
    return cascara;
}

// todo: some way of preventing 0 syncopation from happening
Phrase rhythm::randomClave(Phrase fromPhrase, int minNoteLengthInSubdivisions, int maxNoteLengthInSubdivisions) {
    Phrase clave(fromPhrase);
    
    const Duration subdivision = clave.primarySubdivision();
    Duration minNoteLength = minNoteLengthInSubdivisions * subdivision;
    Duration maxNoteLength = maxNoteLengthInSubdivisions * subdivision;
    
    // choose a random arrangement  of that num notes on each side such that
    // space between any two notes (even across sides, both ways) is 2, 3, or 4
    std::array<double,17> weights = {0,1,1,2,1,2,2,2,1,2,1,1,1,1,1,1,1}; // three is most likely
    for (int i = 0; i < minNoteLengthInSubdivisions; i++) { weights[i] = 0; }
    for (int i = weights.size(); i > maxNoteLengthInSubdivisions; i--) { weights[i] = 0; }
    std::discrete_distribution<int> randomNoteLengthInSubdivisions (weights.begin(), weights.end());

    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    clave.duration = Bars(1);
    int numNotes = getPotentialClaveNoteCount(clave, minNoteLength, maxNoteLength);
    // const int maxClaveNotes = 10; // a regular clave rhythm should really not have too many notes.
    // while (numNotes > maxClaveNotes) {
        // clave.duration -= Bars(1);
        // numNotes = getPotentialClaveNoteCount(clave, minNoteLength, maxNoteLength);
    // }
    int notesOnLeft = chooseNumberOfNotesOnLeft(numNotes);

    bool constraintsBroken = false;
    int attempts = 0;
    do {
        constraintsBroken = false;
        clave.notes.clear();
        attempts++;
        if (attempts > 2000) {
            //give up even if it's wrongish
            break;
        }
        float notePosition = subdivision * (rollDie(maxNoteLengthInSubdivisions) - 1);
        for (int noteInd = 0; noteInd < numNotes; noteInd++) {
            Note note = claveNote(notePosition);
            // note doesn't fall within its side, scrap it and try again
            if ((noteInd < notesOnLeft && clave.isNoteOnRight(note)) ||
                (noteInd >= notesOnLeft && (clave.isNoteOnLeft(note) || note.startTime >= clave.duration))) {
                constraintsBroken = true;
                break;
            }
            
            if (noteInd == numNotes - 1) { // last note
                note.duration = (clave.duration - note.startTime) + clave.notes.front().startTime;
                if (note.duration < (double) minNoteLength || // last note is bad length
                    note.duration > (double) maxNoteLength) {
                    constraintsBroken = true;
                    break;
                }
            } else {
                auto randomNum = randomNoteLengthInSubdivisions(getGen());
                note.duration = randomNum * subdivision;
            }
            notePosition += note.duration;
            clave.addNote(note);
        }
    } while (constraintsBroken);
    
    // while (clave.duration < fromPhrase.duration) {
    //     clave.notes.concat(clave.notes);
    //     clave.duration = 2 * clave.duration;
    // }
    
    return clave;
}


Phrase rhythm::claveFrom(Phrase fromPhrase, int minNoteLengthInSubdivisions, int maxNoteLengthInSubdivisions) {
    Phrase other(fromPhrase);
    if (other.notes.isPolyphonic()) { other.notes = other.notes.toMonophonic(); }
    Phrase clave(fromPhrase);
    clave.notes.clear();
    
    const Duration subdivision = clave.primarySubdivision();
    Duration minNoteLength = minNoteLengthInSubdivisions * subdivision;
    Duration maxNoteLength = maxNoteLengthInSubdivisions * subdivision;
    int numNotes = getPotentialClaveNoteCount(clave, minNoteLength, maxNoteLength);
    int notesOnLeft = chooseNumberOfNotesOnLeft(numNotes);
    int notesOnRight = numNotes - notesOnLeft;
    
    double otherNotesOnLeft = 0;
    double otherNotesOnRight = 0;
    
    for (auto note : other.notes)
    {
        if (other.isNoteOnLeft(note)) { otherNotesOnLeft++; }
        else { otherNotesOnRight++; }
    }
    
    int attempts = 0;
    bool constraintsBroken = false;
    do {
        attempts++;
        constraintsBroken = false;
        double notesNeededOnLeft = notesOnLeft;
        double notesNeededOnRight = notesOnRight;
        clave.notes.clear();
        
        for (auto noteIt = other.notes.begin();
             noteIt != other.notes.end();
             noteIt++)
        {
            
            bool candidateOnLeft = notesNeededOnLeft > 0 && other.isNoteOnLeft(*noteIt);
            bool candidateOnRight = notesNeededOnRight > 0 && !other.isNoteOnLeft(*noteIt);
            
            Probability needToAcceptNote = candidateOnLeft
                ? notesOnLeft / otherNotesOnLeft
                : (candidateOnRight
                    ? notesOnRight / otherNotesOnRight
                    : 0.);
            
            if (flipWeightedCoin(needToAcceptNote)) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                candidateOnLeft ? notesNeededOnLeft-- : 0.;
                candidateOnRight ? notesNeededOnRight-- : 0.;
                clave.addNote(claveNote(noteIt->startTime));
            }
        }
        
        // Fill in gaps left.
        clave = fillClave(clave, notesNeededOnLeft, notesNeededOnRight, minNoteLength, maxNoteLength);
        constraintsBroken = clave.notes.empty();
        
        if (!constraintsBroken) {
            clave.notes.legato();
            for (auto note : clave.notes) {
                if (note.duration < minNoteLength || // bad length
                    note.duration > maxNoteLength) {
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

    // maybe 2/3 - 3/4 of  accents end up being clave notes - the rest fall right next to clave notes. actually acheive these ratios - don't just use them as probabilities that you don't enforce.
    // if two doubles in a row (xx.xx) maybe choose first x of first double and second x of second double, so it's a note of length 4 subdivs
    
    // string notes of length 2 subdivs? probably remove the middle one so it's a note of length 4
    // careful cos there could be like 5 ina row really
    
    return clave;
}
