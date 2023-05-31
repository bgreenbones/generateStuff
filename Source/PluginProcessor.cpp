/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <algorithm>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "HostSettings.h"

juce::AudioProcessorParameterGroup getVoiceParameters(VoiceName voiceName, vector<Parameter> voiceParameters) {
    juce::AudioProcessorParameterGroup parameterGroup(voiceName, voiceName, ":");
    for (auto parameter : voiceParameters) {
        switch (parameter.type) {
            case ParameterType::knob:
                parameterGroup.addChild(make_unique<juce::AudioParameterFloat>(parameter.key, parameter.name, parameter.knobRange, parameter.defaultKnobValue, parameter.units));
                break;
            case ParameterType::button:
                parameterGroup.addChild(make_unique<juce::AudioParameterBool>(parameter.key, parameter.name, parameter.defaultButtonValue, parameter.units));
                break;
            case ParameterType::choice:
                parameterGroup.addChild(make_unique<juce::AudioParameterChoice>(parameter.key, parameter.name, parameter.choices, parameter.defaultChoiceIndex, parameter.units));
                break;
        }
    }
    return parameterGroup;
}

juce::AudioProcessorValueTreeState::ParameterLayout getParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;
    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getVoiceParameters(claveKey, claveParameters)));
    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getVoiceParameters(cascaraKey, cascaraParameters)));
    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getVoiceParameters(harmonyKey, harmonyParameters)));
    parameterLayout.add(make_unique<juce::AudioProcessorParameterGroup>(getVoiceParameters(bassKey, bassParameters)));
    return parameterLayout;
}

//==============================================================================
GenerateStuffAudioProcessor::GenerateStuffAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        editorState(GenerateStuffEditorState(apvts)),
        ensemble(Ensemble(editorState)),
        noteOffIssued(true),
        apvts(*this, nullptr, "parameters", getParameterLayout())
#endif
{
    for (int pitch = 0; pitch <= 127; pitch ++) { // yikes. for now this is the only thing that turns off note on messages when we stop playing
        for (int midiChannel = 1; midiChannel <= 16; midiChannel++) {
            auto noteOff = juce::MidiMessage::noteOff (midiChannel, pitch, (juce::uint8) 0);
            this->allNotesOff.push_back(noteOff);
        }
    }
    
    srand((int) time(0)); // init random number generator stuff

}

GenerateStuffAudioProcessor::~GenerateStuffAudioProcessor()
{
}

//==============================================================================
const juce::String GenerateStuffAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GenerateStuffAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GenerateStuffAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GenerateStuffAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GenerateStuffAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GenerateStuffAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GenerateStuffAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GenerateStuffAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GenerateStuffAudioProcessor::getProgramName (int index)
{
    return {};
}

void GenerateStuffAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void GenerateStuffAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    this->mSampleRate = sampleRate;
    samplesPerMinute = sampleRate * 60;
    this->mSamplesPerBlock = samplesPerBlock;
    
    auto playhead = getPlayHead();
    if (playhead != nullptr) {
        auto positionInfo = playhead->getPosition();
        HostSettings::instance().setTempo((positionInfo->getBpm()).orFallback(120));
        samplesPerBeat = samplesPerMinute / HostSettings::instance().getTempo();
    }
    
    this->noteOffIssued = true;
    this->currentNoteOff = 0;
}

void GenerateStuffAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GenerateStuffAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GenerateStuffAudioProcessor::updateTimeSignature(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newTimeSignatureJuce = (positionInfo->getTimeSignature())
                             .orFallback(juce::AudioPlayHead::TimeSignature());

    TimeSignature newTimeSignature = TimeSignature(newTimeSignatureJuce.numerator, newTimeSignatureJuce.denominator);
    if (HostSettings::instance().getTimeSignature() != newTimeSignature) {
        HostSettings::instance().setTimeSignature(newTimeSignature);
    }
    
    return;
}

void GenerateStuffAudioProcessor::updateBpm(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newBpm = (positionInfo->getBpm()).orFallback(120);
    
    if (newBpm != HostSettings::instance().getTempo()) {
        HostSettings::instance().setTempo(newBpm);
        samplesPerBeat = samplesPerMinute / newBpm;
    }
    
    return;
}


