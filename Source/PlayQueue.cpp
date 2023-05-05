/*
  ==============================================================================

    PlayQueue.cpp
    Created: 15 Feb 2023 12:28:03pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/


#include "PlayQueue.h"



bool PlayQueue::hasVoice(VoiceName voiceName)
{
    return queue.find(voiceName) != queue.end();
}

Voice PlayQueue::getVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return Voice("voice not found", -1, true); } // TODO: use optional?
    return queue.at(voiceName);
}

bool PlayQueue::hasPhrase(VoiceName voiceName, Position startTime, Duration phraseLength) { return true; }
bool PlayQueue::doesntHavePhrase(VoiceName voiceName, Position startTime, Duration phraseLength) { return !hasPhrase(voiceName, startTime, phraseLength); }

void PlayQueue::clearVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).base.clear();
    queue.at(voiceName).initPhraseVector();
}

bool PlayQueue::toggleMuteVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice voice = queue.at(voiceName);
    bool newMuteState = !(voice.mute);
    queue.at(voiceName).mute = newMuteState;
    queue.at(voiceName).muteOrnamentation = newMuteState;
    queue.at(voiceName).muteConnecting = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteConnecting(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteConnecting);
    queue.at(voiceName).muteConnecting = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteConnecting(VoiceName voiceName, bool newMuteState)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice voice = queue.at(voiceName);
    queue.at(voiceName).muteConnecting = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteOrnamentation(VoiceName voiceName, bool newMuteState)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice voice = queue.at(voiceName);
    queue.at(voiceName).muteOrnamentation = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteOrnamentation(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteOrnamentation);
    queue.at(voiceName).muteOrnamentation = newMuteState;
    return newMuteState;
}

void PlayQueue::queuePhrase(VoiceName voiceName, Phrase phrase)
{
    if (!hasVoice(voiceName)) { return; }
    Voice &voice = queue.at(voiceName);
    if (phrase.isPolyphonic()) { voice.base = voice.base.toPolyphonic(); } // TODO: more rigorous way of syncing parameters between queued phrase and generated phrase?
    Phrase baseCopy = voice.base;
    voice.base = voice.base.insert(phrase, OverwriteBehavior::erase); // overwrite if overlap
    schedule.at(scheduleTimes.back()).clear();
    schedule.at(scheduleTimes.back()).push_back(voice.base);
//    if (phrase.notes.size() != voice.base.notes.size()) { // FOR BUG DETECTIN
//        bool messedUp = true;
//        voice.base = baseCopy.insert(phrase, OverwriteBehavior::erase); // overwrite if overlap
//
//    }
    voice.initPhraseVector(); // TODO: lol
}


void PlayQueue::setMidiChannel(VoiceName voiceName, int newMidiChannel)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).midiChannel = newMidiChannel;
}

int PlayQueue::getMidiChannel(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return -1; }
    return queue.at(voiceName).midiChannel;
}

