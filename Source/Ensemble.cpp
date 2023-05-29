/*
  ==============================================================================

    Ensemble.cpp
    Created: 27 May 2023 3:20:25pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Ensemble.h"

/*
  ==============================================================================

    Ensemble.cpp
    Created: 15 Feb 2023 12:28:03pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/


#include "Ensemble.h"

vector<Phrase> Ensemble::at(Quarters ppqPosition) {
    vector<Phrase> result;
    for (auto entry : queue) {
        result.push_back(entry.second.schedule.at(ppqPosition));
    }
    // return schedule.at(ppqPosition);
    return result;
}

Phrase Ensemble::at(Quarters ppqPosition, VoiceName voiceName) {
    if (!hasVoice(voiceName)) { return Phrase(); } // TODO: use optional?
    return queue.at(voiceName).schedule.at(ppqPosition);
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

void Ensemble::queuePhrase(TimedEvent phraseTime, Phrase phrase)
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