float GenerateStuffAudioProcessor::bufferTimeFromPpqTime(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo, const double ppqPosition, float ppqTime)
{
    if (ppqTime < ppqPosition) {
        // check the loop markers to make sure a previous phrase isn't coming up ahead
        bool isLooping = positionInfo->getIsLooping();
        if (isLooping) {
            juce::AudioPlayHead::LoopPoints no_loop = juce::AudioPlayHead::LoopPoints();
            no_loop.ppqStart = -1;
            no_loop.ppqEnd = -1;
            
            juce::AudioPlayHead::LoopPoints loop = (positionInfo->getLoopPoints()).orFallback(no_loop);
            
            if (loop != no_loop) {
                if (loop.ppqStart <= ppqTime) {
                    auto nowUntilLoopEnd = loop.ppqEnd - ppqPosition;
                    auto loopStartUntilNoteStart = ppqTime - loop.ppqStart;
                    return (nowUntilLoopEnd + loopStartUntilNoteStart) * samplesPerBeat + 2; // + 2 ? I guess we're adding two + 1s demonstrated below?
                }
            }
        }
    }
    return (ppqTime - ppqPosition) * samplesPerBeat + 1; // + 1 seems to line things up better...
}


bool GenerateStuffAudioProcessor::isPpqTimeInBuffer(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo, double ppqPosition, float ppqTime) {
    float bufferTime = bufferTimeFromPpqTime(positionInfo, ppqPosition, ppqTime);
//            return 0 <= bufferTime && bufferTime < mSamplesPerBlock;
    return 0 < bufferTime && bufferTime <= mSamplesPerBlock; // todo: get actual lowest and highest indexes for the buffer instead of these guesses
};


void GenerateStuffAudioProcessor::playNoteSequence(juce::MidiBuffer& midiMessages,
    juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo,
    const double ppqPosition, 
    Sequence<Note> noteSequence, 
    TimedEvent scheduledTime,
    int midiChannel) {

    int phraseLoopNumber = Quarters(ppqPosition) / noteSequence.parent.duration;
    Position scheduledStartTime = scheduledTime.startTime + noteSequence.parent.startTime + phraseLoopNumber * noteSequence.parent.duration;
    Position scheduledEndTime = scheduledTime.endTime();

    for (auto noteIt = noteSequence.begin(); noteIt != noteSequence.end(); ++noteIt) {
        Note note = *noteIt;
        
        double noteOnTimeInQuarters = editorState.getDisplacement() + scheduledStartTime + note.startTime;
        double noteOffTimeInQuarters = noteOnTimeInQuarters + note.duration;

        if (!isPpqTimeInBuffer(positionInfo, ppqPosition, noteOnTimeInQuarters) &&
            !isPpqTimeInBuffer(positionInfo, ppqPosition, noteOffTimeInQuarters)) {
            while (ppqPosition > noteOffTimeInQuarters) { // might as well set it to be in the future
                if (noteOnTimeInQuarters + noteSequence.parent.duration < scheduledEndTime) {
                    noteOnTimeInQuarters += noteSequence.parent.duration;
                    noteOffTimeInQuarters = noteOnTimeInQuarters + note.duration;
                } else {
                    break;
                }
            }
        }
        
        if (isPpqTimeInBuffer(positionInfo, ppqPosition, noteOnTimeInQuarters)) {
            auto noteOn = juce::MidiMessage::noteOn (midiChannel,
                                                    note.pitch,
                                                    (juce::uint8) note.velocity);
            double onTime = bufferTimeFromPpqTime(positionInfo, ppqPosition, noteOnTimeInQuarters);
            bool success = midiMessages.addEvent (noteOn, onTime);
            if (!success) {
                cout << "failed to add note on\n";
            }
            
        }
        
        if (isPpqTimeInBuffer(positionInfo, ppqPosition, noteOffTimeInQuarters)) {
            auto noteOff = juce::MidiMessage::noteOff (midiChannel,
                                                        note.pitch,
                                                        (juce::uint8) note.velocity);
            // double offTime = bufferTimeFromPpqTime(positionInfo, ppqPosition, noteOffTimeInQuarters) - 2; // maybe prevent some notes from missing their note off by ending them earlier
            double offTime = bufferTimeFromPpqTime(positionInfo, ppqPosition, noteOffTimeInQuarters);
            bool success = midiMessages.addEvent (noteOff, offTime);
            if (!success) {
                cout << "failed to add note off\n";
            }
        }
    }
}


