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

// TODO: and a way of stepping from tonality to a closely related tonality, so we get smooth changes.
ChordScale harmony::newChordSameScale(ChordScale previousChordScale, Position startTime, Duration duration) {
    Tonality scale = previousChordScale.scale;
    ChordScale nextChordScale(scale, startTime, duration);
    
    if (scale.intervalsUpFromRoot.size() > 1) {
        PitchClass newChordRoot = pitchClassIncrement(scale.root, draw<Interval>(scale.intervalsUpFromRoot));
        while (newChordRoot == scale.root) {
            newChordRoot = pitchClassIncrement(scale.root, draw<Interval>(scale.intervalsUpFromRoot));
        }
        
        Pitch rootPitch = Pitch(newChordRoot, 0);
        Pitch thirdish = scale.multiStep(rootPitch, 2);
        Pitch fifthish = scale.multiStep(rootPitch, 4);
        
        Interval thirdInterval = thirdish - rootPitch;
        Interval fifthInterval = fifthish - rootPitch;
        
        nextChordScale.harmony = Tonality(newChordRoot, { unison, thirdInterval, fifthInterval });
    }

    return nextChordScale;
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
            ChordScale chordScale = fromPhrase.chordScales.empty()
                ? randomChordScale(startTime, chordLength)
                : newChordSameScale(fromPhrase.chordScales.back(), startTime, chordLength);
            fromPhrase.chordScales.add(chordScale);
        }
    } else {
        for (Note accent : accents) {
            if (chordProbabilityPerAccent) {
                ChordScale chordScale = fromPhrase.chordScales.empty()
                    ? randomChordScale(accent.startTime, accent.duration)
                    : newChordSameScale(fromPhrase.chordScales.back(), accent.startTime, accent.duration);
                fromPhrase.chordScales.add(chordScale);
            }
        }
    }

    return fromPhrase;
}





vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
