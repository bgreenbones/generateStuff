/*
  ==============================================================================

    Voice.h
    Created: 4 Mar 2023 11:05:06pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/
#pragma once

#include <stdio.h>
#include "Phrase.hpp"
#include "Rhythm.h"
#include "Harmony.h"
#include "Melody.h"
#include "VoiceParameters.h"
#include "Schedule.h"
#include <JuceHeader.h>

typedef string VoiceName;
using std::vector;

class Ensemble;

// TODO: change the key and the channel data into some other enum or struct type or something
static const VoiceName claveKey = "clave";
static const VoiceName cascaraKey = "cascara";
static const VoiceName subdivisionsKey = "subdivisions";
static const VoiceName harmonyKey = "harmony";
static const VoiceName bassKey = "bass";
static const VoiceName melodyKey = "melody";
//static const string pulseAndDisplaceKey = "pulseAndDisplace";
static const vector<VoiceName> voiceKeys = {
  claveKey, cascaraKey, harmonyKey, bassKey, melodyKey
};

static int claveChannel = 1;
static int cascaraChannel = 2;
static int subdivisionsChannel = 3;
static int harmonyChannel = 4;
static int bassChannel = 5;
static int melodyChannel = 6;
//static int pulseAndDisplaceChannel = 4;




class Voice {
public:
    Ensemble& ensemble;
    const VoiceName name;
    int midiChannel;
    bool mute;
    bool muteOrnamentation; // TODO: decouple this class from any idea of ornamentation/connecting. there should just be a base and a collection of related phrases. or just a collection.
    bool muteConnecting;
    VoiceSchedule schedule;

    void schedulePhrase(Time time, Phrase phrase) {
      phrase.voice = name;
      schedule.schedulePhrase(time, phrase);
    }

    virtual Phrase newPhrase() const = 0;
    virtual Phrase phraseFrom() const = 0;
    virtual Phrase variation() const = 0;

    virtual Phrase connecting(Probability connectingProb, // TODO: just get all this stuff from editor state instead of passing it in
                        Probability associationProb,
                        Probability connectingLengthProb) const;
    virtual Phrase ornament(Probability prob,
                             double breadth,
                             bool flams,
                             bool drags,
                             bool ruffs) const;
                             
    Phrase atOrEmpty(Position startTime) const;    
    string connectingKey() const { return name + "Connecting"; }
    string ornamentsKey() const { return name + "Ornaments"; }
    
    Voice(VoiceName name, int midiChannel, bool mute, Ensemble& ensemble):
        ensemble(ensemble), name(name), midiChannel(midiChannel), mute(mute) {};

    virtual ~Voice() {}
};

