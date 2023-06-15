
/*
  ==============================================================================

    Ensemble.cpp
    Created: 15 Feb 2023 12:28:03pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/


#include "Ensemble.h"

void Ensemble::writeSong() {
    Timed form = Timed(0, Bars(8));
    
    Duration harmonyPhraseLength = Bars(2);
    Duration chordsPhraseLength = Bars(4);
    Duration leadPhraseLength = Bars(8);

    Phrase clavePhrase = rhythm::randomClave(emptyPhrase(claveKey), 2, 4); // min and max note length
    Phrase cascaraPhrase = rhythm::cascaraFrom(clavePhrase);
    Phrase harmony = harmony::generateChordScales(clavePhrase.loop(harmonyPhraseLength),
        HarmonyApproach::smoothishModulations, 0.6, 0.7); // harmonic density and probability per accent
    Phrase chordsPhrase = harmony::smoothVoicings(harmony.loop(chordsPhraseLength), clavePhrase.loop(chordsPhraseLength));
    Phrase bassPhrase = melody::bass(harmony.loop(chordsPhraseLength), clavePhrase.loop(chordsPhraseLength), 1, 4, { 1 }); // burst length min, max, and note length choices
    // Phrase leadPhrase = melody::streamOfConsciousness(harmony.loop(leadPhraseLength));
    // Phrase leadPhrase = melody::repeatingShape(harmony.loop(leadPhraseLength), Beats(3));
    Phrase leadPhrase = melody::streamWithThemes(harmony.loop(leadPhraseLength));
    chordsPhrase = harmony::voicingFills(chordsPhrase.loop(leadPhraseLength),
                                         {bassPhrase.loop(leadPhraseLength), leadPhrase});
    
    dynamics::randomFlux(clavePhrase.notes);
    dynamics::randomFlux(cascaraPhrase.notes);
    dynamics::randomFlux(chordsPhrase.notes);
    dynamics::randomFlux(bassPhrase.notes);
    dynamics::randomFlux(leadPhrase.notes);

    clave.schedulePhrase(form, clavePhrase);
    cascara.schedulePhrase(form, cascaraPhrase);
    chords.schedulePhrase(form, chordsPhrase);
    bass.schedulePhrase(form, bassPhrase);
    lead.schedulePhrase(form, leadPhrase);
}


vector<Phrase> Ensemble::at(Quarters ppqPosition) {
    vector<Phrase> result;
    for (auto entry : queue) {
        Phrase const *phrase = entry.second.schedule.at(ppqPosition);
        if (phrase != NULL && phrase != nullptr) {
            result.push_back(*phrase);
        }
    }
    return result;
}

Phrase Ensemble::at(Quarters ppqPosition, VoiceName voiceName) {
    if (!hasVoice(voiceName)) { return Phrase(); } // TODO: use optional?
    return queue.at(voiceName).atOrEmpty(ppqPosition);
}

bool Ensemble::hasVoice(VoiceName voiceName)
{
    return queue.find(voiceName) != queue.end();
}

Voice& Ensemble::getVoice(VoiceName voiceName)
{
//   if (!hasVoice(voiceName)) { return Voice("voice not found", -1, true, qp); } // TODO: use optional?
    // if (!hasVoice(voiceName)) { return Voice("voice not found", -1, true, *this); } // TODO: use optional?
    return queue.at(voiceName);
}

bool Ensemble::hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength) { return true; }
bool Ensemble::doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength) { return !hasPhrase(voiceName, startTime, phraseLength); }

void Ensemble::clearVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return; }
    // queue.at(voiceName).base.clear();
    // queue.at(voiceName).initPhraseVector();
    queue.at(voiceName).schedule.clear();
    
}

bool Ensemble::toggleMuteVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice &voice = queue.at(voiceName);
    bool newMuteState = !(voice.mute);
    voice.mute = newMuteState;
    voice.muteOrnamentation = newMuteState;
    voice.muteConnecting = newMuteState;
    return newMuteState;
}

bool Ensemble::toggleMuteConnecting(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice &voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteConnecting);
    voice.muteConnecting = newMuteState;
    return newMuteState;
}

bool Ensemble::toggleMuteConnecting(VoiceName voiceName, bool newMuteState)
{
    if (!hasVoice(voiceName)) { return true; };
    queue.at(voiceName).muteConnecting = newMuteState;
    return newMuteState;
}

bool Ensemble::toggleMuteOrnamentation(VoiceName voiceName, bool newMuteState)
{
    if (!hasVoice(voiceName)) { return true; };
    queue.at(voiceName).muteOrnamentation = newMuteState;
    return newMuteState;
}

bool Ensemble::toggleMuteOrnamentation(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice& voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteOrnamentation);
    queue.at(voiceName).muteOrnamentation = newMuteState;
    return newMuteState;
}

void Ensemble::queuePhrase(Timed phraseTime, Phrase phrase)
{
    // schedule.schedulePhrase(phraseTime, phrase);
    VoiceName voiceName = phrase.voice;
    if (!hasVoice(voiceName)) { return; }
    Voice &voice = queue.at(voiceName);
    voice.schedule.schedulePhrase(phraseTime, phrase);
    // if (phrase.isPolyphonic()) { voice.base = voice.base.toPolyphonic(); } // TODO: more rigorous way of syncing parameters between queued phrase and generated phrase?
    // Phrase baseCopy = voice.base;
    // voice.base = voice.base.insert(phrase, OverwriteBehavior::erase); // overwrite if overlap
//    if (phrase.notes.size() != voice.base.notes.size()) { // FOR BUG DETECTIN
//        bool messedUp = true;
//        voice.base = baseCopy.insert(phrase, OverwriteBehavior::erase); // overwrite if overlap
//
   // }
    // voice.initPhraseVector(); // TODO: lol 
}


void Ensemble::setMidiChannel(VoiceName voiceName, int newMidiChannel)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).midiChannel = newMidiChannel;
}

int Ensemble::getMidiChannel(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return -1; }
    return queue.at(voiceName).midiChannel;
}

