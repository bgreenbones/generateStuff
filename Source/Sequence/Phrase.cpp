//
//  Phrase.cpp
//  generateStuff - All
//
//  Created by Benjamin Greenwood on 8/14/22.
//

#include "Phrase.hpp"
#include <JuceHeader.h>


void Phrase::pitchQuantize() {
    if (notes.empty()) {
        return;
    }
    // Tonality firstScale = chordScales.drawByPosition(notes[0].startTime).scale;
    // int transpose = 0;
    for (Timed<Note>& note : notes) {
        Tonality scale = chordScales.drawByPosition(note.startTime).item.scale;
        // if (scale.root != firstScale.root) {
        //     Interval goUp = pitchClassInterval(firstScale.root, scale.root);
        //     transpose = (int)goUp;
        //     if (flipCoin()) {
        //         Interval goDown = invert(goUp);
        //         transpose = -(int)goDown;
        //     }
        // }
        // note.pitch += transpose;
        note.item.pitch = scale.quantize(note.item.pitch);
    }
}

Position Phrase::nextSubdivisionPosition(Position position) {
    Timed<Duration> atPosition = subdivisions.drawByPosition(position);
    Position next = position + atPosition.item;
    if (next >= time.endTime()) {
        return time.startTime;
    }
    
    Timed<Duration> atNextPosition = subdivisions.drawByPosition(next);
    if (atPosition.duration == atNextPosition.duration) { return next; }
    return atNextPosition.startTime;
}

Position Phrase::previousSubdivisionPosition(Position position) {
    Timed<Duration> atPosition = subdivisions.drawByPosition(position);
    Position previous = position - atPosition;
    if (previous < time.startTime) {
        Timed<Duration> atEnd = subdivisions.drawByPosition(time.endTime() - Beats(0.001));
        return time.endTime() - atEnd.item;
    }
    
    Timed<Duration> atPreviousPosition = subdivisions.drawByPosition(previous);
    if (atPosition.duration == atPreviousPosition.duration) { return previous; }
    return atPreviousPosition.endTime() - atPreviousPosition.item;
}


template <class T>
void Phrase::addTimedEvent(T toAdd, vector<T>& eventList) {
    eventList.push_back(toAdd);
    sort(eventList.begin(),
         eventList.end(),
         [](T const &a, T const &b) { return a.startTime < b.startTime; });
}

bool Phrase::addNote(Timed<Note> toAdd) {    
    bool fitsInPhrase = this->time.containsPartially(toAdd);
    if (fitsInPhrase) {
        notes.add(toAdd);
    }
    return fitsInPhrase;
}

bool Phrase::addSubdivision(Timed<Duration> toAdd) {
    bool fitsInPhrase = this->time.containsPartially(toAdd);
    if (fitsInPhrase) {
        subdivisions.add(toAdd);
    }
    return fitsInPhrase;
}

// TODO: call through to Sequence method. or actually this just shouldn't exist.
template <class T>
vector<T> Phrase::concatEvents(vector<T> eventList, vector<T> otherList) const {
    for (auto iter = otherList.begin(); iter < otherList.end(); iter++) {
        iter->startTime += time.duration;
        addTimedEvent<T>(*iter, eventList);
    }
    return eventList;
}

// TODO: call through to Sequence method?? or actually this just shouldn't exist.
void Phrase::tieSubdivisions() {
    if (subdivisions.size() <= 1) {
        return;
    }
    subdivisions.tie(true);
}

// TODO: call through to Sequence methods? or actually this just shouldn't exist...
template <class T>
vector<T> chopAfterDuration(vector<T> toChop, Duration duration) {
    if (toChop.empty()) {
        return toChop;
    }
    
    vector<T> filtered;
    copy_if (toChop.begin(),
             toChop.end(),
             back_inserter(filtered),
             [duration](T &t) { return t.startTime <= duration; });
    
    
    
    if (filtered.empty()) {
        return filtered;
    }
    
    vector<T> chopped;
    transform(filtered.begin(),
              filtered.end(),
              back_inserter(chopped),
              [duration](T &t) {
                bool tooLong = t.endTime() > duration;
                if (tooLong) {
                    t.duration =  duration - t.startTime;
                }
                return t; });
    
    return chopped;
}

