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

void PlayQueue::clearVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).phrases.clear();
    queue.at(voiceName).rolls.clear();
    queue.at(voiceName).ornamentation.clear();
}

bool PlayQueue::toggleMuteVoice(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice &voice = queue.at(voiceName);
    bool newMuteState = !(voice.mute);
    queue.at(voiceName).mute = newMuteState;
    queue.at(voiceName).muteOrnamentation = newMuteState;
    queue.at(voiceName).muteRolls = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteRolls(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice &voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteRolls);
    queue.at(voiceName).muteRolls = newMuteState;
    return newMuteState;
}

bool PlayQueue::toggleMuteOrnamentation(VoiceName voiceName)
{
    if (!hasVoice(voiceName)) { return true; };
    Voice &voice = queue.at(voiceName);
    bool newMuteState = !(voice.muteOrnamentation);
    queue.at(voiceName).muteOrnamentation = newMuteState;
    return newMuteState;
}

void PlayQueue::queuePhrase(VoiceName voiceName, Phrase phrase)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).phrases.add(phrase, PushBehavior::ignore, true); // overwrite existing phrases if they overlap
}

void PlayQueue::queueRoll(VoiceName voiceName, Phrase phrase)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).rolls.add(phrase, PushBehavior::ignore, true); // overwrite existing phrases if they overlap
}

void PlayQueue::queueOrnamentation(VoiceName voiceName, Phrase phrase)
{
    if (!hasVoice(voiceName)) { return; }
    queue.at(voiceName).ornamentation.add(phrase, PushBehavior::ignore, true); // overwrite existing phrases if they overlap
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

