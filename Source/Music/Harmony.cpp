/*
  ==============================================================================

    Harmony.cpp
    Created: 8 Mar 2023 10:50:55pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Harmony.h"
#include "Utility.h"



ChordScale harmony::selectApproachAndGenerate(juce::String approach, Sequence<ChordScale> chordScales, Position startTime, Duration chordLength) {
    if (approach == randomHarmonyApproachKey || chordScales.empty()) {
        return randomChordScale(startTime, chordLength);
    }
    if (approach == diatonicHarmonyApproachKey) {
        return newChordSameScale(chordScales.back(), startTime, chordLength);
    }
    if (approach == smoothishModulationsHarmonyApprachKey) {
        return subtleModulations(chordScales.back(), startTime, chordLength);
    }
    return randomChordScale(startTime, chordLength);
};


ChordScale harmony::randomChordScale(Position startTime, Duration duration) {
    PitchClass root = draw<PitchClass>(pitches);
    vector<Interval> scale = draw<vector<Interval>>(diatonicModes);
    ChordScale harm(root, scale, startTime, duration);
    return harm;
}

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

ChordScale harmony::subtleModulations(ChordScale previousChordScale, Position startTime, Duration duration) {
    ChordScale newChordScale(previousChordScale.scale.smoothModulation(1, draw<Direction>({ up, down })), startTime, duration);
    newChordScale.harmony = previousChordScale.harmony;
    return newChordSameScale(newChordScale, startTime, duration);
}

Phrase harmony::generateChordScales(Phrase fromPhrase, GenerateStuffEditorState const& editorState) {
    fromPhrase.chordScales.clear();

    Sequence<Note> notes(fromPhrase.notes.toMonophonic());
    Sequence<Note> accents(notes);
    accents.assignEvents(filter<Note>(notes, [](Note note) { return note.accented; }));
    accents.legato();
    
    Probability chordProbabilityPerAccent = 0.6;
    juce::String harmonyApproach = editorState.getChoiceValue(harmonyApproachKey);
    
    
    if (accents.empty()) {
        int numberOfChords = fromPhrase.duration.asBars();
        bars startTimeInBars = 0;
        while(numberOfChords-- > 0) {
            Bars startTime(startTimeInBars++);
            Bars chordLength(min(numberOfChords--, 1));
            ChordScale chordScale = selectApproachAndGenerate(harmonyApproach, fromPhrase.chordScales, startTime, chordLength);
            fromPhrase.chordScales.add(chordScale);
        }
    } else {
        for (Note accent : accents) {
            if (chordProbabilityPerAccent) {
                ChordScale chordScale = selectApproachAndGenerate(harmonyApproach, fromPhrase.chordScales, accent.startTime, accent.duration);
                fromPhrase.chordScales.add(chordScale);
            }
        }
    }

    return fromPhrase;
}





vector<Pitch> harmony::randomChord() { return randomChordScale().harmony.randomVoicing(); }
