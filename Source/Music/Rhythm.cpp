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
    
vector<Time> rhythm::gaps(Phrase gapFiller, vector<Phrase> competingVoices) {

  vector<Position> startTimes = {0};
  vector<Position> allStartTimes = {0};
  // Position longestPhraseDuration = 0;
//   vector<Position> startTimes = {0 };
  for (Phrase competingPhrase : competingVoices) {
    // if (competingPhrase.getDuration() > longestPhraseDuration) {
    //   longestPhraseDuration = competingPhrase.getDuration();
    // }
    for (Timed<Note> note : competingPhrase.notes) {
      if (note.startTime < gapFiller.getDuration()) {
        allStartTimes.push_back(note.startTime);
      }
      // startTimes.emplace(note.startTime);
    }
  }
  allStartTimes.push_back(gapFiller.getDuration());
    
  sort(allStartTimes);
  for (Position startTime : allStartTimes) {
      if (startTime != startTimes[startTimes.size() - 1]) {
        startTimes.push_back(startTime);
      }
  }
//   startTimes.push_back(unfilledVoicings.getDuration());
  // sort(startTimes);
  vector<Time> spaces;
  for (auto it = startTimes.begin(); it != startTimes.end(); it++) {
    auto next = it + 1;
    if (next == startTimes.end()) {
      break;
    }
    Duration difference = *next - *it;
    Subdivision subdivision = gapFiller.subdivisions.drawByPosition(*it);
    Duration threshold = subdivision;
    if (difference > threshold) {
      spaces.push_back(Time(*it, difference));
    //   spaces.push_back(Timed(*it + threshold, difference - threshold));
//      totalSpaceToFill += difference;
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
            Subdivision subDiv = source.subdivisions.drawByPosition(note.startTime);
            Position newStartTime = note.startTime - note.duration;
            if (flipCoin()) {        
                // Subdivision subDiv = draw<Subdivision>(source.subdivisions.bySpan(TimedEvent(newStartTime, 2 * note.duration)));
                newStartTime = note.startTime - subDiv;
                note.startTime = newStartTime;
                vector<reference_wrapper<Timed<Note>>> overlappingTimed<Note>s = source.notes.refsBySpan(note);
                for (Timed<Note> & overlapping : overlappingTimed<Note>s) {
                    overlapping.duration = newStartTime - overlapping.startTime;
                }
            } else {
                newStartTime = note.startTime + subDiv;
                note.startTime = newStartTime;
                vector<reference_wrapper<Timed<Note>>> overlappingTimed<Note>s = source.notes.refsBySpan(note);
                for (Timed<Note> & overlapping : overlappingTimed<Note>s) {
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
        Subdivision subdiv = fromPhrase.subdivisions.drawByPosition(cursor);
        
        if(Probability(rhythm::beatWiseStability(cursor)) && filter) {
            fromPhrase.addTimed<Note>(Timed<Note>(cursor, subdiv));
        }
        
        cursor += subdiv;
    }
    
    return fromPhrase;
}

vector<Time> rhythm::stabilityBased(Time time, 
                                    Sequence<Subdivision> subdivisions, 
                                    double stabilityThreshold, 
                                    Probability filter)
{
    Position cursor = time.startTime;
    vector<Time> result;
    while (cursor < time.endTime()) {
        Subdivision subdiv = subdivisions.drawByPosition(cursor);
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
    bool searchingForMovableTimed<Note> = true;
    
    Sequence<Timed<Note>> monoTimed<Note>sCopy = fromPhrase.notes.toMonophonic();
    while (searchingForMovableTimed<Note> && !monoTimed<Note>sCopy.empty()) {
        shuffle(monoTimed<Note>sCopy.begin(), monoTimed<Note>sCopy.end(), getGen());
        Timed<Note> note = monoTimed<Note>sCopy.back(); // sample without replacement.
        monoTimed<Note>sCopy.pop_back();
        double noteStability = rhythm::beatWiseStability(note.startTime);

        Position next = fromPhrase.nextSubdivisionPosition(note.startTime);
        double nextStability = rhythm::beatWiseStability(next);
        Position previous = fromPhrase.previousSubdivisionPosition(note.startTime);
        double previousStability = rhythm::beatWiseStability(previous);
        bool noTimed<Note>sAfter = fromPhrase.notes.byStartPosition(next).empty();
        bool noTimed<Note>sBefore = fromPhrase.notes.byStartPosition(previous).empty();
        
        bool canMoveForward = noTimed<Note>sAfter && direction * nextStability > direction * noteStability;
        bool canMoveBackward = noTimed<Note>sBefore && direction * previousStability > direction * noteStability;
        
        if (!(canMoveForward || canMoveBackward)) { continue; }
        searchingForMovableTimed<Note> = false;
        bool moveForward = canMoveForward;
        if (canMoveForward && canMoveBackward) {
            bool moveForward = flipCoin();
        }
        
        Position newStartTime = moveForward ? next : previous;
        Position nextTimed<Note>StartTime = monoTimed<Note>sCopy.nextStartTime(newStartTime);
        Duration newDuration = min(nextTimed<Note>StartTime - newStartTime, note.duration);
        
        for(Timed<Note> &otherTimed<Note> : fromPhrase.notes) {
            if (otherTimed<Note>.startTime == note.startTime) {
                otherTimed<Note>.startTime = newStartTime;
                otherTimed<Note>.duration = newDuration;
            }
        }
    }
    return fromPhrase;
}



Phrase rhythm::burst(Phrase fromPhrase, Timed<Note> note, int minimumRepeats, int maximumRepeats, float noteLengthInSubdivisions) {
    vector<Subdivision> subdivs = fromPhrase.subdivisions.byPosition(note.startTime);
    Duration subdiv = subdivs.empty() ? sixteenths : subdivs.at(0);
    Duration noteLength = noteLengthInSubdivisions * subdiv;
    note.duration = noteLength;
    
    int numberOfPossibleBurstLengths = maximumRepeats - minimumRepeats;
    int numberOfTimed<Note>s = rollDie(numberOfPossibleBurstLengths);

    vector<Time> times = nOfLengthM(numberOfTimed<Note>s, noteLength);
    vector<Timed<Note>> notes = Sequence<Timed<Note>>::fromTimes(times, note);
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
    fromPhrase.ornamentationTimed<Note>s.flip();
    fromPhrase.connectingTimed<Note>s.flip();
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

Timed<Note> claveTimed<Note>(Position startTime, Duration duration = 1) {
    Timed<Note> newClaveTimed<Note> = Timed<Note>(clavePitch, claveVelocity, startTime, duration).accent();
    newClaveTimed<Note>.ornamented = ornamentProbabilityClave;
    return newClaveTimed<Note>;
}

void applyCascaraAccents(Sequence<Timed<Note>> &cascara, Duration displacement) { // TODO: should maybe take an accompanying clave to determine where accents are? or should just note the accents on the cascara itself when we generate...
    
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
        auto nextTimed<Note> = next<Timed<Note>>(cascara.notes, noteIt);
        Duration timeBetweenTimed<Note>s = timeBetween<Timed<Note>>(*noteIt, *nextTimed<Note>, cascara);
        
        Position spaceStartTime = noteIt->startTime;
        if (timeBetweenTimed<Note>s > (2 * subdivision)) {
            filled.notes.insertSequence(filled.notes.pulseAndDisplace(2 * subdivision, subdivision, 0.75, 0.5, timeBetweenTimed<Note>s - subdivision),
                                         spaceStartTime + subdivision);
        }
    }
            
    return filled;
};


int rhythm::getPotentialClaveTimed<Note>Count(Phrase fromPhrase, Duration minTimed<Note>Length, Duration maxTimed<Note>Length) {
    cout << "get potential clave count" << std::endl;
    Phrase clave(fromPhrase);
    Duration subdivision = clave.primarySubdivision();
    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    int maxNumTimed<Note>s = floor(clave.getDuration() / minTimed<Note>Length) - 1;
    int minNumTimed<Note>s = ceil(clave.getDuration() / maxTimed<Note>Length) + 1;
    auto numTimed<Note>sRange = maxNumTimed<Note>s - minNumTimed<Note>s;
    if (numTimed<Note>sRange < 0) { throw exception(); }
    auto numTimed<Note>s = uniformInt(minNumTimed<Note>s, maxNumTimed<Note>s); // todo: parameterize, but keep random option
    if (numTimed<Note>s % 2 == 0) { // force odd nums for 2-3, 3-2, 3-4, 4-3, etc.
        if (numTimed<Note>s + 1 > maxNumTimed<Note>s) {
            numTimed<Note>s--;
        } else if (numTimed<Note>s - 1 < minNumTimed<Note>s) {
            numTimed<Note>s++;
        } else {
            if (rollDie(3) == 1) { // more likely to subtract - gives more space to rhythms
                numTimed<Note>s--;
            } else {
                numTimed<Note>s++;
            }
        }
    }
    if (numTimed<Note>s > maxNumTimed<Note>s) { throw exception(); }
    return numTimed<Note>s;
}

int rhythm::chooseNumberOfTimed<Note>sOnLeft(double numTimed<Note>s) {
    // choose which side gets how many notes
    // one rule: can't have same # on each side
    bool moreOnLeft = flipCoin();
    double notesOnLeft = moreOnLeft ? ceil (numTimed<Note>s / 2.) : floor (numTimed<Note>s / 2.);
    return notesOnLeft;
}

Phrase rhythm::fillClave(Phrase fromPhrase,
                         int notesNeededOnLeft,
                         int notesNeededOnRight,
                         Duration minTimed<Note>Length,
                         Duration maxTimed<Note>Length) {
    Phrase clave(fromPhrase);
    Duration subdivision = clave.primarySubdivision();
    
    Phrase filledClave(clave);
    if (notesNeededOnLeft > 0 || notesNeededOnRight > 0) {
        for (auto noteIt = clave.notes.begin();
             noteIt < clave.notes.end();
             noteIt++)
        {
            auto nextTimed<Note> = next<Timed<Note>>(clave.notes, noteIt);
            Duration timeBetweenTimed<Note>s = timeBetween<Timed<Note>>(*noteIt, *nextTimed<Note>, filledClave);
            
            if (timeBetweenTimed<Note>s <= maxTimed<Note>Length) { continue; }
            
            int iters = 0;
            Timed<Note> currentTimed<Note> = *noteIt;
            while (timeBetweenTimed<Note>s > maxTimed<Note>Length && (notesNeededOnLeft > 0 || notesNeededOnRight > 0))
            {
                Duration wrapAround = (nextTimed<Note>->startTime < currentTimed<Note>.startTime) ? clave.getDuration() : Duration(0.);
                Position earliestTimed<Note>Time = currentTimed<Note>.startTime + minTimed<Note>Length; // need to wrap around phrase bounds
                Position latestTimed<Note>Time = nextTimed<Note>->startTime + wrapAround - minTimed<Note>Length;
                // TODO: some validation on possible note times?g
                int numberOfPossibleTimed<Note>Times = ((latestTimed<Note>Time - earliestTimed<Note>Time).asQuarters() / subdivision.asQuarters()) + 1;
                Position chosenTimed<Note>Time = earliestTimed<Note>Time + (rollDie(numberOfPossibleTimed<Note>Times) - 1) * subdivision;
                chosenTimed<Note>Time = (chosenTimed<Note>Time > clave.getDuration()) ? chosenTimed<Note>Time - clave.getDuration() : chosenTimed<Note>Time;
                Timed<Note> newTimed<Note> = claveTimed<Note>(chosenTimed<Note>Time, nextTimed<Note>->startTime - chosenTimed<Note>Time);
                
                bool newTimed<Note>IsOnLeft = clave.isTimed<Note>OnLeft(newTimed<Note>);
                bool newTimed<Note>IsOnRight = !newTimed<Note>IsOnLeft;
                if (newTimed<Note>IsOnLeft && notesNeededOnLeft > 0) {
                    notesNeededOnLeft--;
                    filledClave.notes.add(newTimed<Note>, PushBehavior::wrap, OverwriteBehavior::cutoff);
                } else if (newTimed<Note>IsOnRight && notesNeededOnRight > 0) {
                    notesNeededOnRight--;
                    filledClave.notes.add(newTimed<Note>, PushBehavior::wrap, OverwriteBehavior::cutoff);
                }
                timeBetweenTimed<Note>s = timeBetween<Timed<Note>>(newTimed<Note>, *nextTimed<Note>, filledClave);
                currentTimed<Note> = newTimed<Note>;
                
                if (timeBetweenTimed<Note>s <= maxTimed<Note>Length) { break; }
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
    cascara.addTimed<Note>(clave.notes.front()
                    .withAccent()
                    .withDuration(subdivision));

    for (auto noteIt = clave.notes.begin();
         noteIt < clave.notes.end();
         noteIt++)
    {
        auto nextTimed<Note> = next<Timed<Note>>(clave.notes, noteIt);
        Duration timeBetweenTimed<Note>s = timeBetween<Timed<Note>>(*noteIt, *nextTimed<Note>, clave);
        double subdivisionsBetweenClaveTimed<Note>s = timeBetweenTimed<Note>s.asBeats() / subdivision.asBeats();
        Timed<Note>& lastCascaraTimed<Note> = cascara.notes.back();
        Duration timeSinceLastCascaraTimed<Note>Start = noteIt->startTime - lastCascaraTimed<Note>.startTime;
        double subdivisionsSinceLastCascaraTimed<Note> = timeSinceLastCascaraTimed<Note>Start.asBeats() / subdivision.asBeats();
        
        if (subdivisionsBetweenClaveTimed<Note>s == 2.) {
            if (subdivisionsSinceLastCascaraTimed<Note> == 0.) { // x . x
                cascara.notes.append(".X", subdivision, PushBehavior::wrap);
            } else if (subdivisionsSinceLastCascaraTimed<Note> == 1.0) {
                if (flipCoin()) { // . x x
                    cascara.notes.append(".xX", subdivision, PushBehavior::wrap);
                } else { // x . x
                    cascara.notes.append("x.X", subdivision, PushBehavior::wrap);
                }
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveTimed<Note>s == 3.) {
            if (subdivisionsSinceLastCascaraTimed<Note> == 0.) { // this note already hit. just add next note.
                auto choice = rollDie(3);
                if (choice == 1) { // x x . x
                    cascara.notes.append("x.X", subdivision, PushBehavior::wrap);
                } else if (choice == 2) { // x . x x
                    cascara.notes.append(".xX", subdivision, PushBehavior::wrap);
                } else if (choice == 3) { // x . x . misses next note!!
                    cascara.notes.append(".x.", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraTimed<Note> == 1.0) {
                // only one option: . x . x
                cascara.notes.append(".x.X", subdivision, PushBehavior::wrap);
            } else {
                DBG ("cascara has some weird note lengths??");
            }
        } else if (subdivisionsBetweenClaveTimed<Note>s == 4.) {
            if (subdivisionsSinceLastCascaraTimed<Note> == 0.) { // this note already hit.
                if (flipCoin()) { // x . x . x
                    cascara.notes.append(".x.X", subdivision, PushBehavior::wrap);
                } else { // x x . x x
                    cascara.notes.append("x.xX", subdivision, PushBehavior::wrap);
                }
            } else if (subdivisionsSinceLastCascaraTimed<Note> == 1.0) {
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
Phrase rhythm::randomClave(Phrase fromPhrase, int minTimed<Note>LengthInSubdivisions, int maxTimed<Note>LengthInSubdivisions) {
    Phrase clave(fromPhrase);
    
    const Duration subdivision = clave.primarySubdivision();
    Duration minTimed<Note>Length = minTimed<Note>LengthInSubdivisions * subdivision;
    Duration maxTimed<Note>Length = maxTimed<Note>LengthInSubdivisions * subdivision;
    
    // choose a random arrangement  of that num notes on each side such that
    // space between any two notes (even across sides, both ways) is 2, 3, or 4
    std::array<double,17> weights = {0,1,1,2,1,2,2,2,1,2,1,1,1,1,1,1,1}; // three is most likely
    for (int i = 0; i < minTimed<Note>LengthInSubdivisions; i++) { weights[i] = 0; }
    for (int i = weights.size(); i > maxTimed<Note>LengthInSubdivisions; i--) { weights[i] = 0; }
    std::discrete_distribution<int> randomTimed<Note>LengthInSubdivisions (weights.begin(), weights.end());

    // aspects of clave:
    //   1. groupings of 2, 3, and 4
    //   2. 2-sided - 2-3 and 3-2 - even 2-1 and 1-2 -  maybe 3-4 and 4-3 - maybe 2-4 and 4-2?
    //      a. the longer they are, the more can fit in?
    clave.setDuration(Bars(1));
    // const int maxClaveTimed<Note>s = 10; // a regular clave rhythm should really not have too many notes.
    // while (numTimed<Note>s > maxClaveTimed<Note>s) {
        // clave.duration -= Bars(1);
        // numTimed<Note>s = getPotentialClaveTimed<Note>Count(clave, minTimed<Note>Length, maxTimed<Note>Length);
    // }

    bool constraintsBroken = false;
    int attempts = 0;
    do {
        int numTimed<Note>s = getPotentialClaveTimed<Note>Count(clave, minTimed<Note>Length, maxTimed<Note>Length);
        int notesOnLeft = chooseNumberOfTimed<Note>sOnLeft(numTimed<Note>s);
        constraintsBroken = false;
        clave.notes.clear();
        attempts++;
        if (attempts > 2000) {
            //give up even if it's wrongish
            break;
        }
        float notePosition = subdivision * (rollDie(maxTimed<Note>LengthInSubdivisions) - 1);
        for (int noteInd = 0; noteInd < numTimed<Note>s; noteInd++) {
            Timed<Note> note = claveTimed<Note>(notePosition);
            // note doesn't fall within its side, scrap it and try again
            if ((noteInd < notesOnLeft && clave.isTimed<Note>OnRight(note)) ||
                (noteInd >= notesOnLeft && (clave.isTimed<Note>OnLeft(note) || note.startTime >= clave.getDuration()))) {
                constraintsBroken = true;
                break;
            }
            
            if (noteInd == numTimed<Note>s - 1) { // last note
                note.duration = (clave.getDuration() - note.startTime) + clave.notes.front().startTime;
                if (note.duration < (double) minTimed<Note>Length || // last note is bad length
                    note.duration > (double) maxTimed<Note>Length) {
                    constraintsBroken = true;
                    break;
                }
            } else {
                auto randomNum = randomTimed<Note>LengthInSubdivisions(getGen());
                note.duration = randomNum * subdivision;
            }
            notePosition += note.duration;
            clave.addTimed<Note>(note);
        }
    } while (constraintsBroken);
    
    // while (clave.duration < fromPhrase.duration) {
    //     clave.notes.concat(clave.notes);
    //     clave.duration = 2 * clave.duration;
    // }
    
    return clave;
}


Phrase rhythm::claveFrom(Phrase fromPhrase, int minTimed<Note>LengthInSubdivisions, int maxTimed<Note>LengthInSubdivisions) {
    Phrase other(fromPhrase);
    if (other.notes.isPolyphonic()) { other.notes = other.notes.toMonophonic(); }
    Phrase clave(fromPhrase);
    clave.notes.clear();
    
    const Duration subdivision = clave.primarySubdivision();
    Duration minTimed<Note>Length = minTimed<Note>LengthInSubdivisions * subdivision;
    Duration maxTimed<Note>Length = maxTimed<Note>LengthInSubdivisions * subdivision;
    int numTimed<Note>s = getPotentialClaveTimed<Note>Count(clave, minTimed<Note>Length, maxTimed<Note>Length);
    int notesOnLeft = chooseNumberOfTimed<Note>sOnLeft(numTimed<Note>s);
    int notesOnRight = numTimed<Note>s - notesOnLeft;
    
    double otherTimed<Note>sOnLeft = 0;
    double otherTimed<Note>sOnRight = 0;
    
    for (auto note : other.notes)
    {
        if (other.isTimed<Note>OnLeft(note)) { otherTimed<Note>sOnLeft++; }
        else { otherTimed<Note>sOnRight++; }
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
            
            bool candidateOnLeft = notesNeededOnLeft > 0 && other.isTimed<Note>OnLeft(*noteIt);
            bool candidateOnRight = notesNeededOnRight > 0 && !other.isTimed<Note>OnLeft(*noteIt);
            
            Probability needToAcceptTimed<Note> = candidateOnLeft
                ? notesOnLeft / otherTimed<Note>sOnLeft
                : (candidateOnRight
                    ? notesOnRight / otherTimed<Note>sOnRight
                    : 0.);
            
            if (flipWeightedCoin(needToAcceptTimed<Note>)) { // todo: check previous note's time and make it more likely the longer it gets, and definitely not if it's 1 subdivision since last note
                candidateOnLeft ? notesNeededOnLeft-- : 0.;
                candidateOnRight ? notesNeededOnRight-- : 0.;
                clave.addTimed<Note>(claveTimed<Note>(noteIt->startTime));
            }
        }
        
        // Fill in gaps left.
        clave = fillClave(clave, notesNeededOnLeft, notesNeededOnRight, minTimed<Note>Length, maxTimed<Note>Length);
        constraintsBroken = clave.notes.empty();
        
        if (!constraintsBroken) {
            clave.notes.legato();
            for (auto note : clave.notes) {
                if (note.duration < minTimed<Note>Length || // bad length
                    note.duration > maxTimed<Note>Length) {
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
