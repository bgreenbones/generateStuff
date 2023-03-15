/*
  ==============================================================================

    Harmony.cpp
    Created: 8 Mar 2023 10:50:55pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Harmony.h"



Tonality harmony::randomTonality(Position startTime, Duration duration) {
    PitchClass root = draw<PitchClass>(pitches);
    Interval third = draw<Interval>({ m3, M3 });
    Interval fifth = P5; // draw<Interval>({ b5, P5 });
    Interval seventh = draw<Interval>({ m7, M7 });
    Tonality harm(startTime, duration);
    harm.root = root;
    harm.intervalsUpFromRoot = { unison, third, fifth, seventh };
    return harm;
}


Phrase harmony::generateTonalities(Phrase fromPhrase, Probability chordProbabilityPerAccent) {
    fromPhrase.tonalities.clear();

    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.clear();
    copy_if(notes.begin(), notes.end(), back_inserter(accents), [](Note note) { return note.accented; });
    accents.legato();
    
    if (accents.empty()) {
        int numberOfChords = fromPhrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            Tonality tonality = randomTonality(startTime, chordLength);
            fromPhrase.tonalities.add(tonality);
        }
    } else {
        for (Note accent : accents) {
            if (chordProbabilityPerAccent) {
                Tonality tonality = randomTonality(accent.startTime, accent.duration);
                fromPhrase.tonalities.add(tonality);
            }
        }
    }

    return fromPhrase;
}





vector<Pitch> harmony::randomChord() { return randomTonality().randomVoicing(); }