Phrase Phrase::concat(Phrase other, bool useLastNote, bool keepDuration) const {
    // todo: what if i just want to concat subdivisions?
    // this currently destroys subdivision phrases (but preserves a single lengthy subdivision.
    // I imagine that later, and for other expressions we want to phrase,
    // we will want to have a concat method that can only concat some particular vectors...
    Phrase phrase(*this);
    Duration durationToPersist = phrase.time.duration;
    
    
    // // // // // // /// Sequence version.
    phrase.notes.concat(other.notes, useLastNote);
    phrase.subdivisions.concat(other.subdivisions, useLastNote);
    phrase.chordScales.concat(other.chordScales, useLastNote);
    if (keepDuration) {
        phrase.notes.chopAfterDuration(durationToPersist);
        phrase.subdivisions.chopAfterDuration(durationToPersist);
        phrase.chordScales.chopAfterDuration(durationToPersist);
    }
    phrase.subdivisions.tie(true);
    phrase.chordScales.tie(true);
    // // // // // // ///
    
    return phrase;
}


// TODO: we can actually optionally get subdivs from the subdivs sequence??
Phrase Phrase::parseMininotation(std::string phraseString, Duration subdivision) {
    double phraseLength = subdivision.asQuarters() * (double) Mininotation::getLength(phraseString);
    Phrase phrase = Phrase(subdivision, 0, Quarters(phraseLength));
    phrase.notes = notes.parseMininotation(phraseString, subdivision);
    return phrase;
}

Phrase Phrase::accents() const {
    Phrase result(*this);
    result.notes.erase(std::remove_if(result.notes.begin(), result.notes.end(),
                       [](Timed<Note> note) { return !(note.item.accented || note.item.velocity >= accentVelocity); }),
                       result.notes.end());
    return result;
}

Phrase Phrase::randomSubdivisions(vector<Duration> availableSubdivisions, vector<double> weights) const {
    Phrase result(*this);
    Sequence<Note> accents = result.accents().notes.legato();
    result.subdivisions.clear();
    
    if (weights.empty()) { weights.push_back(1); }
    double average = accumulate(weights.begin(), weights.end(), 0) / (double) weights.size();
    while (weights.size() < availableSubdivisions.size()) { weights.push_back(average); }
    while (weights.size() > availableSubdivisions.size()) { weights.pop_back(); }
    
    std::discrete_distribution<int> subdivisionIndexDistribution (weights.begin(), weights.end());
    
    for (Timed<Note> note : accents) {
        int subdivisionIndex = subdivisionIndexDistribution(getGen());
        Timed<Duration> randomSubdivision(Time(note.startTime, note.duration), availableSubdivisions[subdivisionIndex]);
        result.subdivisions.add(randomSubdivision);
    }
    
    return result;
}


Phrase Phrase::randomGhostSubdivision(Probability ghostProbability, Probability subdivisionProbability, Pitch pitch, Time span) const {
    Phrase result(*this);
    bool fillWholePhrase = span == nullTime;
    
    for (Timed<Duration> subdiv : result.subdivisions) {
        if (subdivisionProbability) {
            double numberOfGhosts = subdiv.duration / subdiv;
            for (double ghost = 0; ghost < numberOfGhosts; ghost++) {
                Position ghostPosition = subdiv.startTime + ghost * subdiv.item;
                if (fillWholePhrase || span.contains(ghostPosition)) {
                    if (notes.byStartPosition(ghostPosition).empty() && ghostProbability) {
                        result.notes.add(Timed<Note>(Time(ghostPosition, subdiv), Note(pitch, pppp)), PushBehavior::ignorePush, OverwriteBehavior::cutoff);
                    }
                }
            }
        }
    }
    
    return result;
}

Phrase Phrase::ghostSubdivision(Pitch pitch) const { return randomGhostSubdivision(1, 1, pitch); }


Phrase Phrase::randomGhostBursts(Duration minimumBurstLength, Duration maximumBurstLength,
                         Probability ghostProbabilityWithinBurst,
                         Probability burstProbability, Pitch pitch) const {
    Phrase result(*this);
    
    Position cursor = 0;
    double numberOfPossibleBurstLengths = (maximumBurstLength - minimumBurstLength) / sixteenths;
    
    while (cursor < result.time.duration) {
        Duration burstLength = minimumBurstLength + (rollDie(numberOfPossibleBurstLengths) * sixteenths);
        Time span(cursor, burstLength);
        result = burstProbability ? result.randomGhostSubdivision(ghostProbabilityWithinBurst, 1, pitch, span) : result;
        cursor += burstLength;
    }
    
    return result;
}