void GenerateStuffAudioProcessor::playPlayables(
        juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo,
        juce::MidiBuffer& midiMessages)
{

    const double ppqPosition = (positionInfo->getPpqPosition()).orFallback(0);
    
    if (positionInfo->getIsLooping()) { // if we're looping, do stuff a little different each time around to keep things interesting.
        juce::AudioPlayHead::LoopPoints loop = (positionInfo->getLoopPoints()).orFallback(juce::AudioPlayHead::LoopPoints());
        
        // TODO: not the most robust way of scheduling tasks, but should work for now.
        if (isPpqTimeInBuffer(positionInfo, ppqPosition, loop.ppqStart)) {
           if (loopTasks.isScheduled()) {
            loopTasks.performTasks();
            loopTasks.complete(); }
        } else if (!loopTasks.isScheduled()) {
            loopTasks.schedule();
        }
        
        float samplesUntilLoop = bufferTimeFromPpqTime(positionInfo, ppqPosition, loop.ppqStart);
        if (samplesUntilLoop <= allNotesOff.size()) {
          noteOffIssued = issueNoteOff(midiMessages);        
        } else {
          noteOffIssued = false;
          currentNoteOff = 0;
        }
    }

    vector<
    Phrase> phrases = ensemble.at(ppqPosition);
    for (auto phrase : phrases) 
    {
        string voiceName = phrase.voice;
        Voice &voice = ensemble.getVoice(voiceName);
        if (voice.mute) { continue; }
        int midiChannel = voice.midiChannel;

        for (TimedEvent scheduledTime : phrase.schedule) {
            playNoteSequence(midiMessages, positionInfo, ppqPosition, phrase.notes, scheduledTime, midiChannel);
            if (!voice.muteConnecting) {
                playNoteSequence(midiMessages, positionInfo, ppqPosition, phrase.connectingNotes, scheduledTime, midiChannel);
            }
            if (!voice.muteOrnamentation) {
                playNoteSequence(midiMessages, positionInfo, ppqPosition, phrase.ornamentationNotes, scheduledTime, midiChannel);
            }
        }
    }
}


void GenerateStuffAudioProcessor::issueNoteOff(int midiChannel) {
    for (int pitch = 0; pitch <= 127; pitch++) {
        auto noteOff = juce::MidiMessage::noteOff (midiChannel, pitch, (juce::uint8) 0);
        midiMessageQueue.push_back(noteOff);
   }
}
   
bool GenerateStuffAudioProcessor::issueNoteOff(juce::MidiBuffer& midiMessages) {
    if (currentNoteOff < allNotesOff.size()) {
        size_t upperBound = std::min(size_t(currentNoteOff + mSamplesPerBlock), allNotesOff.size());
        for (auto i = currentNoteOff; i < upperBound; i++) {
            bool success = midiMessages.addEvent(allNotesOff[i], 0);
            if (!success) {
//                            throw exception();
            }
        }
        currentNoteOff = upperBound;
    }
    if (currentNoteOff >= allNotesOff.size()) {
        return true;
    }
    return false;
}

void GenerateStuffAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();
    if (!midiMessages.isEmpty()) {
        midiMessages.clear();
    }
    
    while (midiMessageQueue.size() > 0) {
        midiMessages.addEvent(midiMessageQueue.back(), 0);
        midiMessageQueue.pop_back();
    }

    auto playhead = getPlayHead();
    if (playhead != nullptr) {
        auto positionInfo = playhead->getPosition();
        updateTimeSignature(positionInfo);
        updateBpm(positionInfo);
        if (!songIsWritten) {
            ensemble.writeSong();
            songIsWritten = true;
        }
        auto isPlaying = positionInfo->getIsPlaying();
        if (isPlaying) {
            playPlayables(positionInfo, midiMessages);
        } else {
            if (!noteOffIssued) {
              noteOffIssued = issueNoteOff(midiMessages);
            }
        }
    }
    
    juce::ScopedNoDenormals noDenormals;
     // In case we have more outputs than inputs, this code clears any output
     // channels that didn't contain input data, (because these aren't
     // guaranteed to be empty - they may contain garbage).
     // This is here to avoid people getting screaming feedback
     // when they first compile a plugin, but obviously you don't need to keep
     // this code if your algorithm always overwrites all the output channels.
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
         buffer.clear (i, 0, buffer.getNumSamples());
}

//==============================================================================
bool GenerateStuffAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GenerateStuffAudioProcessor::createEditor()
{
    return new GenerateStuffAudioProcessorEditor (*this);
}

//==============================================================================
void GenerateStuffAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GenerateStuffAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GenerateStuffAudioProcessor();
}
