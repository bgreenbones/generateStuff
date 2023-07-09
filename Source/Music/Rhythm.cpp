/*
  ==============================================================================

    Rhythm.cpp
    Created: 7 Mar 2023 10:09:04pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Rhythm.h"
#include "Voice.h"
#include "Ensemble.h"
#include "Schedule.h"
#include "Utility.h"



Phrase rhythm::leaveSpace(Phrase tooBusy, vector<Phrase> competingVoices, float density)
{
    
    // tooBusy.subdivisions.tie(true);
    
    vector<Time> timeToBeQuiet = rhythm::busySpots(competingVoices);

    if (timeToBeQuiet.empty()) {
      // in this case we might just add rhythms on top of other active voices 
      // instead of trying to fill in space
      return tooBusy;
    }
  
    // sort(timeToBeQuiet.begin(), timeToBeQuiet.end(),
    //     [](Time const &a, Time const &b) { return a.duration > b.duration; }); // longest to shortest
    
    for (Time time : timeToBeQuiet) {
      if (flipWeightedCoin(1 - density) && // fill in less 
          flipWeightedCoin(1 - time.startTime / tooBusy.getDuration())) { // fill in more as time goes on
        continue;
      }
      vector<Timed<Note>*> notesToCutOff = tooBusy.notes.pointersByPosition(time.startTime);
      for (auto note : notesToCutOff) {
        note->setEndTime(time.startTime);
      }
      vector<Timed<Note>*> notesToMoveUp = tooBusy.notes.pointersByPosition(time.endTime());
      for (auto note : notesToMoveUp) {
        note->setStartTime(time.endTime());
      }
    }

    return tooBusy;

}


Phrase rhythm::fillLegatoLongTones(Phrase unfilled, vector<Phrase> competingVoices, float density) {
  unfilled.subdivisions.tie(true);
  
  Phrase filled(unfilled);
  vector<Time> spaces = rhythm::gaps(filled, competingVoices);

  // if (totalSpaceToFill / unfilledVoicings.getDuration() < 1./6.
  //       || spaces.empty()) {
  if (spaces.empty()) {
    // in this case we might just add rhythms on top of other active voices 
    // instead of trying to fill in space
    return unfilled;
  }

  sort(spaces.begin(), spaces.end(), 
        [](Time const &a, Time const &b) { return a.duration > b.duration; }); // longest to shortest

  for (int i = 0; i < spaces.size() / 2 + 1; i++) {
    if (flipWeightedCoin(0.3)) {// TODO: let's do better random
      continue;
    }
    Time spaceToFill = spaces[i];
    
    Duration subdivision = unfilled.subdivisions.drawByPosition(spaceToFill.startTime);
    int subdivisionsInSpace = spaceToFill.duration / subdivision;
    int lengthInSubdivisions = rollDie(subdivisionsInSpace);
    
    double modifyProportion = uniformDouble(0.4, 0.6);
    double doubleProportion = 0.6 * (1. - density); 
    double halfProportion = (doubleProportion / 2.) * 0.2 * density;
    vector<Time> times = rhythm::nOfLengthM(lengthInSubdivisions, subdivision);
    times = rhythm::doublesAndDiddles(times, modifyProportion, doubleProportion, halfProportion);
    // times = rhythm::doublesAndDiddles(times, 0.5);

    double displacementInSubdivisions = uniformInt(0, subdivisionsInSpace - lengthInSubdivisions);
    Duration displacement = displacementInSubdivisions * subdivision;
    
    vector<Timed<Note>> vertical;
    for (Time time : times) {
        Position realStartTime = spaceToFill.startTime + displacement + time.startTime;
        Time noteTime(realStartTime, time.duration);
        
        vector<Timed<Note>> newVertical = unfilled.notes.byPosition(realStartTime);
        vertical = newVertical.empty() ? vertical : newVertical;
        
        // TODO: make this depend on how many chords we actually want to play
        // OverwriteBehavior overwrite = draw<OverwriteBehavior>({OverwriteBehavior::insert, OverwriteBehavior::cutoff});
        OverwriteBehavior overwrite = OverwriteBehavior::insert;
        for (Timed<Note> note : vertical) {
            filled.notes.add(Timed<Note>(noteTime, note.item), PushBehavior::ignorePush, overwrite);
        }
    }
  }

  return filled;
}



vector<Time> rhythm::doublesAndDiddles(vector<Time> times, double modifyProportion, double doubleProportion, double halfProportion) {
    // choose times to modify
    vector<vector<Time*>> toDoubleOrHalf = rhythm::distinctSubsets<Time>(times, modifyProportion, {doubleProportion, halfProportion});
    vector<Time*> toDouble = toDoubleOrHalf[0];
    vector<Time*> toHalf = toDoubleOrHalf[1];
    // // modify times
    rhythm::multiplyTimeLength(times, toDouble, 2);
    rhythm::repeat(times, toHalf, 0.5);

    return times;
}
    
vector<Time> rhythm::busySpots(vector<Phrase> competingVoices, Duration threshold) {

  vector<Position> startTimes = {0};
  vector<Position> allStartTimes = {0};
  for (Phrase competingPhrase : competingVoices) {
    for (Timed<Note> note : competingPhrase.notes) {
      allStartTimes.push_back(note.startTime);
    }
  }
    
  sort(allStartTimes);
  for (Position startTime : allStartTimes) {
      if (startTime != startTimes[startTimes.size() - 1]) {
        startTimes.push_back(startTime);
      }
  }
  vector<Time> busy;
  bool trackingBusySpot = false;
  Time busySpot;
  for (auto it = startTimes.begin(); it != startTimes.end(); it++) {
    auto next = it + 1;
    if (next == startTimes.end()) {
      break;
    }
    Duration difference = *next - *it;
    if (difference <= threshold) {
        if (!trackingBusySpot) {
          trackingBusySpot = true; 
          busySpot.startTime = *it;
          busySpot.duration = difference;
        } else {
          busySpot.duration += difference;
        }
    } else {
      busy.push_back(busySpot);
      trackingBusySpot = false;
    }
  }
  return busy;
} 
vector<Time> rhythm::gaps(Phrase gapFiller, vector<Phrase> competingVoices) {

  vector<Position> startTimes = {0};
  vector<Position> allStartTimes = {0};
  for (Phrase competingPhrase : competingVoices) {
    for (Timed<Note> note : competingPhrase.notes) {
      if (note.startTime < gapFiller.getDuration()) {
        allStartTimes.push_back(note.startTime);
      }
    }
  }
  allStartTimes.push_back(gapFiller.getDuration());
    
  sort(allStartTimes);
  for (Position startTime : allStartTimes) {
      if (startTime != startTimes[startTimes.size() - 1]) {
        startTimes.push_back(startTime);
      }
  }
  vector<Time> spaces;
  for (auto it = startTimes.begin(); it != startTimes.end(); it++) {
    auto next = it + 1;
    if (next == startTimes.end()) {
      break;
    }
    Duration difference = *next - *it;
    Duration subdivision = gapFiller.subdivisions.drawByPosition(*it);
    Duration threshold = subdivision;
    if (difference > threshold) {
      spaces.push_back(Time(*it, difference));
    }
  }
  return spaces;
}

void rhythm::multiplyTimeLength(vector<Time>& timed, vector<Time*> toMultiply, double multiplyBy) {
    for (auto timeIter = timed.begin(); timeIter < timed.end(); timeIter++) {
        Time* time = &(*timeIter);
        if (!contains<Time*>(toMultiply, time)) {
          continue;
        }
        if (multiplyBy > 1 && timeIter == timed.end() - 1) {
          continue;
        }
        
        time->duration = multiplyBy * time->duration;
        if (timeIter + 1 == timed.end()) { break; }
        else if ((timeIter + 1)->containsPartially(*time)) {
            timeIter = timed.erase(timeIter + 1) - 1;}
    }
}

void rhythm::repeat(vector<Time>& timed, vector<Time*> toMultiply, double multiplyBy) {
    if (multiplyBy >= 1) { return; }
    for (auto timeIter = timed.begin(); timeIter < timed.end(); timeIter++) {
        Time* time = &(*timeIter);
        if (!contains<Time*>(toMultiply, time)) { continue; }
        
        Position originalEndTime = time->endTime();
        time->duration = multiplyBy * time->duration;
        Time repetition(*time);
        while (repetition.endTime() < originalEndTime) {
            repetition.startTime = repetition.endTime();
            if (repetition.endTime() > originalEndTime) {repetition.setEndTime(originalEndTime);}
            timeIter = timed.insert(timeIter + 1, repetition);
        }
    }
}


template <typename T>
vector<T*> rhythm::selectAtRandom(vector<T>& t, Probability prob) {
    vector<T*> result;
    for (T & time : t) {
      if (prob) {
        result.push_back(&time);
      }
    }
    return result;
}
template vector<Time*> rhythm::selectAtRandom(vector<Time>& t, Probability prob);
template vector<Timed<Note>*> rhythm::selectAtRandom(vector<Timed<Note>>& t, Probability prob);

template <typename T>
vector<vector<T*>> rhythm::distinctSubsets(vector<T>& t, int n, Probability prob) {
    vector<T*> candidates = selectAtRandom<T>(t, prob);
    vector<vector<T*>> result;
    for (int i = 0; i < n; i++) {
        result.push_back(vector<T*>());
    }
    for (T* time : candidates) {
        result[rollDie(n - 1)].push_back(time);        
    }
    return result;    
}
template vector<vector<Time*>> rhythm::distinctSubsets(vector<Time>& t, int n, Probability prob);
template vector<vector<Timed<Note>*>> rhythm::distinctSubsets(vector<Timed<Note>>& t, int n, Probability prob);

template <typename T>
vector<vector<T*>> rhythm::distinctSubsets(vector<T>& t, Probability prob, vector<double> distribution) {
    vector<vector<T*>> result;
    int n = distribution.size();
    if (n <= 1) { return {selectAtRandom<T>(t, prob)}; }
    std::discrete_distribution<int> selectSubset (distribution.begin(), distribution.end());
    vector<T*> candidates = selectAtRandom<T>(t, prob);
    for (int i = 0; i < n; i++) {
        result.push_back(vector<T*>());
    }
    for (T* time : candidates) {
        result[selectSubset(getGen())].push_back(time);
    }
    return result;    
}
template vector<vector<Time*>> rhythm::distinctSubsets(vector<Time>& t, Probability prob, vector<double> distribution);
template vector<vector<Timed<Note>*>> rhythm::distinctSubsets(vector<Timed<Note>>& t, Probability prob, vector<double> distribution);

vector<Time> rhythm::onePerShortForLong(Duration longDuration, Duration shortDuration) {
    vector<Time> timeds;
    int numberOfTimeds = longDuration / shortDuration;
    int timedNumber = 0;
    while(numberOfTimeds-- > 0) {
        Duration startTime = (timedNumber++) * shortDuration;
        Duration untilEnd = longDuration - startTime;
        Duration timedLength(untilEnd < shortDuration ? untilEnd : shortDuration);
        Time timed(startTime, timedLength);
        timeds.push_back(timed);
    }
    return timeds;
}

vector<Time> rhythm::nOfLengthM(int n, Duration m) {
    vector<Time> timeds;
    if (n < 1) {
        return timeds;
    }
    for (int i = 0; i < n; i++) {
        Position startTime = i * m;
        timeds.push_back(Time(startTime, m));
    }
    return timeds;
}

Phrase rhythm::rhythmicVariation(Phrase source) {

    for (Timed<Note>& note : source.notes) {

        if (flipWeightedCoin(0.2)) {
            Duration subDiv = source.subdivisions.drawByPosition(note.startTime);
            Position newStartTime = note.startTime - note.duration;
            if (flipCoin()) {        
                // Subdivision subDiv = draw<Subdivision>(source.subdivisions.bySpan(TimedEvent(newStartTime, 2 * note.duration)));
                newStartTime = note.startTime - subDiv;
                note.startTime = newStartTime;
                vector<reference_wrapper<Timed<Note>>> overlappingNotes = source.notes.refsBySpan(note);
                for (Timed<Note> & overlapping : overlappingNotes) {
                    overlapping.duration = newStartTime - overlapping.startTime;
                }
            } else {
                newStartTime = note.startTime + subDiv;
                note.startTime = newStartTime;
                vector<reference_wrapper<Timed<Note>>> overlappingNotes = source.notes.refsBySpan(note);
                for (Timed<Note> & overlapping : overlappingNotes) {
                    overlapping.duration = overlapping.endTime() - note.startTime;
                    overlapping.startTime = note.endTime();
                }
            }
        }

        
    }
    return source;
}

// will be 0 < stability <= 1.0
double rhythm::beatWiseStability(Position position) {
    Beats startingPoint = floor(position.asBeats());
    Beats positionInBeat = position - startingPoint;
    if (positionInBeat == Beats(0.)) { return 1.; }
    // if (positionInBeat == Beats(0.)) { positionInBeat += Beats(1); }
    
    bool inFirstHalf = positionInBeat <= Beats(0.5);
    Beats halfWisePosition = inFirstHalf ? positionInBeat : positionInBeat - Beats(0.5);
    double halfWiseStability = halfWisePosition / Beats(0.5);

    // TODO: we need to use this to get -- lower stability scores in first half of beat, higher stability scores in second half of beat (highest of those is on the half way marker though) and highest scores are ON the beat
    double secondHalfStablenessBoost = 0.3;
    double stability = inFirstHalf ? halfWiseStability : halfWiseStability + secondHalfStablenessBoost; // higher stabilities for second half
    stability /= (1. + secondHalfStablenessBoost); // normalize between 0 and 1
    
    return stability;
}


Phrase rhythm::stabilityBased(Phrase fromPhrase, Probability filter)
{
    fromPhrase.notes.clear();
    Position cursor = fromPhrase.getStartTime();
    while (cursor < fromPhrase.getEndTime()) {
        Duration subdiv = fromPhrase.subdivisions.drawByPosition(cursor);
        
        if(Probability(rhythm::beatWiseStability(cursor)) && filter) {
            fromPhrase.notes.add(Timed<Note>(Time(cursor, subdiv)));
        }
        
        cursor += subdiv;
    }
    
    return fromPhrase;
}

vector<Time> rhythm::stabilityBased(Time time, 
                                    Sequence<Duration> subdivisions,
                                    double stabilityThreshold, 
                                    Probability filter,
                                    bool markBeginning)
{
    time.trim(subdivisions.parent);
    Position cursor = time.startTime;
    vector<Time> result;
    while (cursor < time.endTime()) {
        Duration subdiv = subdivisions.drawByPosition(cursor);
        
        if (cursor == time.startTime && markBeginning) {
            result.push_back(Time(cursor - time.startTime, subdiv));
            cursor += subdiv;
            continue;
        }

        double stability = rhythm::beatWiseStability(cursor); 
        double thresholdedStability = stability > stabilityThreshold 
            ? (stability - stabilityThreshold) / (1. - stabilityThreshold)
            : 0.; 
        if(Probability(thresholdedStability) && filter) {
            result.push_back(Time(cursor - time.startTime, subdiv));
        }
        cursor += subdiv;
    }
    
    return result;
}


Phrase rhythm::stabilityFilter(Phrase fromPhrase, Direction direction) {
    // TODO: maybe parameterize number of notes to move, or likelihood of any note moving
    // TODO: think about what part of the phrase to move a note...based on idea of moving from tension to resolution or vice versa
    // TODO: make a cool thing that moves around startTimes in a chord so we get a sweep effect.
    bool searchingForMovableNote = true;
    
    Sequence<Note> monoNotesCopy = fromPhrase.notes.toMonophonic();
    while (searchingForMovableNote && !monoNotesCopy.empty()) {
        shuffle(monoNotesCopy.begin(), monoNotesCopy.end(), getGen());
        Timed<Note> note = monoNotesCopy.back(); // sample without replacement.
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
        
        for(Timed<Note> &otherNote : fromPhrase.notes) {
            if (otherNote.startTime == note.startTime) {
                otherNote.startTime = newStartTime;
                otherNote.duration = newDuration;
            }
        }
    }
    return fromPhrase;
}



Phrase rhythm::burst(Phrase fromPhrase, Timed<Note> note, int minimumRepeats, int maximumRepeats, float noteLengthInSubdivisions) {
    vector<Timed<Duration>> subdivs = fromPhrase.subdivisions.byPosition(note.startTime);
    Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
    Duration noteLength = noteLengthInSubdivisions * subdiv;
    note.duration = noteLength;
    
    int numberOfPossibleBurstLengths = maximumRepeats - minimumRepeats;
    int numberOfNotes = rollDie(numberOfPossibleBurstLengths);

    vector<Time> times = nOfLengthM(numberOfNotes, noteLength);
    vector<Timed<Note>> notes = Sequence<Note>::fromTimes(times, note);
    fromPhrase.notes.insertVector(notes, note.startTime, PushBehavior::ignorePush, OverwriteBehavior::cutoff);
    // for (Timed time : times) {
    //     Timed<Note> repeatTimed<Note>(note.pitch, note.velocity, note.startTime + time.startTime, noteLength);
    //     fromPhrase.notes.add(repeatTimed<Note>, PushBehavior::ignore, OverwriteBehavior::cutoff);
    // }
    
    // for (double repeat = 0; repeat < numberOfTimed<Note>s; repeat++) {
    //     Position position = note.startTime + repeat * noteLength;
    //     if (fromPhrase.notes.byStartPosition(position).empty()) {
    //         Timed<Note> repeatTimed<Note>(note.pitch, note.velocity, position, noteLength);
    //         fromPhrase.notes.add(repeatTimed<Note>, PushBehavior::ignore, OverwriteBehavior::cutoff);
    //     }
    // }
    
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
    fromPhrase.notes = fromPhrase.notes.pulseAndDisplace(pulse, displacement, pDisplace, pDouble, fromPhrase.getDuration());
    return fromPhrase;
}



Probability ornamentProbabilityClave = 0.75;
int clavePitch = 60;
DynamicLevel claveVelocity = ff;

Timed<Note> claveNote(Position startTime, Duration duration = 1) {
    Note n = Note(clavePitch, claveVelocity).accent();
    n.ornamented = ornamentProbabilityClave;
    Timed<Note> newClaveNote = Timed<Note>(Time(startTime, duration), n);
    return newClaveNote;
}

void applyCascaraAccents(Sequence<Note> &cascara, Duration displacement) { // TODO: should maybe take an accompanying clave to determine where accents are? or should just note the accents on the cascara itself when we generate...
    
    for (auto noteIt = cascara.begin();
         noteIt != cascara.end();
         noteIt++) {
        if (noteIt->duration == displacement) { // beginning of a double
            noteIt->item.velocity = unaccentedVelocity;
        } else {
            noteIt->item.accented = true;
            noteIt->item.velocity = accentVelocity;
            noteIt->item.ornamented = 0.5;
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


int rhythm::getPotentialClaveNoteCount(Phrase clave,
                                        Duration minNoteLength,
                                        Duration maxNoteLength,
                                        double density) {
    cout << "get potential clave count" << std::endl;
    Duration subdivision = clave.primarySubdivision();

    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
//    int maxNumNotes = floor(clave.getDuration() / minNoteLength) - 1;
//    int minNumNotes = ceil(clave.getDuration() / maxNoteLength) + 1;
    int maxNumNotes = floor(clave.getDuration() / minNoteLength);
    int minNumNotes = ceil(clave.getDuration() / maxNoteLength);
    auto numNotesRange = maxNumNotes - minNumNotes;
    if (numNotesRange < 0) { throw exception(); }
    int numNotes = minNumNotes + density * (maxNumNotes - minNumNotes);
    // auto numNotes = uniformInt(minNumNotes, maxNumNotes); // todo: parameterize, but keep random option
    numNotes = min(numNotes, maxNumNotes);
    numNotes = max(numNotes, minNumNotes);
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
            Timed<Note> currentNote = *noteIt;
            while (timeBetweenNotes > maxNoteLength && (notesNeededOnLeft > 0 || notesNeededOnRight > 0))
            {
                Duration wrapAround = (nextNote->startTime < currentNote.startTime) ? clave.getDuration() : Duration(0.);
                Position earliestNoteTime = currentNote.startTime + minNoteLength; // need to wrap around phrase bounds
                Position latestNoteTime = nextNote->startTime + wrapAround - minNoteLength;
                // TODO: some validation on possible note times?g
                int numberOfPossibleNoteTimes = ((latestNoteTime - earliestNoteTime).asQuarters() / subdivision.asQuarters()) + 1;
                Position chosenNoteTime = earliestNoteTime + (rollDie(numberOfPossibleNoteTimes) - 1) * subdivision;
                chosenNoteTime = (chosenNoteTime > clave.getDuration()) ? chosenNoteTime - clave.getDuration() : chosenNoteTime;
                Timed<Note> newNote = claveNote(chosenNoteTime, nextNote->startTime - chosenNoteTime);
                
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

Phrase rhythm::cascaraFrom(Phrase fromPhrase, float density) {
    Phrase clave(fromPhrase);
    if (clave.notes.size() <= 0) {
        DBG ("no notes to generate a cascara from");
        return randomCascara(fromPhrase);
    }

    if (clave.notes.isPolyphonic()) { clave.notes = clave.notes.toMonophonic(); }
    Phrase cascara(fromPhrase);
    Duration subdivision = cascara.primarySubdivision();
    cascara.notes.clear();
    Timed<Note> firstNote = clave.notes.front().withDuration(subdivision);
    firstNote.item = firstNote.item.withAccent();
    cascara.notes.add(firstNote);

    for (auto noteIt = clave.notes.begin();
         noteIt < clave.notes.end();
         noteIt++)
    {
        auto nextNote = next<Note>(clave.notes, noteIt);
        Duration timeBetweenNotes = timeBetween<Note>(*noteIt, *nextNote, clave);
        double subdivisionsBetweenClaveNotes = timeBetweenNotes.asBeats() / subdivision.asBeats();
        Timed<Note>& lastCascaraNote = cascara.notes.back();
        Duration timeSinceLastCascaraNoteStart = noteIt->startTime - lastCascaraNote.startTime;
        double subdivisionsSinceLastCascaraNote = timeSinceLastCascaraNoteStart.asBeats() / subdivision.asBeats();
        
        if (subdivisionsBetweenClaveNotes == 2.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // x . x
                cascara.notes.append(".x", subdivision, PushBehavior::wrap);
                cascara.notes.last()->item.accent();
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // . x x
                    cascara.notes.append(".xx", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                } else { // x . x
                    cascara.notes.append("x.x", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                }
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 3.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit. just add next note.
                auto choice = rollDie(3);
                if (choice == 1) { // x x . x
                    cascara.notes.append("x.x", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                } else if (choice == 2) { // x . x x
                    cascara.notes.append(".xx", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                } else if (choice == 3) { // x . x . misses next note!!
                    cascara.notes.append(".x.", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                // only one option: . x . x
                cascara.notes.append(".x.x", subdivision, PushBehavior::wrap);
                cascara.notes.last()->item.accent();
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveNotes == 4.) {
            if (subdivisionsSinceLastCascaraNote == 0.) { // this note already hit.
                if (flipCoin()) { // x . x . x
                    cascara.notes.append(".x.x", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                } else { // x x . x x
                    cascara.notes.append("x.xx", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                }
            } else if (subdivisionsSinceLastCascaraNote == 1.0) {
                if (flipCoin()) { // // . x x . x
                    cascara.notes.append(".xx.x", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
                } else { // . x . x x
                    cascara.notes.append(".x.xx", subdivision, PushBehavior::wrap);
                    cascara.notes.last()->item.accent();
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
    
    if (density < 0.5) {
        Probability removeProbability((0.5 - density) * 2.);
        for (auto note_it = cascara.notes.begin(); note_it != cascara.notes.end(); note_it++) {
            bool noteStartsDouble = note_it->duration == subdivision;
            if ((noteStartsDouble && flipWeightedCoin(removeProbability))) {
                
                note_it = cascara.notes.erase(note_it) - 1;
            }
        }
    } else if (density > 0.5) {
        Probability fillInProbability((density - 0.5) * 2.);

        for (auto note_it = cascara.notes.begin(); note_it != cascara.notes.end(); note_it++) {
            if (flipWeightedCoin(fillInProbability)) {
                note_it->duration = 0.5 * note_it->duration;
                Timed<Note> repetition(*note_it);
                repetition.startTime = note_it->endTime();
                note_it = cascara.notes.insert(note_it + 1, repetition);
            }
        }
    }
    cascara.notes.legato();
    return cascara;
}


Phrase rhythm::clave(Duration subdivision, double density) {
    int minNoteLength = 2;
    int maxNoteLength = 4;

    if (subdivision == Beats(1./3.)) {
            minNoteLength = 1;
            maxNoteLength = 3;
    } else if (subdivision == Beats(1./5.)) {
            minNoteLength = 2;
            maxNoteLength = 5;
    }
    // TODO: evaluate effects of these and other numbers for other subdivisions.

    vector<double> weights = { 0 };
    double noteLengthRange = maxNoteLength - minNoteLength;
    double middleNoteLength = (double)(maxNoteLength + minNoteLength) / 2.;
    double minWeight = INFINITY;
    for (int noteLength = 1; noteLength <= maxNoteLength; noteLength++) {
        if (noteLength < minNoteLength) {
            weights.push_back(0);
            continue;
        }

        double weight = 
            (maxNoteLength - noteLength) * density +
            (noteLength - minNoteLength) * (1 - density) +
            ((noteLengthRange - abs(noteLength - middleNoteLength)) * (1 - 2 * abs(0.5 - density)));
        weights.push_back(weight);
        minWeight = weight < minWeight ? weight : minWeight;
    }

    for (double &weight : weights) {
        if (weight == 0) continue;
        weight -= minWeight - 1;
    }
    

    return rhythm::randomClave(Phrase(subdivision, 0, Bars(2)), minNoteLength, maxNoteLength, weights, density);
}

// todo: some way of preventing 0 syncopation from happening
Phrase rhythm::randomClave(Phrase fromPhrase, int minNoteLengthInSubdivisions, int maxNoteLengthInSubdivisions,
                            vector<double> weights, double density) {
    Phrase clave(fromPhrase);
    
    const Duration subdivision = clave.primarySubdivision();
    Duration minNoteLength = minNoteLengthInSubdivisions * subdivision;
    Duration maxNoteLength = maxNoteLengthInSubdivisions * subdivision;
    
    // choose a random arrangement  of that num notes on each side such that
    // space between any two notes (even across sides, both ways) is 2, 3, or 4
    // std::array<double,17> weights = {0,1,1,2,1,2,2,2,1,2,1,1,1,1,1,1,1}; // three is most likely
    // for (int i = 0; i < minNoteLengthInSubdivisions; i++) { weights[i] = 0; }
    // for (int i = weights.size(); i > maxNoteLengthInSubdivisions; i--) { weights[i] = 0; }
    std::discrete_distribution<int> randomNoteLengthInSubdivisions (weights.begin(), weights.end());

    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    clave.setDuration(Bars(1)); // todo: do this based on seconds? and/or num notes?

    bool constraintsBroken = false;
    int attempts = 0;
    do {
        int numNotes = getPotentialClaveNoteCount(clave, minNoteLength, maxNoteLength, density);
        int notesOnLeft = chooseNumberOfNotesOnLeft(numNotes);
        constraintsBroken = false;
        clave.notes.clear();
        attempts++;
        if (attempts > 2000) {
            //give up even if it's wrongish
            break;
        }
        float notePosition = subdivision * (rollDie(maxNoteLengthInSubdivisions) - 1);
        for (int noteInd = 0; noteInd < numNotes; noteInd++) {
            Timed<Note> note = claveNote(notePosition);
            // note doesn't fall within its side, scrap it and try again
            if ((noteInd < notesOnLeft && clave.isNoteOnRight(note)) ||
                (noteInd >= notesOnLeft && (clave.isNoteOnLeft(note) || note.startTime >= clave.getDuration()))) {
                constraintsBroken = true;
                break;
            }
            
            if (noteInd == numNotes - 1) { // last note
                note.duration = (clave.getDuration() - note.startTime) + clave.notes.front().startTime;
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
            clave.notes.add(note);
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
    int numNotes = getPotentialClaveNoteCount(clave, minNoteLength, maxNoteLength, {});
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
                clave.notes.add(claveNote(noteIt->startTime));
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
