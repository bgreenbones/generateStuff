/*
  ==============================================================================

    Harmony.cpp
    Created: 8 Mar 2023 10:50:55pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Harmony.h"
#include "Utility.h"


ChordScale harmony::randomChordScale(Position startTime, Duration duration) {
    PitchClass root = draw<PitchClass>(pitches);
    vector<Interval> scale = draw<vector<Interval>>(diatonicModes);
    ChordScale harm(root, scale, startTime, duration);
    return harm;
}


Phrase harmony::generateChordScales(Phrase fromPhrase, Probability chordProbabilityPerAccent) {
    fromPhrase.chordScales.clear();

    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.assignEvents(filter<Note>(notes, [](Note note) { return note.accented; }));
    accents.legato();
    
    if (accents.empty()) {
        int numberOfChords = fromPhrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            ChordScale chordScale = randomChordScale(startTime, chordLength);
            fromPhrase.chordScales.add(chordScale);
        }
    } else {
        for (Note accent : accents) {
            if (chordProbabilityPerAccent) {
                ChordScale chordScale = randomChordScale(accent.startTime, accent.duration);
                fromPhrase.chordScales.add(chordScale);
            }
        }
    }

    return fromPhrase;
}





vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
